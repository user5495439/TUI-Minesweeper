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

        //private static int cursorX = 0;

        //private static int cursorY = 0;

        private const string clearEscapeSequence = "\x1b[2J";    // clear the screen

        private const string defaultConsoleBuffer = "\x1b[1;1H"; // set cursor pos to 1, 1

        private static StringBuilder consoleBuffer = new(defaultConsoleBuffer);

        private static StringBuilder lastBuffer = new(defaultConsoleBuffer);

        private static StringBuilder tmpBuffer = new();

        public static void SetCursorPosition(int x, int y)
        {
            //cursorX = x;
            //cursorY = y;

            consoleBuffer.Append($"\x1b[{y};{x}H");
        }

        public static void SetCursorPosition((int x, int y) xy)
        {
            SetCursorPosition(xy.x, xy.y);
        }

        public static void Write(string text)
        {
            consoleBuffer.Append(text);
        }

        public static void Write(char text)
        {
            consoleBuffer.Append(text);
        }

        public static void Write(StringBuilder text)
        {
            consoleBuffer.Append(text);
        }

        public static void Write(int number)
        {
            consoleBuffer.Append(number);
        }

        public static void WriteLine(string text)
        {
            consoleBuffer.Append(text);
            consoleBuffer.Append('\n');
        }

        public static void WriteLine(char text)
        {
            consoleBuffer.Append(text);
            consoleBuffer.Append('\n');
        }

        public static void WriteLine(int number)
        {
            consoleBuffer.Append(number);
            consoleBuffer.Append('\n');
        }

        public static void Clear()
        {
            consoleBuffer.Append(clearEscapeSequence);
        }

        public static void ClearNow()
        {
            // Write the clear escape sequence
            byte[] bytes = Encoding.UTF8.GetBytes(clearEscapeSequence);

            // fd 1 = stdout
            write(1, bytes, (ulong)bytes.Length);
        }

        public static void BackgroundColor(ConsoleBufferColor color, bool OldANSI = false)
        {
            consoleBuffer.Append(GetBackgroundColor(color, OldANSI));
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
            consoleBuffer.Append(GetForegroundColor(color, OldANSI));
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
            consoleBuffer.Append(GetResetColor());
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
            consoleBuffer.Append(GetColor(color, background));
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
            consoleBuffer.Append(GetColor(background, bright));
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
            consoleBuffer.Append(GetOldANSIColor(color, background)); 
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
            consoleBuffer.Append(GetColorEscape(red, green, blue, background));
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
            if (clearScreen)
                tmpBuffer.Append(clearEscapeSequence);

            tmpBuffer.Append(consoleBuffer);

            byte[] bytes = Encoding.UTF8.GetBytes(tmpBuffer.ToString());

            // fd 1 = stdout
            write(1, bytes, (ulong)bytes.Length);

            lastBuffer.Clear().Append(consoleBuffer);       // apparently this works, so this is Clear() then Append()

            tmpBuffer.Clear();

            if (resetBuffer)
                BufferReset();
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
            consoleBuffer.Clear().Append(defaultConsoleBuffer);
        }

        public static bool LastBufferDifferent()
        {
            if (consoleBuffer.Length != lastBuffer.Length)
                return true;

            for (int i = 0; i < consoleBuffer.Length; i++)
            {
                if (consoleBuffer[i] != lastBuffer[i])
                    return true;
            }

            return false;
        }
    }
}