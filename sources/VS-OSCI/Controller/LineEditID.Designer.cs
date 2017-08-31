namespace Controller_S8_53 {
    partial class LineEditID {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing) {
            if(disposing && (components != null)) {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.tb1 = new System.Windows.Forms.TextBox();
            this.tb2 = new System.Windows.Forms.TextBox();
            this.tb3 = new System.Windows.Forms.TextBox();
            this.tb4 = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // tb1
            // 
            this.tb1.Location = new System.Drawing.Point(1, 1);
            this.tb1.MaxLength = 3;
            this.tb1.Name = "tb1";
            this.tb1.Size = new System.Drawing.Size(40, 20);
            this.tb1.TabIndex = 12;
            this.tb1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tb1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tb1_KeyDown);
            // 
            // tb2
            // 
            this.tb2.Location = new System.Drawing.Point(42, 1);
            this.tb2.MaxLength = 3;
            this.tb2.Name = "tb2";
            this.tb2.Size = new System.Drawing.Size(40, 20);
            this.tb2.TabIndex = 11;
            this.tb2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tb2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tb2_KeyPress);
            // 
            // tb3
            // 
            this.tb3.Location = new System.Drawing.Point(83, 1);
            this.tb3.MaxLength = 3;
            this.tb3.Name = "tb3";
            this.tb3.Size = new System.Drawing.Size(40, 20);
            this.tb3.TabIndex = 10;
            this.tb3.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tb3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tb3_KeyPress);
            // 
            // tb4
            // 
            this.tb4.Location = new System.Drawing.Point(124, 1);
            this.tb4.MaxLength = 3;
            this.tb4.Name = "tb4";
            this.tb4.Size = new System.Drawing.Size(40, 20);
            this.tb4.TabIndex = 9;
            this.tb4.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tb4.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tb4_KeyPress);
            // 
            // LineEditID
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tb4);
            this.Controls.Add(this.tb1);
            this.Controls.Add(this.tb2);
            this.Controls.Add(this.tb3);
            this.Name = "LineEditID";
            this.Size = new System.Drawing.Size(166, 25);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tb1;
        private System.Windows.Forms.TextBox tb2;
        private System.Windows.Forms.TextBox tb3;
        private System.Windows.Forms.TextBox tb4;
    }
}
