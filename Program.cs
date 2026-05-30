namespace MineSweeper
{
    class Program
    {
        public const bool cheats = true;

        public const bool windowSizeGame = false;

        public const bool tileNumbersStartWithZero = true;      // tiles are numbered as "05" instead of "55"

        public const bool canMoveMap = true;

        public const bool debugMode = false;

        private static int gameWidth = 32;

        private static int gameHeight = 16;

        public static int mines { get; private set; } = 99;

        private static bool useMineDensity = true;

        private static int mineDensity = 20;    // in percents, 0 - 100

        public static int windowWidth { get; private set; } = Console.WindowWidth;

        public static int windowHeight { get; private set; } = Console.WindowHeight;

        public static int startConsoleWidth { get; set; } = windowWidth;

        public static int startConsoleHeight { get; set; } = windowHeight;

        public static int remainingMines { get; set; }

        private static int gameEnd;

        private static int seed;

        private static Random rng = new Random();

        private static void Main(string[] args)
        {
            GameInput.InitInput();

            Task.Run(ResizeWatcher);

            windowWidth = Console.WindowWidth;
            windowHeight = Console.WindowHeight;

            initProgram();

            Update();

            ProgramLoop();
        }

        public static void initProgram()
        {
            GameInput.ResetClicks();

            gameEnd = 0;

            seed = rng.Next();

            if (windowSizeGame)
            {
                gameWidth = windowWidth / 2;
                gameHeight = windowHeight;
            }

            mines = useMineDensity ? (int)((gameWidth * gameHeight) * ((float)mineDensity / 100.0)) : mines;

            remainingMines = mines;

            GameLogic.initGame(gameWidth, gameHeight, mines, seed, true);
        }

        private static void ProgramLoop()
        {
            while (true)
            {
                mouseAndKeyboardInput.pollUserInput();

                Update();
            }
        }

        private static void Update()
        {
            windowWidth = Console.WindowWidth;
            windowHeight = Console.WindowHeight;

            bool windowOK = canMoveMap ? true : ProgramDraw.WindowSizeOK(-4, -2);     // if false, the screen that asks the user to resize the window should appear

            GameInput.Input(!windowOK, gameEnd, debugMode);

            ProgramDraw.Write(!windowOK, gameEnd, debugMode);

            if (ConsoleBuffer.LastBufferDifferent())
                ConsoleBuffer.BufferWrite();
            else
                ConsoleBuffer.BufferReset();
        }

        // 2 = lost, 1 = won
        public static void EndGame(int result)
        {
            if (result == 1)
                GameLogic.placeFlagsOnMines();
            else
                GameLogic.revealMines();

            gameEnd = result;

            //WriteEndScreen();
        }

        private static void ResizeWatcher()
        {
            int previousWidth;
            int previousHeight;

            while (true)
            {
                int width = Console.WindowWidth;
                int height = Console.WindowHeight;

                previousWidth = width;
                previousHeight = height;

                Thread.Sleep(100);

                width = Console.WindowWidth;
                height = Console.WindowHeight;

                if (previousWidth != width || previousHeight != height)
                {
                    Update();
                }
            }
        }
    }
}
