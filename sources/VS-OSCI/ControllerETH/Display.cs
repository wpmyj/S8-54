using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Drawing.Imaging;

namespace Controller_S8_53
{
    public partial class Display : UserControl
    {

        private static bool needForEndDrawing = false;

        enum Command : byte
        {
            SET_COLOR = 1,
            FILL_REGION = 2,
            END_SCENE = 3,
            DRAW_HLINE = 4,
            DRAW_VLINE = 5,
            SET_POINT = 6,
            DRAW_SIGNAL_LINES = 7,
            DRAW_TEXT = 8,
            SET_PALETTE = 9,
            SET_FONT = 10,
            DRAW_VLINES_ARRAY = 13,
            DRAW_SIGNAL_POINTS = 14,
            DRAW_MULTI_HPOINT_LINES_2 = 17,
            DRAW_MULTI_VPOINT_LINES = 18,
            LOAD_FONT = 19
        };

        [DllImport("gdi32")]
        private extern static int SetDIBitsToDevice(HandleRef hDC, int xDest, int yDest, int dwWidth, int dwHeight, int XSrc, int YSrc, int uStartScan,
            int cScanLines, ref int lpvBits, ref BITMAPINFO lpbmi, uint fuColorUse);

        [StructLayout(LayoutKind.Sequential)]
        private struct BITMAPINFOHEADER
        {
            public int bihSize;
            public int bihWidth;
            public int bihHeight;
            public short bihPlanes;
            public short bihBitCount;
            public int bihCompression;
            public int bihSizeImage;
            public double bihXPelsPerMeter;
            public double bihClrUsed;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct BITMAPINFO
        {
            public BITMAPINFOHEADER biHeader;
            public int biColors;
        }

        BITMAPINFO bmi;
        private static int[] buffer = new int[320 * 240];
        private static Graphics g;

        public class Symbol
        {
            public Symbol()
            {
                bytes = new int[8];
            }
            public int width;
            public int []bytes;
        }

        public class MyFont 
        {
            public MyFont()
            {
                symbols = new Symbol[256];
            }
            public int height;
            public Symbol []symbols;
        }

        private static MyFont []fonts = new MyFont[4];

        private static Color color = Color.Black;

        private static Color[] colors = new Color[16];

        private static Thread processThread = null;

        private static PictureBox display;

        private static Stopwatch stopWatch = new Stopwatch();

        private static TCPSocket socket;

        private static int currentFont = 0;

        private static bool running = false;

        static public event EventHandler<EventArgs> EndFrameEvent;

        public Display()
        {
            InitializeComponent();

            bmi = new BITMAPINFO
            {
                biHeader =
                {
                    bihBitCount = 32,
                    bihPlanes = 1,
                    bihSize = 40,
                    bihWidth = 320,
                    bihHeight = 240,
                    bihSizeImage = 320 * 240 * 4
                }
            };

            g = pbDisplay.CreateGraphics();
            display = pbDisplay;

            stopWatch.Start();
        }

        private static void SetPoint(int x, int y)
        {
            if(x >= 0 && x < 320 && y >= 0 && y < 240)
            {
                buffer[x + y * 320] = color.ToArgb();
            }
        }

        private static void EndScene()
        {
            Bitmap bmp = new Bitmap(320, 240, PixelFormat.Format32bppArgb);
            BitmapData bmData = bmp.LockBits(new Rectangle(0, 0, 320, 240), ImageLockMode.ReadWrite, bmp.PixelFormat);
            bmData.Stride = 320 * 4;
            Marshal.Copy(buffer, 0, bmData.Scan0, 320 * 240);
            bmp.UnlockBits(bmData);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.DrawImage(bmp, new Rectangle(0, 0, 639, 479), 0, 0, 319, 239, GraphicsUnit.Pixel);
        }

        private static void FillRegion(int x, int y, int width, int height)
        {
            for(int col = x; col <= x + width; col++)
            {
                for(int row = y; row <= y + height; row++)
                {
                    SetPoint(col, row);
                }
            }
        }

        private static void DrawHLine(int y, int x0, int x1)
        {
            if(x1 < x0)
            {
                int temp = x0;
                x0 = x1;
                x1 = temp;
            }
            for(int x = x0; x <= x1; x++)
            {
                SetPoint(x, y);
            }
        }

        private static void DrawVLine(int x, int y0, int y1)
        {
            if(y1 < y0)
            {
                int temp = y0;
                y0 = y1;
                y1 = temp;
            }
            for(int y = y0; y <= y1; y++)
            {
                SetPoint(x, y);
            }
        }

        private static void SetColor(uint numColor)
        {
            if(numColor < 16)
            {
                color = colors[numColor];
            }
        }

        private static int GetBit(int value, int numBit)
        {
            return (value >> numBit) & 1;
        }

        private static void DrawText(int x, int y, char[] str)
        {
            for(int numSymbol = 0; numSymbol < str.Length; numSymbol++)
            {
                int symbol = (int)str[numSymbol];
                int height = fonts[currentFont].height;
                int width = fonts[currentFont].symbols[symbol].width;

                for(int row = 0; row < height; row++)
                {
                    for(int col = 0; col < width; col++)
                    {
                        if(GetBit(fonts[currentFont].symbols[symbol].bytes[row], col + (8 - width)) == 1)
                        {
                            SetPoint(x + (width - col), y + row);
                        }
                    }
                }

                x += width + 1;
            }
        }

        private static void SetPalette(byte numColor, UInt16 color)
        {
            colors[numColor] = ColorFromUINT16(color);
        }

        private static Color ColorFromUINT16(UInt16 color)
        {
            float kRed = 255.0f / 31;
            float kGreen = 255.0f / 63;
            float kBlue = 255.0f / 31;
            int r = (color >> 11) & 31;
            int g = (color >> 5) & 63;
            int b = color & 31;
            return Color.FromArgb((int)(r * kRed), (int)(g * kGreen), (int)(b * kBlue));
        }


        private static void Processing()
        {
            while(running)
            {
                byte command = (byte)int8();

                if((Command)command == Command.SET_COLOR)
                {
                 //   Console.WriteLine("SetColor");
                    SetColor((uint)int8());
                }
                else if((Command)command == Command.SET_PALETTE)
                {
                 //   Console.WriteLine("SetPalette");
                    SetPalette((byte)int8(), (ushort)int16());
                }
                else if((Command)command == Command.FILL_REGION)
                {
                 //   Console.WriteLine("FillRegion");
                    FillRegion(int16(), int8(), int16(), int8());
                }
                else if((Command)command == Command.END_SCENE)
                {
                 //   Console.WriteLine("EndScene");
                    EndScene();
                    running = false;
                }
                else if((Command)command == Command.DRAW_HLINE)
                {
                 //   Console.WriteLine("DrawHLine");
                    DrawHLine(int8(), int16(), int16());
                }
                else if((Command)command == Command.DRAW_VLINE)
                {
                 //   Console.WriteLine("DrawVLine");
                    DrawVLine(int16(), int8(), int8());
                }
                else if((Command)command == Command.SET_POINT)
                {
                 //   Console.WriteLine("SetPoint");
                    SetPoint(int16(), int8());
                }
                else if((Command)command == Command.DRAW_SIGNAL_POINTS)
                {
                    int x0 = int16();

                 //   Console.WriteLine("DRAW_SIGNAL_POINTS");
                    for(int i = 0; i < 281; i++)
                    {
                        SetPoint(x0 + i, int8());
                    }
                 //   Console.WriteLine("EXIT                   DRAW_SIGNAL_POINTS");
                }
                else if((Command)command == Command.DRAW_SIGNAL_LINES)
                {
                    int x0 = int16();

                    int prevX = int8();

                 //   Console.WriteLine("DRAW_SIGNAL_LINES");
                    for(int i = 0; i < 280; i++)
                    {
                        int nextX = int8();
                        DrawVLine(x0 + i, prevX, nextX);
                        prevX = nextX;
                    }
                 //   Console.WriteLine("EXIT                     DRAW_SIGNAL_LINES");
                }
                else if((Command)command == Command.DRAW_MULTI_HPOINT_LINES_2)
                {
                 //   Console.WriteLine("DRAW_MULTI_HPOINT_LINES_2");
                    int numLines = int8();
                    int x0 = int16();
                    int numPoints = int8();
                    int dX = int8();
                    for(int i = 0; i < numLines; i++)
                    {
                        int y = int8();

                        for(int point = 0; point < numPoints; point++)
                        {
                            SetPoint(x0 + dX * point, y);
                        }
                    }
                 //   Console.WriteLine("EXIT                          DRAW_MULTI_HPOINT_LINES_2");
                }
                else if((Command)command == Command.DRAW_MULTI_VPOINT_LINES)
                {
                 //   Console.WriteLine("DRAW_MULTI_VPOINT_LINES");
                    int numLines = int8();
                    int y0 = int8();
                    int numPoints = int8();
                    int dY = int8();
                    int8();
                    for(int i = 0; i < numLines; i++)
                    {
                        int x = int16();

                        for(int point = 0; point < numPoints; point++)
                        {
                            SetPoint(x, y0 + dY * point);
                        }
                    }
                 //   Console.WriteLine("EXIT                           DRAW_MULTI_VPOINT_LIINES");
                }
                else if((Command)command == Command.DRAW_VLINES_ARRAY)
                {
                 //   Console.WriteLine("DRAW_VLINES_ARRAY");
                    int x0 = int16();
                    int numLines = int8();
                    for(int i = 0; i < numLines; i++)
                    {
                        DrawVLine(x0 + i, int8(), int8());
                    }
                 //   Console.WriteLine("EXIT                              DRAW_VLINES_ARRAY");
                }
                else if((Command)command == Command.LOAD_FONT)
                {
                 //   Console.WriteLine("LOAD_FONT");
                    int typeFont = int8();
                    if (typeFont < 4)
                    {
                        fonts[typeFont] = new MyFont();
                        fonts[typeFont].height = int8();
                        int8();
                        int8();
                        int8();
                        for (int i = 0; i < 256; i++)
                        {
                            fonts[typeFont].symbols[i] = new Symbol();
                            fonts[typeFont].symbols[i].width = int8();
                            for (int j = 0; j < 8; j++)
                            {
                                fonts[typeFont].symbols[i].bytes[j] = int8();
                            }
                        }
                    }
                 //   Console.WriteLine("EXIT                               LOAD_FONT");
                }
                else if((Command)command == Command.SET_FONT)
                {
                 //   Console.WriteLine("SET_FONT");
                    currentFont = int8();
                }
                else if((Command)command == Command.DRAW_TEXT)
                {
               //     Console.WriteLine("DRAW_TEXT");
                    int x0 = int16();
                    int y0 = int8();
                    int numSymbols = int8();
                    char[] str = new char[numSymbols];
                    for(int i = 0; i < numSymbols; i++)
                    {
                        str[i] = (char)int8();
                    }
                    DrawText(x0, y0, str);
                }
                else
                {
                 //   Console.WriteLine("Неизвестная команда " + command);
                }
            }
            if (needForEndDrawing == false)
            {
                SendEndFrameEvent();
            }
        }

        private static byte[] recData = new byte[0];
        private static int pointer = 1;

        private static int int8()
        {
            if(pointer < recData.Length)
            {
                return recData[pointer++];
            }
            while(socket.BytesToRead() == 0)
            {
            };
            int length = socket.BytesToRead();
            recData = new byte[length];
            socket.Read(recData, 0, length);
            pointer = 1;
            return recData[0];
        }

        private static int int16()
        {
            return int8() + (int8() << 8);
        }

        public void StartDrawing(TCPSocket socket_)
        {
            processThread = new Thread(Processing);
            socket = socket_;
            running = true;
            processThread.Start();
        }

        private static void SendEndFrameEvent()
        {
            EventHandler<EventArgs> handler = EndFrameEvent;
            if(handler != null)
            {
                handler(null, new EventArgs());
            }
        }
    }
}
