using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace Controller_S8_53
{
    public class TcpSocket : Client
    {
        Socket socket;

        public TcpSocket ()
        {
            typeClient = 1;
        }

        override public bool Open(string address)
        {
            IPEndPoint ipPoint = new IPEndPoint(IPAddress.Parse(address), 7);
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Connect(ipPoint);
            string remotePoint = socket.RemoteEndPoint.ToString();
            return remotePoint.Length > 0;
        }

        override public void Stop()
        {
            if(socket != null)
            {
                socket.Close();
            }
        }

        public override void SendString(string str)
        {
            byte[] msg = Encoding.ASCII.GetBytes(str);
            try
            {
                socket.Send(msg);
            }
            catch
            {
                int i = 0;
            }
        }
    }
}
