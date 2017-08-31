using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Controller_S8_53
{
    /*
    public enum TypeClient
    {
        TypeClient_ComPort,
        TypeClient_Socket
    };
    */

    public class Client
    {
        public int typeClient;  // 0 - ComPort, 1 - TcpSocket

        virtual public int GetTypeClient()
        {
            return typeClient;
        }

        virtual public string[] GetPorts()
        {
            return new string[1];
        }
        virtual public bool IsOpen()
        {
            return false;
        }
        virtual public bool Open(int numPort)
        {
            return false;
        }
        virtual public bool Open(string address)
        {
            return false;
        }
        virtual public void SendString(string str)
        {

        }
        virtual public bool DeviceConnectToPort(int numPort)
        {
            return false;
        }
        virtual public void Stop()
        {

        }
        virtual public int BytesToRead()
        {
            return 0;
        }

        virtual public void Read(byte[] buffer, int start, int length)
        {

        }
    }
}
