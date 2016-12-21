namespace Controller_S8_53 {
    partial class MainForm {
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent() {
            this.btnMenu = new System.Windows.Forms.Button();
            this.btnF1 = new System.Windows.Forms.Button();
            this.btnF2 = new System.Windows.Forms.Button();
            this.btnF3 = new System.Windows.Forms.Button();
            this.btnF4 = new System.Windows.Forms.Button();
            this.btnF5 = new System.Windows.Forms.Button();
            this.btnCursors = new System.Windows.Forms.Button();
            this.btnDisplay = new System.Windows.Forms.Button();
            this.btnMemory = new System.Windows.Forms.Button();
            this.btnMeasures = new System.Windows.Forms.Button();
            this.btnHelp = new System.Windows.Forms.Button();
            this.btnService = new System.Windows.Forms.Button();
            this.btnChannel0 = new System.Windows.Forms.Button();
            this.btnChannel1 = new System.Windows.Forms.Button();
            this.btnTime = new System.Windows.Forms.Button();
            this.btnTrig = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnConnect = new System.Windows.Forms.Button();
            this.govTBase = new Controller_S8_53.Governor();
            this.govRange1 = new Controller_S8_53.Governor();
            this.govSet = new Controller_S8_53.Governor();
            this.govRange0 = new Controller_S8_53.Governor();
            this.govTrigLev = new Controller_S8_53.Governor();
            this.govTShift = new Controller_S8_53.Governor();
            this.govRShift1 = new Controller_S8_53.Governor();
            this.govRShift0 = new Controller_S8_53.Governor();
            this.display = new Controller_S8_53.Display();
            this.SuspendLayout();
            // 
            // btnMenu
            // 
            this.btnMenu.Location = new System.Drawing.Point(652, 82);
            this.btnMenu.Name = "btnMenu";
            this.btnMenu.Size = new System.Drawing.Size(51, 23);
            this.btnMenu.TabIndex = 2;
            this.btnMenu.Text = "МЕНЮ";
            this.btnMenu.UseVisualStyleBackColor = true;
            this.btnMenu.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnMenu.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnF1
            // 
            this.btnF1.Location = new System.Drawing.Point(652, 145);
            this.btnF1.Name = "btnF1";
            this.btnF1.Size = new System.Drawing.Size(51, 23);
            this.btnF1.TabIndex = 2;
            this.btnF1.Text = "1";
            this.btnF1.UseVisualStyleBackColor = true;
            this.btnF1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnF1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnF2
            // 
            this.btnF2.Location = new System.Drawing.Point(652, 205);
            this.btnF2.Name = "btnF2";
            this.btnF2.Size = new System.Drawing.Size(51, 23);
            this.btnF2.TabIndex = 2;
            this.btnF2.Text = "2";
            this.btnF2.UseVisualStyleBackColor = true;
            this.btnF2.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnF2.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnF3
            // 
            this.btnF3.Location = new System.Drawing.Point(652, 271);
            this.btnF3.Name = "btnF3";
            this.btnF3.Size = new System.Drawing.Size(51, 23);
            this.btnF3.TabIndex = 2;
            this.btnF3.Text = "3";
            this.btnF3.UseVisualStyleBackColor = true;
            this.btnF3.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnF3.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnF4
            // 
            this.btnF4.Location = new System.Drawing.Point(652, 340);
            this.btnF4.Name = "btnF4";
            this.btnF4.Size = new System.Drawing.Size(51, 23);
            this.btnF4.TabIndex = 2;
            this.btnF4.Text = "4";
            this.btnF4.UseVisualStyleBackColor = true;
            this.btnF4.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnF4.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnF5
            // 
            this.btnF5.Location = new System.Drawing.Point(652, 411);
            this.btnF5.Name = "btnF5";
            this.btnF5.Size = new System.Drawing.Size(51, 23);
            this.btnF5.TabIndex = 2;
            this.btnF5.Text = "5";
            this.btnF5.UseVisualStyleBackColor = true;
            this.btnF5.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnF5.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnCursors
            // 
            this.btnCursors.Location = new System.Drawing.Point(715, 250);
            this.btnCursors.Name = "btnCursors";
            this.btnCursors.Size = new System.Drawing.Size(60, 23);
            this.btnCursors.TabIndex = 2;
            this.btnCursors.Text = "Курсоры";
            this.btnCursors.UseVisualStyleBackColor = true;
            this.btnCursors.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnCursors.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnDisplay
            // 
            this.btnDisplay.Location = new System.Drawing.Point(789, 250);
            this.btnDisplay.Name = "btnDisplay";
            this.btnDisplay.Size = new System.Drawing.Size(60, 23);
            this.btnDisplay.TabIndex = 2;
            this.btnDisplay.Text = "Дисплей";
            this.btnDisplay.UseVisualStyleBackColor = true;
            this.btnDisplay.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnDisplay.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnMemory
            // 
            this.btnMemory.Location = new System.Drawing.Point(863, 250);
            this.btnMemory.Name = "btnMemory";
            this.btnMemory.Size = new System.Drawing.Size(60, 23);
            this.btnMemory.TabIndex = 2;
            this.btnMemory.Text = "Память";
            this.btnMemory.UseVisualStyleBackColor = true;
            this.btnMemory.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnMemory.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnMeasures
            // 
            this.btnMeasures.Location = new System.Drawing.Point(715, 289);
            this.btnMeasures.Name = "btnMeasures";
            this.btnMeasures.Size = new System.Drawing.Size(60, 23);
            this.btnMeasures.TabIndex = 2;
            this.btnMeasures.Text = "Измер";
            this.btnMeasures.UseVisualStyleBackColor = true;
            this.btnMeasures.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnMeasures.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnHelp
            // 
            this.btnHelp.Location = new System.Drawing.Point(789, 289);
            this.btnHelp.Name = "btnHelp";
            this.btnHelp.Size = new System.Drawing.Size(60, 23);
            this.btnHelp.TabIndex = 2;
            this.btnHelp.Text = "Помощь";
            this.btnHelp.UseVisualStyleBackColor = true;
            this.btnHelp.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnHelp.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnService
            // 
            this.btnService.Location = new System.Drawing.Point(863, 289);
            this.btnService.Name = "btnService";
            this.btnService.Size = new System.Drawing.Size(60, 23);
            this.btnService.TabIndex = 2;
            this.btnService.Text = "Сервис";
            this.btnService.UseVisualStyleBackColor = true;
            this.btnService.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnService.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnChannel0
            // 
            this.btnChannel0.Location = new System.Drawing.Point(721, 341);
            this.btnChannel0.Name = "btnChannel0";
            this.btnChannel0.Size = new System.Drawing.Size(44, 23);
            this.btnChannel0.TabIndex = 2;
            this.btnChannel0.Text = "Кан.1";
            this.btnChannel0.UseVisualStyleBackColor = true;
            this.btnChannel0.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnChannel0.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnChannel1
            // 
            this.btnChannel1.Location = new System.Drawing.Point(785, 341);
            this.btnChannel1.Name = "btnChannel1";
            this.btnChannel1.Size = new System.Drawing.Size(43, 23);
            this.btnChannel1.TabIndex = 2;
            this.btnChannel1.Text = "Кан.2";
            this.btnChannel1.UseVisualStyleBackColor = true;
            this.btnChannel1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnChannel1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnTime
            // 
            this.btnTime.Location = new System.Drawing.Point(848, 341);
            this.btnTime.Name = "btnTime";
            this.btnTime.Size = new System.Drawing.Size(54, 23);
            this.btnTime.TabIndex = 2;
            this.btnTime.Text = "Разв";
            this.btnTime.UseVisualStyleBackColor = true;
            this.btnTime.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnTime.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnTrig
            // 
            this.btnTrig.Location = new System.Drawing.Point(918, 341);
            this.btnTrig.Name = "btnTrig";
            this.btnTrig.Size = new System.Drawing.Size(54, 23);
            this.btnTrig.TabIndex = 2;
            this.btnTrig.Text = "Синхр";
            this.btnTrig.UseVisualStyleBackColor = true;
            this.btnTrig.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnTrig.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(650, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(62, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Установка";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(714, 415);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Вольт/дел";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(780, 415);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(60, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Вольт/дел";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(849, 415);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(63, 13);
            this.label4.TabIndex = 5;
            this.label4.Text = "Время/дел";
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(934, 270);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(46, 23);
            this.btnStart.TabIndex = 6;
            this.btnStart.Text = "ПУСК";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button_MouseDown);
            this.btnStart.MouseUp += new System.Windows.Forms.MouseEventHandler(this.button_MouseUp);
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(924, 12);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(56, 41);
            this.btnConnect.TabIndex = 2;
            this.btnConnect.Text = "Подкл";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // govTBase
            // 
            this.govTBase.Location = new System.Drawing.Point(853, 441);
            this.govTBase.Name = "govTBase";
            this.govTBase.Size = new System.Drawing.Size(40, 40);
            this.govTBase.TabIndex = 3;
            this.govTBase.TypeGovernor = Controller_S8_53.TypeGov.Big;
            this.govTBase.ValueToSend = "TBASE";
            this.govTBase.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govTBase.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govRange1
            // 
            this.govRange1.Location = new System.Drawing.Point(787, 441);
            this.govRange1.Name = "govRange1";
            this.govRange1.Size = new System.Drawing.Size(40, 40);
            this.govRange1.TabIndex = 3;
            this.govRange1.TypeGovernor = Controller_S8_53.TypeGov.Big;
            this.govRange1.ValueToSend = "RANGE2";
            this.govRange1.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govRange1.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govSet
            // 
            this.govSet.Location = new System.Drawing.Point(663, 32);
            this.govSet.Name = "govSet";
            this.govSet.Size = new System.Drawing.Size(25, 25);
            this.govSet.TabIndex = 3;
            this.govSet.TypeGovernor = Controller_S8_53.TypeGov.Small;
            this.govSet.ValueToSend = "SET";
            this.govSet.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govSet.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govRange0
            // 
            this.govRange0.Location = new System.Drawing.Point(721, 441);
            this.govRange0.Name = "govRange0";
            this.govRange0.Size = new System.Drawing.Size(40, 40);
            this.govRange0.TabIndex = 3;
            this.govRange0.TypeGovernor = Controller_S8_53.TypeGov.Big;
            this.govRange0.ValueToSend = "RANGE1";
            this.govRange0.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govRange0.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govTrigLev
            // 
            this.govTrigLev.Location = new System.Drawing.Point(934, 382);
            this.govTrigLev.Name = "govTrigLev";
            this.govTrigLev.Size = new System.Drawing.Size(25, 25);
            this.govTrigLev.TabIndex = 1;
            this.govTrigLev.TypeGovernor = Controller_S8_53.TypeGov.Small;
            this.govTrigLev.ValueToSend = "TRIGLEV";
            this.govTrigLev.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govTrigLev.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govTShift
            // 
            this.govTShift.Location = new System.Drawing.Point(862, 382);
            this.govTShift.Name = "govTShift";
            this.govTShift.Size = new System.Drawing.Size(25, 25);
            this.govTShift.TabIndex = 1;
            this.govTShift.TypeGovernor = Controller_S8_53.TypeGov.Small;
            this.govTShift.ValueToSend = "TSHIFT";
            this.govTShift.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govTShift.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govRShift1
            // 
            this.govRShift1.Location = new System.Drawing.Point(795, 382);
            this.govRShift1.Name = "govRShift1";
            this.govRShift1.Size = new System.Drawing.Size(25, 25);
            this.govRShift1.TabIndex = 1;
            this.govRShift1.TypeGovernor = Controller_S8_53.TypeGov.Small;
            this.govRShift1.ValueToSend = "RSHIFT2";
            this.govRShift1.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govRShift1.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // govRShift0
            // 
            this.govRShift0.Location = new System.Drawing.Point(732, 382);
            this.govRShift0.Name = "govRShift0";
            this.govRShift0.Size = new System.Drawing.Size(25, 25);
            this.govRShift0.TabIndex = 1;
            this.govRShift0.TypeGovernor = Controller_S8_53.TypeGov.Small;
            this.govRShift0.ValueToSend = "RSHIFT1";
            this.govRShift0.RotateLeft += new System.EventHandler<System.EventArgs>(this.governor_RotateLeft);
            this.govRShift0.RotateRight += new System.EventHandler<System.EventArgs>(this.governor_RotateRight);
            // 
            // display
            // 
            this.display.Location = new System.Drawing.Point(2, 5);
            this.display.Name = "display";
            this.display.Size = new System.Drawing.Size(644, 484);
            this.display.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(992, 493);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.govTBase);
            this.Controls.Add(this.govRange1);
            this.Controls.Add(this.govSet);
            this.Controls.Add(this.govRange0);
            this.Controls.Add(this.btnF5);
            this.Controls.Add(this.btnF4);
            this.Controls.Add(this.btnF3);
            this.Controls.Add(this.btnF2);
            this.Controls.Add(this.btnF1);
            this.Controls.Add(this.btnTrig);
            this.Controls.Add(this.btnTime);
            this.Controls.Add(this.btnService);
            this.Controls.Add(this.btnMemory);
            this.Controls.Add(this.btnChannel1);
            this.Controls.Add(this.btnHelp);
            this.Controls.Add(this.btnDisplay);
            this.Controls.Add(this.btnChannel0);
            this.Controls.Add(this.btnMeasures);
            this.Controls.Add(this.btnCursors);
            this.Controls.Add(this.btnMenu);
            this.Controls.Add(this.govTrigLev);
            this.Controls.Add(this.govTShift);
            this.Controls.Add(this.govRShift1);
            this.Controls.Add(this.govRShift0);
            this.Controls.Add(this.display);
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(1000, 520);
            this.MinimumSize = new System.Drawing.Size(1000, 520);
            this.Name = "MainForm";
            this.Text = "C8-54 ETH Пульт оператора";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Display display;
        private Governor govRShift0;
        private System.Windows.Forms.Button btnMenu;
        private Governor govRange0;
        private System.Windows.Forms.Button btnF1;
        private System.Windows.Forms.Button btnF2;
        private System.Windows.Forms.Button btnF3;
        private System.Windows.Forms.Button btnF4;
        private System.Windows.Forms.Button btnF5;
        private System.Windows.Forms.Button btnCursors;
        private System.Windows.Forms.Button btnDisplay;
        private System.Windows.Forms.Button btnMemory;
        private System.Windows.Forms.Button btnMeasures;
        private System.Windows.Forms.Button btnHelp;
        private System.Windows.Forms.Button btnService;
        private System.Windows.Forms.Button btnChannel0;
        private System.Windows.Forms.Button btnChannel1;
        private System.Windows.Forms.Button btnTime;
        private System.Windows.Forms.Button btnTrig;
        private Governor govRShift1;
        private Governor govTShift;
        private Governor govTrigLev;
        private Governor govRange1;
        private Governor govTBase;
        private Governor govSet;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnConnect;
    }
}

