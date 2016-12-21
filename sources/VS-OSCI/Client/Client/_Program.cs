using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;

public class StateObject
{
    public Socket workSocket = null;

    public const int BufferSize = 1024;

    public byte[] buffer = new byte[BufferSize];

    public StringBuilder sb = new StringBuilder();
}

namespace Client
{
    class Program
    {
        private const int port = 7;

        private static ManualResetEvent connectDone = new ManualResetEvent(false);

        private static ManualResetEvent sendDone = new ManualResetEvent(false);

        private static ManualResetEvent receiveDone = new ManualResetEvent(false);

        private static String response = String.Empty;

        private static void StartClient()
        {
            try
            {
                byte[] addr = new byte[4];
                addr[0] = 192;
                addr[1] = 168;
                addr[2] = 1;
                addr[3] = 200;
                IPAddress ipAddress = new IPAddress(addr);
                IPEndPoint remoteEP = new IPEndPoint(ipAddress, port);

                Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                client.BeginConnect(remoteEP, new AsyncCallback(ConnectCallback), client);
                connectDone.WaitOne();

                Send(client, "This is a test");
                sendDone.WaitOne();

                Receive(client);
                receiveDone.WaitOne();

                Console.WriteLine("Response received: {0}", response);

                client.Shutdown(SocketShutdown.Both);
                client.Close();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private static void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                client.EndConnect(ar);

                Console.WriteLine("Socket connected to {0}", client.RemoteEndPoint.ToString());

                connectDone.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private static void Receive(Socket client)
        {
            try
            {
                StateObject state = new StateObject();
                state.workSocket = client;

                client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReceiveCallback), state);
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private static void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                StateObject state = (StateObject)ar.AsyncState;
                Socket client = state.workSocket;

                int bytesRead = client.EndReceive(ar);

                if(bytesRead > 0)
                {
                    string dat = Encoding.ASCII.GetString(state.buffer, 0, bytesRead);
                    Console.WriteLine(dat);
                    state.sb.Append(dat);

                    client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReceiveCallback), state);
                }
                else
                {
                    if(state.sb.Length > 1)
                    {
                        response = state.sb.ToString();
                    }
                    receiveDone.Set();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        private static void Send(Socket client, String data)
        {
            byte[] byteData = Encoding.ASCII.GetBytes(data);

            client.BeginSend(byteData, 0, byteData.Length, 0, new AsyncCallback(SendCallback), client);
        }

        private static void SendCallback(IAsyncResult ar)
        {
            try
            {
                Socket client = (Socket)ar.AsyncState;

                int byteSent = client.EndSend(ar);
                Console.WriteLine("Sent {0} bytes to server", byteSent);

                sendDone.Set();
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        static int Main(string[] args)
        {
            StartClient();
            return 0;
        }
    }
}
