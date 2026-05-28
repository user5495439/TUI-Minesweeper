using System.Runtime.InteropServices;
using System.Text;

// todo: color manipulation and get char and get color from buffer
// maybe todo 2: clean up some function names or provide descriptions

namespace MineSweeper
{
    class ConsoleBuffer
    {
        // libc write(2)
        [DllImport("libc", SetLastError = true)]
        static extern long write(int fd, byte[] buf, ulong count);

        private static int cursorX = 0;

        private static int cursorY = 0;

        private const string clearEscapeSequence = "\x1b[2J";    // clear the screen

        private const string defaultConsoleBuffer = "\x1b[1;1H"; // set cursor pos to 1, 1

        private static string consoleBuffer = defaultConsoleBuffer;

        private static string lastBuffer = consoleBuffer;

        public static void SetCursorPosition(int x, int y)
        {
            cursorX = x;
            cursorY = y;

            consoleBuffer += $"\x1b[{y};{x}H";
        }

        public static void SetCursorPosition((int x, int y) xy)
        {
            SetCursorPosition(xy.x, xy.y);
        }

        public static void Write(string text)
        {
            consoleBuffer += text;
        }

        public static void Write(char text)
        {
            consoleBuffer += text;
        }

        public static void Write(int number)
        {
            consoleBuffer += number.ToString();
        }

        public static void WriteLine(string text)
        {
            consoleBuffer += text + '\n';
        }

        public static void WriteLine(char text)
        {
            consoleBuffer += text + '\n';
        }

        public static void WriteLine(int number)
        {
            consoleBuffer += number.ToString() + '\n';
        }

        public static void Clear()
        {
            // Write the clear escape sequence
            byte[] bytes = Encoding.UTF8.GetBytes(clearEscapeSequence);

            // fd 1 = stdout
            write(1, bytes, (ulong)bytes.Length);
        }

        public static void BackgroundColor(ConsoleBufferColor color, bool OldANSI = false)
        {
            consoleBuffer += GetBackgroundColor(color, OldANSI);
        }

        public static string GetBackgroundColor(ConsoleBufferColor color, bool OldANSI = false)
        {
            if (OldANSI)
                return GetOldANSIColor(color, true);
            else
                return GetColor(color, true);
        }

        public static void ForegroundColor(ConsoleBufferColor color, bool OldANSI = false)
        {
            consoleBuffer += GetForegroundColor(color, OldANSI);
        }

        public static string GetForegroundColor(ConsoleBufferColor color, bool OldANSI = false)
        {
            if (OldANSI)
                return GetOldANSIColor(color, false);
            else
                return GetColor(color, false);
        }

        public static void ResetColor()
        {
            consoleBuffer += GetResetColor();
        }

        public static string GetResetColor()
        {
            return "\x1b[0m";
        }

        private static (int red, int green, int blue) HexToRGB(int hexInt)
        {
            string hexString = hexInt.ToString("x");

            string hexRed = hexString.Substring(0, 2);
            string hexGreen = hexString.Substring(2, 2);
            string hexBlue = hexString.Substring(4, 2);

            int red = Convert.ToInt32(hexRed, 16);
            int green = Convert.ToInt32(hexGreen, 16);
            int blue = Convert.ToInt32(hexBlue, 16);

            return (red, green, blue);
        }

        // Non-RGB color
        public static void Color(ConsoleBufferColor color, bool background = false)
        {
            consoleBuffer += GetColor(color, background);
        }

        public static string GetColor(ConsoleBufferColor color, bool background = false)
        {
            int colorInt = (int)color;

            int index = colorInt - ((colorInt >= 60) ? 50 : 0);

            (int red, int green, int blue) = HexToRGB(ConsoleBufferColorsBreeze.values[index]);

            return GetColorEscape(red, green, blue, background);
        }

        // Default color
        public static void Color(bool background = false, bool bright = false)
        {
            consoleBuffer += GetColor(background, bright);
        }

        public static string GetColor(bool background = false, bool bright = false)
        {
            int index = 8 + (background ? 1 : 0) + (bright ? 10 : 0);

            (int red, int green, int blue) = HexToRGB(ConsoleBufferColorsBreeze.values[index]);

            return GetColorEscape(red, green, blue, background);
        }

        // Non-RGB old ANSI color
        public static void OldANSIColor(ConsoleBufferColor color, bool background = false)
        {
            consoleBuffer += GetOldANSIColor(color, background); 
        }

        public static string GetOldANSIColor(ConsoleBufferColor color, bool background = false)
        {
            int index = (int)color + (background ? 40 : 30);

            // foreground values: 30-37, 90-97, background values: 40-47, 100-107
            return $"\x1b[{index}m"; 
        }

        // RGB color
        public static void SetColor(int red, int green, int blue, bool background = false)
        {
            consoleBuffer += GetColorEscape(red, green, blue, background);
        }

        public static string GetColorEscape(int red, int green, int blue, bool background = false)
        {
            byte groundByte = 38;   // foreground

            if (background)
                groundByte = 48;    // background

            return $"\x1b[{groundByte};2;{red};{green};{blue}m";
        }
        
        public static void SetColor(bool background, int red, int green, int blue)
        {
            SetColor(red, green, blue, background);
        }

        // to manipulate non old ansi colors in the buffer (like making them darker or brighter)
        public static void BufferManipulateColors()
        {
            // too lazy to write this right now
        }

        public static void BufferWrite(bool resetBuffer = true, bool clearScreen = true)
        {
            string buffer;

            if (clearScreen)
                buffer = clearEscapeSequence + consoleBuffer;
            else
                buffer = consoleBuffer;

            byte[] bytes = Encoding.UTF8.GetBytes(buffer);

            // fd 1 = stdout
            write(1, bytes, (ulong)bytes.Length);

            lastBuffer = consoleBuffer;

            if (resetBuffer)
                consoleBuffer = defaultConsoleBuffer;
        }

        public static char BufferGetCellChar(int x, int y)
        {
            return ' '; // too lazy to write this right now
        }

        public static (int red, int green, int blue) BufferGetCellColor(int x, int y)
        {
            return (0, 0, 0);   // too lazy to write this right now
        }

        public static ConsoleBufferColor RGBToBufferColor(int red, int green, int blue)
        {
            return ConsoleBufferColor.Black;    // too lazy to write this right now
        }

        public static (int red, int green, int blue) BufferColorToRGB(ConsoleBufferColor color)
        {
            int colorInt = (int)color;

            int index = colorInt - ((colorInt >= 60) ? 50 : 0);

            return HexToRGB(ConsoleBufferColorsBreeze.values[index]);
        }

        public static void BufferReset()
        {
            consoleBuffer = defaultConsoleBuffer;
        }

        public static bool LastBufferDifferent()
        {
            return consoleBuffer != lastBuffer;
        }
    }
}