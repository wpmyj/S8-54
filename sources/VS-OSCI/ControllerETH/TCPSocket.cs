using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.Threading;
using System.Drawing;
using System.Net;
using System.Net.Sockets;


public class StateObject
{
    public Socket workSocket = null;
    public const int BufferSize = 1024 * 100;
    public byte[] buffer = new byte[BufferSize];
    public StringBuilder sb = new StringBuilder();
}


namespace Controller_S8_53
{
    public class TCPSocket
    {
        private static Socket client;

        private static Mutex mutex = new Mutex();

        private Mutex mutexRecv = new Mutex();

        private static ManualResetEvent connectDone = new ManualResetEvent(false);

        private static List<byte> recvBuffer = new List<byte>();   // Здесь будут храниться считанные байты

        public TCPSocket()
        {
        }

        public void Stop()
        {
            if (client.Connected)
            {
                client.Disconnect(false);
                client.Close();
            }
        }

        public void SendString(string str)
        {
            mutex.WaitOne();

            if (client.Connected)
            {
                byte[] byteData = Encoding.ASCII.GetBytes(":" + str + "\x0d\x0a");
                client.Send(byteData);
            }

            mutex.ReleaseMutex();
        }

        static private string ReadLine()
        {
            byte[] byteData = new byte[500];

            try
            {
                int size = client.Receive(byteData);

                byte[] buffer = new byte[size];

                for(int i = 0; i < size; i++)
                {
                    buffer[i] = byteData[i];
                }

                string line = Encoding.ASCII.GetString(buffer);
                return line.Substring(0, line.Length - 2);
            }
            catch (SystemException)
            {

            }
            return null;
        }

        private static void ConnectCallback(IAsyncResult ar)
        {
            client.EndConnect(ar);
            connectDone.Set();
        }

        static private void Connect()
        {
            byte[] addr = new byte[4];
            addr[0] = 192;
            addr[1] = 168;
            addr[2] = 1;
            addr[3] = 200;

            client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            client.BeginConnect(new IPAddress(addr), 7, new AsyncCallback(ConnectCallback), client);
        }

        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                mutexRecv.WaitOne();

                StateObject state = (StateObject)ar.AsyncState;

                int bytesRead = client.EndReceive(ar);

                if(bytesRead > 0)
                {
                    for(int i = 0; i < bytesRead; i++)
                    {
                        recvBuffer.Add(state.buffer[i]);
                    }

                    client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReceiveCallback), state);
                }

                mutexRecv.ReleaseMutex();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public bool Open()
        {
            try
            {
                Connect();

                connectDone.WaitOne(1000);

                if (!client.Connected)
                {   
                    return false;
                }

                SendString("REQUEST ?");
                string answer = ReadLine();
                SendString("REQUEST ?");
                answer = ReadLine();

                if (answer != "S8-53/1" && answer != "S8-54")
                {
                    client.Disconnect(false);
                    client.Close();
                }

                StateObject state = new StateObject();
                state.workSocket = client;

                client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReceiveCallback), state);
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            return client.Connected;
        }

        private static byte[] data = new byte[16 * 1024];

        public bool IsOpen()
        {
            if (client == null)
            {
                return false;
            }
            return client.Connected;
        }

        public int BytesToRead()
        {
            int retValue = 0;

            try
            {
                mutexRecv.WaitOne();

                retValue = recvBuffer.Count;

                mutexRecv.ReleaseMutex();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            return retValue;
        }

        public void Read(byte[] buffer, int start, int numBytes)
        {
            try
            {
                mutexRecv.WaitOne();

                recvBuffer.CopyTo(0, buffer, start, numBytes);
                recvBuffer.RemoveRange(0, numBytes);

                mutexRecv.ReleaseMutex();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
    }
}
