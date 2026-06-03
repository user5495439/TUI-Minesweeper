using System.Text;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;


/*
Usage:

 enable raw mode and mouse tracking:

 mouseAndKeyboardInput.enableRawMode();
 mouseAndKeyboardInput.enableMouseEventTracking();


 make a loop (usually a while loop) and poll user input in it constantly:

 mouseAndKeyboardInput.pollUserInput();




 example: read keyboard input and output it to console:

class example
{
    mouseAndKeyboardInput.enableRawMode();
    mouseAndKeyboardInput.enableMouseEventTracking();

    string exampleString = "";

    while (true)
    {
        mouseAndKeyboardInput.pollUserInput();

        foreach (char c in mouseAndKeyboardInput.input)
        {
            exampleString += c;
        }

        Console.SetCursorPosition(0, 0);

        Console.Write(exampleString);
    }
}



example: read mouse position in the terminal emulator and output it:

class example
{
    mouseAndKeyboardInput.enableRawMode();
    mouseAndKeyboardInput.enableMouseEventTracking();

    while (true)
    {
        mouseAndKeyboardInput.pollUserInput();

        Console.SetCursorPosition(0, 0);

        Console.Write($"X: {mouseAndKeyboardInput.mouseX}, Y: {mouseAndKeyboardInput.mouseY}    ");
    }
}

*/

namespace MineSweeper
{
    class mouseAndKeyboardInput
    {
        const int STDIN_FILENO = 0;
        const int TCSANOW = 0;

        // termios flag constants (all uint)
        const uint ICANON = 0x0002u;
        const uint ECHO = 0x0008u;
        const uint ISIG = 0x0001u;

        // indices into c_cc array
        const int VMIN = 6;
        const int VTIME = 5;

        [DllImport("libc")]
        static extern int tcgetattr(int fd, out Termios termios_p);

        [DllImport("libc")]
        static extern int tcsetattr(int fd, int optional_actions, ref Termios termios_p);

        [DllImport("libc", SetLastError = true)]
        static extern int read(int fd, byte[] buf, int count);

        [StructLayout(LayoutKind.Sequential)]
        struct Termios
        {
            public uint c_iflag;
            public uint c_oflag;
            public uint c_cflag;
            public uint c_lflag;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
            public byte[] c_cc;
            public uint c_ispeed;
            public uint c_ospeed;
        }

        private static Termios orig;

        protected static bool altScreenBuffer { get; set; } = false;

        public static void enableAltScreenBuffer()
        {
            altScreenBuffer = true;

            Console.Write("\x1b[?1049h");
        }

        public static void enableRawMode()
        {
            // Save original terminal
            tcgetattr(STDIN_FILENO, out orig);
            var raw = orig;

            // Disable canonical mode, echo, signals
            raw.c_lflag &= ~(ICANON | ECHO | ISIG);
            if (raw.c_cc == null) raw.c_cc = new byte[32];
            raw.c_cc[VMIN] = 1;  // return as soon as 1 byte is available
            raw.c_cc[VTIME] = 0; // no timeout

            tcsetattr(STDIN_FILENO, TCSANOW, ref raw);

            Console.Write("\x1b[?25l"); // disable cursor
        }

        public static void disableRawMode()
        {
            tcsetattr(STDIN_FILENO, TCSANOW, ref orig);

            Console.Write("\x1b[?25h"); // enable cursor
        }

        public static void enableMouseEventTracking()
        {
            // Enable any-event mouse tracking + SGR mode
            Console.Write("\x1b[?1003h"); // any-event mouse
            Console.Write("\x1b[?1006h"); // SGR mode
        }

        public static void disableMouseEventTracking()
        {
            Console.Write("\x1b[?1003l");
            Console.Write("\x1b[?1006l");
        }

        public static void enableExitHandling()
        {
            AppDomain.CurrentDomain.ProcessExit += (_, _) => cleanUp();

            AppDomain.CurrentDomain.UnhandledException += (_, e) =>
            {
                cleanUp();
            };

            TaskScheduler.UnobservedTaskException += (_, e) =>
            {
                cleanUp();
                e.SetObserved();
            };
        }

        private static byte[] buf = new byte[1024];

        public static bool CtrlCExit { get; set; } = true;

        public static int mouseX { get; private set; }

        public static int mouseY { get; private set; }

        public static int mouseState { get; private set; }

        public static char mouseButton { get; private set; }

        public static string Input { get; private set; }    // regular unfiltered input, including mouse and keyboard input

        public static string KbInput { get; private set; }  // filtered keyboard only input

        public static void pollUserInput()
        {
            int n = read(STDIN_FILENO, buf, buf.Length);
            if (n <= 0) safeExit(1);

            Input = Encoding.UTF8.GetString(buf, 0, n);

            foreach (Match m in Regex.Matches(Input, @"\x1B\[<(\d+);(\d+);(\d+)([Mm])"))
            {
                mouseState = int.Parse(m.Groups[1].Value);
                mouseX = int.Parse(m.Groups[2].Value);
                mouseY = int.Parse(m.Groups[3].Value);
                mouseButton = m.Groups[4].Value[0];
            }

            KbInput = "";

            foreach (char c in Input)
            {
                if (c == 27) // 27 = mouse input escape character
                {
                    Input = "";
                    break;
                }

                if (c == '\x03' && CtrlCExit) // Ctrl + C
                    safeExit(0);

                KbInput += c;
            }
        }

        public static void safeExit(int exitCode)
        {
            cleanUp();

            Environment.Exit(exitCode);
        }

        public static void cleanUp()
        {
            if (altScreenBuffer)
                Console.Write("\x1b[?1049l");

            disableMouseEventTracking();
            disableRawMode();
        }
    }
}