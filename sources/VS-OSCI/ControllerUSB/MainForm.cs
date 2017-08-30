using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Controller_S8_53 {

    public partial class MainForm : Form {

        private bool needForDisconnect = false;
        private ComPort port = new ComPort();
        private Dictionary<Button, string> mapButtons = new Dictionary<Button, string>();

        private Queue<string> commands = new Queue<string>();

        public MainForm() {
            InitializeComponent();

            mapButtons.Add(btnChannel0,    "CHAN1");
            mapButtons.Add(btnService,     "SERVICE");
            mapButtons.Add(btnChannel1,    "CHAN2");
            mapButtons.Add(btnDisplay,     "DISPLAY");
            mapButtons.Add(btnTime,        "TIME");
            mapButtons.Add(btnMemory,      "MEMORY");
            mapButtons.Add(btnTrig,        "TRIG");
            mapButtons.Add(btnStart,       "START");
            mapButtons.Add(btnCursors,     "CURSORS");
            mapButtons.Add(btnMeasures,    "MEASURES");
            mapButtons.Add(btnHelp,        "HELP");
            mapButtons.Add(btnMenu,        "MENU");
            mapButtons.Add(btnF1,          "1");
            mapButtons.Add(btnF2,          "2");
            mapButtons.Add(btnF3,          "3");
            mapButtons.Add(btnF4,          "4");
            mapButtons.Add(btnF5,          "5");

            Display.EndFrameEvent += OnEndFrameEvent;

            btnUpdatePorts_Click(null, null);
        }

        private void button_MouseDown(object sender, MouseEventArgs e) {
            commands.Enqueue("KEY:" + StringToSendForButton(sender) + " DOWN");
        }

        private void button_MouseUp(object sender, MouseEventArgs e) {
            commands.Enqueue("KEY:" + StringToSendForButton(sender) + " UP");
        }

        private void governor_RotateLeft(object sender, EventArgs e) {
            commands.Enqueue("GOV:" + ((Governor)sender).ValueToSend + " LEFT");
        }

        private void governor_RotateRight(object sender, EventArgs e) {
            commands.Enqueue("GOV:" + ((Governor)sender).ValueToSend + " RIGHT");
        }

        private string StringToSendForButton(object btn) {
            return mapButtons[(Button)btn];
        }

        private void btnUpdatePorts_Click(object sender, EventArgs e) {
            string[] ports = port.GetPorts();
            cbPorts.Items.Clear();
            cbPorts.Items.AddRange(ports);
            cbPorts.SelectedIndex = ports.Length - 1;
        }

        private void btnConnect_Click(object sender, EventArgs e) {
            if(port.IsOpen()) {
                needForDisconnect = true;
                btnConnectUSB.Text = "Подкл";
            } else {
                if(port.Open(cbPorts.SelectedIndex)) {
                    btnConnectUSB.Text = "Откл";
                    port.SendString("DISPLAY:AUTOSEND 1");
                    display.StartDrawing(port.GetSerialPort());
                    needForDisconnect = false;
                }
            }
        }

        private void cbPorts_SelectedIndexChanged(object sender, EventArgs e)
        {
            btnConnectUSB.Enabled = port.DeviceConnectToPort(cbPorts.SelectedIndex);
        }

        private void OnEndFrameEvent(object sender, EventArgs e)
        {
            if (needForDisconnect)
            {
                port.Stop();
            }
            else
            {
                while (commands.Count != 0)
                {
                    port.SendString(commands.Dequeue());
                }
                port.SendString("DISPLAY:AUTOSEND 2");
                display.StartDrawing(port.GetSerialPort());
            }
        }
    }
}
