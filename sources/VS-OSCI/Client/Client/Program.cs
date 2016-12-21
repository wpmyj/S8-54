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

        private static ManualResetEvent sendDone = new ManualResetEvent(false);

        private static ManualResetEvent receiveDone = new ManualResetEvent(false);

        private static String response = String.Empty;

        private static Socket client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

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
        
        private static void ConnectCallback(IAsyncResult ar)
        {
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
                    Console.WriteLine(" >> " + dat);
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
            catch(Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        static int Main(string[] args)
        {
            // Коннектимся
            byte[] addr = new byte[4];
            addr[0] = 192;
            addr[1] = 168;
            addr[2] = 1;
            addr[3] = 200;

            client.BeginConnect(new IPAddress(addr), port, new AsyncCallback(ConnectCallback), client);

            while(!client.Connected) { };

            StateObject state = new StateObject();
            state.workSocket = client;

            // Настраиваемся на приём
            client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0, new AsyncCallback(ReceiveCallback), state);


            while(true)
            {
                string data = Console.ReadLine();
                if(data == "close")
                {
                    client.Close();
                    while(true) { };
                }
                Send(client, ":" + data + "\x0d\x0a");
            };
            
            return 0;
        }
    }
}
