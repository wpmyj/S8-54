namespace Controller_S8_53 {
    partial class Governor {
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
            this.SuspendLayout();
            // 
            // Governor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "Governor";
            this.Size = new System.Drawing.Size(55, 56);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Governor_Paint);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Governor_MouseDown);
            this.MouseEnter += new System.EventHandler(this.Governor_MouseEnter);
            this.MouseLeave += new System.EventHandler(this.Governor_MouseLeave);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Governor_MouseMove);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Governor_MouseUp);
            this.ResumeLayout(false);

        }

        #endregion
    }
}
