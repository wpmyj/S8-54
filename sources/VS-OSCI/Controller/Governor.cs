using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Controller_S8_53 {

    public enum TypeGov : short {
        Small,
        Big
    };

    public partial class Governor : UserControl {

        private TypeGov typeGov = TypeGov.Small;
        private string valueToSend;
        private int maximum;
        private double angle = 0.0;
        private double prevIntegerNumberOfDegress = 0.0f;
        private double integerNumberOfDegress = 0.0f;
        private int mouseY;
        private Point downMousePos;
        private bool mouseIsDown = false;

        public event EventHandler<EventArgs> RotateLeft;
        public event EventHandler<EventArgs> RotateRight;

        public Governor() {
            InitializeComponent();
            Tune();
        }

        private void Governor_Paint(object sender, PaintEventArgs e) {
            this.Width = this.Height = typeGov == TypeGov.Small ? 25 : 40;
            e.Graphics.DrawArc(new Pen(Color.Black), 0, 0, this.Width - 1, this.Height - 1, 0, 360);
            double x = Math.Cos(integerNumberOfDegress * 3.14 / 180) * this.Width * 0.25 + this.Width / 2;
            double y = Math.Sin(integerNumberOfDegress * 3.14 / 180) * this.Width * 0.25 + this.Width / 2;
            double radius = this.Width * 0.1f;

            e.Graphics.DrawArc(new Pen(Color.Black), (float)(x - radius), (float)(y - radius), (float)radius * 2, (float)radius * 2, 0, 360);
        }

        private void Tune() {
            maximum = typeGov == TypeGov.Small ? 23 : 11;
        }

        public string GetValueToSend() {
            return valueToSend;
        }

        public void SetValueToSend(string val) {
            valueToSend = val;
        }

        public TypeGov GetTypeGovernor() {
            return typeGov;
        }

        public void SetTypeGovernor(TypeGov type) {
            typeGov = type;
        }

        #region "Properties"

        public TypeGov TypeGovernor {
            get { return GetTypeGovernor(); }
            set { SetTypeGovernor(value); }
        }

        public string ValueToSend {
            get { return GetValueToSend(); }
            set { SetValueToSend(value); }
        }

        #endregion

        private void Governor_MouseEnter(object sender, EventArgs e) {
            this.Cursor = Cursors.Hand;
        }

        private void Governor_MouseLeave(object sender, EventArgs e) {
            this.Cursor = Cursors.Arrow;
        }

        private void Governor_MouseDown(object sender, MouseEventArgs e) {
            Cursor.Hide();
            downMousePos = Cursor.Position;
            mouseY = e.Y;
            mouseIsDown = true;
        }

        private void Governor_MouseUp(object sender, MouseEventArgs e) {
            Cursor.Show();
            Cursor.Position = downMousePos;
            mouseIsDown = false;
        }

        void OnRotateLeft() {
            EventHandler<EventArgs> handler = RotateLeft;
            if(handler != null) {
                handler(this, new EventArgs());
            }
        }

        void OnRotateRight() {
            EventHandler<EventArgs> handler = RotateRight;
            if(handler != null) {
                handler(this, new EventArgs());
            }
        }

        private void Governor_MouseMove(object sender, MouseEventArgs e) {
            if(!mouseIsDown) {
                return;
            }

            double deltaY = mouseY - e.Y;
            mouseY = e.Y;

            if(typeGov == TypeGov.Big) {
                deltaY /= 2.0;
            }

            CalculateAngles(deltaY);
        }

        private void CalculateAngles(double deltaY) {
            angle += deltaY;
            while(angle < 0.0) {
                angle += 360.0;
            }
            while(angle >= 360.0) {
                angle -= 360.0;
            }
            CalculateIntegerNumberOfDegress();
        }

        private void CalculateIntegerNumberOfDegress() {
            int stepOfCircle = 360 / (maximum + 1);
            int numberSectors = ((int)angle) / stepOfCircle;

            integerNumberOfDegress = numberSectors * stepOfCircle;

            if(integerNumberOfDegress != prevIntegerNumberOfDegress) {
                if(prevIntegerNumberOfDegress == 0 && integerNumberOfDegress == maximum) {
                    OnRotateLeft();
                } 
                else if(prevIntegerNumberOfDegress == maximum && integerNumberOfDegress == 0) 
                {
                    OnRotateRight();
                } 
                else 
                {
                    if(integerNumberOfDegress > prevIntegerNumberOfDegress) {
                        OnRotateRight();
                    } else {
                        OnRotateLeft();
                    }
                }
                prevIntegerNumberOfDegress = integerNumberOfDegress;
                Invalidate();
            }
        }
    }
}
