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
    public partial class LineEditID : UserControl {
        public LineEditID() {
            InitializeComponent();
        }

        private void tb1_KeyDown(object sender, KeyEventArgs e) {
            /*
            if(e.KeyChar == (char)Keys.Return) {
                tb2.Focus();
            }
            if(tb1.TextLength == 3) {
                tb2.Focus();
            }
             * */
        }

        private void tb2_KeyPress(object sender, KeyPressEventArgs e) {
            if(e.KeyChar == (char)Keys.Return) {
                tb3.Focus();
            }
        }

        private void tb3_KeyPress(object sender, KeyPressEventArgs e) {
            if(e.KeyChar == (char)Keys.Return) {
                tb4.Focus();
            }
        }

        private void tb4_KeyPress(object sender, KeyPressEventArgs e) {
            if(e.KeyChar == (char)Keys.Return) {
            }
        }
    }
}
