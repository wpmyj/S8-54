using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Controller_S8_53
{

    public class TextBoxIntValue : TextBox
    {
        private int maxValue = 0;

        public event EventHandler<EventArgs> FieldFilled;

        public TextBoxIntValue(int maxValue)
        {
            this.maxValue = maxValue;
        }

        private bool ConditionPressEvent(KeyPressEventArgs e)
        {
            return ((e.KeyChar < '0' || e.KeyChar > '9') && (int)e.KeyChar != 8 && (int)e.KeyChar != 37 && (int)e.KeyChar != 39 && (int)e.KeyChar != 46);
        }

        private bool ConditionUpDownEvent(KeyEventArgs e)
        {
            return ((e.KeyValue < '0' || e.KeyValue > '9') && e.KeyValue != 8 && e.KeyValue != 37 && e.KeyValue != 39 && e.KeyValue != 46);
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            if(ConditionPressEvent(e))
            {
                e.Handled = true;
                return;
            }
            base.OnKeyPress(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if(ConditionUpDownEvent(e))
            {
                e.Handled = true;
                return;
            }
            base.OnKeyDown(e);
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            Console.WriteLine((int)e.KeyValue);
            if(ConditionUpDownEvent(e))
            {
                e.Handled = true;
                return;
            }
            base.OnKeyUp(e);
            string txt = Text;
            if(txt != "")
            {
                int value = Convert.ToInt32(txt);
                if(value > maxValue)
                {
                    int pos = SelectionStart;
                    Text = Convert.ToString(value / 10);
                    SelectionStart = pos;
                }
            }
        }
    }


    public partial class TextBoxIP : UserControl
    {
        public TextBoxIP()
        {
            InitializeComponent();
        }
    }
}
