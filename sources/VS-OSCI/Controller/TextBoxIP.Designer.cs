namespace Controller_S8_53
{
    partial class TextBoxIP
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if(disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {

            this.tbByte1 = new TextBoxIntValue(255);
            this.tbByte2 = new TextBoxIntValue(255);
            this.tbByte3 = new TextBoxIntValue(255);
            this.tbByte4 = new TextBoxIntValue(255);
            this.tbPort = new TextBoxIntValue(65535);

            this.SuspendLayout();
            // 
            // TextBoxIP
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "TextBoxIP";
            this.Size = new System.Drawing.Size(160, 27);
            this.ResumeLayout(false);

            //
            const int sizeX = 28;
            const int sizeY = 10;
            const int delta = 0;

            int x = 0;
            int y = 0;

            this.tbByte1.Location = new System.Drawing.Point(x, y);
            this.tbByte1.Size = new System.Drawing.Size(sizeX, sizeY);

            x += sizeX + delta;
            this.tbByte2.Location = new System.Drawing.Point(x, y);
            this.tbByte2.Size = new System.Drawing.Size(sizeX, sizeY);

            x += sizeX + delta;
            this.tbByte3.Location = new System.Drawing.Point(x, y);
            this.tbByte3.Size = new System.Drawing.Size(sizeX, sizeY);

            x += sizeX + delta;
            this.tbByte4.Location = new System.Drawing.Point(x, y);
            this.tbByte4.Size = new System.Drawing.Size(sizeX, sizeY);

            x += sizeX + delta + 5;
            this.tbPort.Location = new System.Drawing.Point(x, y);
            this.tbPort.Size = new System.Drawing.Size(sizeX + 10, sizeY);

            this.Controls.Add(this.tbByte1);
            this.Controls.Add(this.tbByte2);
            this.Controls.Add(this.tbByte3);
            this.Controls.Add(this.tbByte4);
            this.Controls.Add(this.tbPort);
        }

        #endregion

        private TextBoxIntValue tbByte1;
        private TextBoxIntValue tbByte2;
        private TextBoxIntValue tbByte3;
        private TextBoxIntValue tbByte4;
        private TextBoxIntValue tbPort;

    }
}
