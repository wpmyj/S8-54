using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace Controller_S8_53
{
    public class Client
    {
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
        virtual public void SendString(string str)
        {

        }
        virtual public SerialPort GetSerialPort()
        {
            return new SerialPort();
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
