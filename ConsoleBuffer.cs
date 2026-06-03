using System.Runtime.InteropServices;
using System.Text;

// v2 or whatever. maybe to do: OldANSI colors

namespace MineSweeper
{
    class ConsoleBuffer
    {
        // libc write(2)
        [DllImport("libc", SetLastError = true)]
        static extern long write(int fd, byte[] buf, ulong count);

        const int leftOffset = 1;   // in linux terminals, cursor at 0 width is out of bounds
        const int topOffset = 1;    // in linux terminals, cursor at 0 height is out of bounds
        const int bottomOffset = -1;
        const int rightOffset = -1;

        const char emptyCellChar = ' ';     // what char to use for empty cells

        private static int bufferWidth;
        private static int bufferHeight;

        private static int cursorX = 0;
        private static int cursorY = 0;

        private static readonly RGB defaultFColor = RGB.Foreground;
        private static readonly RGB defaultBColor = RGB.Background;

        private static RGB cursorFColor = defaultFColor;
        private static RGB cursorBColor = defaultBColor;

        private static bool allDirty = false;

        public static bool clearBufferWhenWrite { get; set; } = false;

        private static readonly Cell emptyCell = new Cell
        {
            cellFColor = defaultFColor,
            cellBColor = defaultBColor,
            cellChar = emptyCellChar
        };

        public struct RGB
        {
            private const int brightColorOffset = 50;

            public byte r, g, b;

            public RGB(byte red, byte green, byte blue)  // i'm relatively new to structs and chatgpt thinks i should use a constructor here
            {
                r = red; g = green; b = blue;
            }

            // absolutely genius way of getting rgb from hex by chatgpt
            public static RGB FromInt(int value)
            => new(
                (byte)((value >> 16) & 0xFF),
                (byte)((value >> 8) & 0xFF),
                (byte)(value & 0xFF)
            );

            public static RGB Black => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Black]);
            public static RGB DarkRed => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkRed]);
            public static RGB DarkGreen => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkGreen]);
            public static RGB DarkYellow => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkYellow]);
            public static RGB DarkBlue => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkBlue]);
            public static RGB DarkMagenta => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkMagenta]);
            public static RGB DarkCyan => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkCyan]);
            public static RGB Gray => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Gray]);

            public static RGB DarkGray => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.DarkGray - brightColorOffset]);
            public static RGB Red => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Red - brightColorOffset]);
            public static RGB Green => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Green - brightColorOffset]);
            public static RGB Yellow => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Yellow - brightColorOffset]);
            public static RGB Blue => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Blue - brightColorOffset]);
            public static RGB Magenta => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Magenta - brightColorOffset]);
            public static RGB Cyan => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Cyan - brightColorOffset]);
            public static RGB White => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.White - brightColorOffset]);

            public static RGB Foreground => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Foreground]);
            public static RGB Background => FromInt(ConsoleBufferColorsBreeze.values[(int)ConsoleBufferColor.Background]);

            // i used a bit of chatgpt here because this is completely new territory for me so i didn't know how to implement this before
            public static bool operator ==(RGB left, RGB right) => 
                left.r == right.r &&
                left.g == right.g &&
                left.b == right.b;

            public static bool operator !=(RGB left, RGB right) => 
                !(left == right);

            public override bool Equals(object obj) =>
                obj is RGB other && this == other;

            public override int GetHashCode() =>
                HashCode.Combine(r, g, b);
        }

        private struct Cell
        {
            public RGB cellFColor;

            public RGB cellBColor;

            public char cellChar;

            public bool cellDirty;

            public static bool operator ==(Cell left, Cell right) => 
                left.cellFColor == right.cellFColor &&
                left.cellBColor == right.cellBColor &&
                left.cellChar == right.cellChar;

            public static bool operator !=(Cell left, Cell right) => 
                !(left == right);

            public override bool Equals(object obj) =>
                obj is Cell other && this == other;

            public override int GetHashCode() =>
                HashCode.Combine(cellFColor, cellBColor, cellChar);
        }

        private static Cell[,] buffer;

        private static Cell[,] lastBuffer;

        public static void SetCursorPosition(int x, int y)
        {
            // the cursor variables store array positions
            cursorX = x - leftOffset;
            cursorY = y - topOffset;

            CursorBoundsCheck();
        }

        public static void SetCursorPosition((int x, int y) xy)
        {
            SetCursorPosition(xy.x, xy.y);
        }

        public static (int x, int y) GetCursorPosition()
        {
            return (cursorX + leftOffset, cursorY + topOffset);
        }

        public static void Write(char text)
        {
            CursorBoundsCheck();

            ref Cell cell = ref buffer[cursorX, cursorY];

            cursorX++;

            if (cell.cellChar == text && cell.cellFColor == cursorFColor && cell.cellBColor == cursorBColor)
                return;

            cell.cellChar = text;

            cell.cellBColor = cursorBColor;
            cell.cellFColor = cursorFColor;

            if (!clearBufferWhenWrite) cell.cellDirty = true;
        }

        public static void Write(string text)
        {
            int length = text.Length;

            for (int i = 0; i < length; i++)
            {
                Write(text[i]);
            }
        }

        public static void Write(StringBuilder text)
        {
            int length = text.Length;

            for (int i = 0; i < length; i++)
            {
                Write(text[i]);
            }
        }

        public static void SetColor(RGB rgb, bool background = false)
        {
            if (background)
                cursorBColor = rgb;
            else
                cursorFColor = rgb;
        }

        public static void SetColor(RGB fColor, RGB bColor)
        {
            cursorBColor = bColor;
            cursorFColor = fColor;
        }

        private static void InitializeBuffer(ref Cell[,] buffer, bool updateSizes = true, bool resize = false)
        {
            if (updateSizes)
            {
                bufferWidth = Console.WindowWidth - rightOffset - leftOffset;
                bufferHeight = Console.WindowHeight - bottomOffset - topOffset;
            }

            if (!resize)
            {
                buffer = new Cell[bufferWidth, bufferHeight];
                return;
            }

            Cell[,] bufferCopy = buffer;

            buffer = new Cell[bufferWidth, bufferHeight];

            int MaxX = Math.Min(bufferCopy.GetLength(0), bufferWidth);
            int MaxY = Math.Min(bufferCopy.GetLength(1), bufferHeight);
            
            for (int y = 0; y < MaxY; y++)
            {
                for (int x = 0; x < MaxX; x++)
                {
                    buffer[x, y] = bufferCopy[x, y];
                }
            }
        }

        public static void BufferInitialize()
        {
            InitializeBuffer(ref buffer, updateSizes: true, resize: false);

            if (clearBufferWhenWrite) InitializeBuffer(ref lastBuffer, updateSizes: true, resize: false);
        }

        public static void BufferResize()
        {
            InitializeBuffer(ref buffer, updateSizes: true, resize: true);

            if (clearBufferWhenWrite) InitializeBuffer(ref lastBuffer, updateSizes: true, resize: true);
        }

        public static void BufferMarkAllDirty()
        {
            allDirty = true;
        }

        public static void BufferWrite()
        {
            if (clearBufferWhenWrite && !allDirty)
                FindDirty(ref buffer, ref lastBuffer);

            const string resetColorCode = "\x1b[0m";

            StringBuilder writeBuffer = new();

            for (int y = 0; y < bufferHeight; y++)
            {
                RGB? previousFColor = null;
                RGB? previousBColor = null;

                int dirtyLength = 0;

                for (int x = 0; x < bufferWidth; x++)
                {
                    ref Cell cell = ref buffer[x, y];

                    if (cell.cellDirty == false && !allDirty)
                    {
                        dirtyLength = 0;
                        continue;
                    }

                    char cellChar = cell.cellChar;
                    RGB cellFColor = cell.cellFColor;
                    RGB cellBColor = cell.cellBColor;

                    if (cellChar == '\0')
                    {
                        cellChar = emptyCell.cellChar;
                        cellFColor = emptyCell.cellFColor;
                        cellBColor = emptyCell.cellBColor;
                    }

                    if (dirtyLength == 0)
                        writeBuffer.Append(GetCursorPosEscape(x + leftOffset, y + topOffset));

                    dirtyLength++;

                    if (previousFColor == null || previousFColor != cellFColor)
                        writeBuffer.Append(GetColorEscape(cellFColor, false));

                    if (previousBColor == null || previousBColor != cellBColor)
                        writeBuffer.Append(GetColorEscape(cellBColor, true));

                    previousFColor = cellFColor;
                    previousBColor = cellBColor;

                    writeBuffer.Append(cellChar);

                    cell.cellDirty = false;
                }

                writeBuffer.Append(resetColorCode);
            }

            byte[] bytes = Encoding.UTF8.GetBytes(writeBuffer.ToString());
            // fd 1 = stdout
            write(1, bytes, (ulong)bytes.Length);

            if (clearBufferWhenWrite)
            {
                lastBuffer = buffer;
                InitializeBuffer(ref buffer, updateSizes: false);
            }

            allDirty = false;
        }

        private static void FindDirty(ref Cell[,] Buffer, ref Cell[,] LastBuffer)
        {
            for (int y = 0; y < bufferHeight; y++)
            {
                for (int x = 0; x < bufferWidth; x++)
                {
                    if (Buffer[x, y] != LastBuffer[x, y])
                    {
                        Buffer[x, y].cellDirty = true;
                    }

                    else
                    {
                        Buffer[x, y].cellDirty = false;
                    }
                }
            }
        }

        private static string GetCursorPosEscape(int x, int y)
        {
            return $"\x1b[{y};{x}H";
        }

        private static string GetColorEscape(RGB rgb, bool background = false)
        {
            byte groundByte = 38;   // foreground

            if (background)
                groundByte = 48;    // background

            return $"\x1b[{groundByte};2;{rgb.r};{rgb.g};{rgb.b}m";
        }

        public static void BackgroundColor(ConsoleBufferColor color)
        {
            SetPaletteColor(color, true);
        }

        public static void ForegroundColor(ConsoleBufferColor color)
        {
            SetPaletteColor(color, false);
        }

        public static RGB GetRGBfromPalette(ConsoleBufferColor color)
        {
            int index = (int)color - (((int)color >= 60) ? 50 : 0);

            return RGB.FromInt(ConsoleBufferColorsBreeze.values[index]);
        }

        public static void SetPaletteColor(ConsoleBufferColor color, bool background)
        {
            SetColor(GetRGBfromPalette(color), background);
        }

        public static void ResetColor()
        {
            cursorFColor = defaultFColor;
            cursorBColor = defaultBColor;
        }

        public static char GetCellChar(int x, int y)
        {
            return buffer[x - leftOffset, y - topOffset].cellChar;
        }

        public static RGB GetCellFColor(int x, int y)
        {
            return buffer[x - leftOffset, y - topOffset].cellFColor;
        }

        public static RGB GetCellBColor(int x, int y)
        {
            return buffer[x - leftOffset, y - topOffset].cellBColor;
        }


        private static void CursorBoundsCheck()
        {
            cursorX = Math.Max(cursorX, 0);
            cursorY = Math.Max(cursorY, 0);

            cursorX = Math.Min(cursorX, bufferWidth - 1);
            cursorY = Math.Min(cursorY, bufferHeight - 1);
        }
    }
}