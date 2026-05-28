namespace MineSweeper
{
    class GameInput
    {
        public static int clicks { get; private set; } = 0;

        public static void InitInput()
        {
            mouseAndKeyboardInput.enableRawMode();
            mouseAndKeyboardInput.enableMouseEventTracking();
            mouseAndKeyboardInput.enableAltScreenBuffer();
        }

        public static void Input(bool resizeScreen, int gameEnd, bool debugMode = false)
        {
            if (resizeScreen)
            {
                ResizeInput();
            }
            else
            {
                if (gameEnd != 0)
                {
                    GameOverInput();
                }
                else 
                {
                    InGameInput();
                }
            }
        }

        public static void InGameInput()
        {
            (int x, int y) = ProgramDraw.CalculateMouseGameCoords();

            string kbInput = mouseAndKeyboardInput.KbInput ?? "";   // mouseAndKeyboardInput.KbInput can be null (at the start of the program at least) because i coded stuff in a bit of a weird way i guess

            foreach (char c in kbInput)
            {
                if (c == 'c' && Program.cheats)
                {
                    GameSolverCheat.Cheat();
                }
            }

            if (!GameLogic.inBounds(x, y))
                return;

            switch (Click())
            {
                case 1:
                    LeftClick(x, y);
                    break;
                case 2:
                    RightClick(x, y);
                    break;
            }
        }

        public static void GameOverInput()
        {
            switch (Click())
            {
                case 2:
                    Program.initProgram();
                    break;
            }
        }

        public static void ResizeInput()
        {
            switch (Click())
            {
                case 2:
                    Program.startConsoleWidth = Program.windowWidth;
                    Program.startConsoleHeight = Program.windowHeight;

                    Program.initProgram();
                    break;
            }
        }

        public static void LeftClick(int x, int y)
        {
            switch (GameLogic.revealTile(x, y))
            {
                case -1:
                    Program.EndGame(2);
                    break;
                case 0:
                    break;
                case 1:
                    clicks++;
                    break;
                case 2:
                    Program.EndGame(1);
                    break;
            }
        }

        public static void RightClick(int x, int y)
        {
            Program.remainingMines -= GameLogic.placeFlag(x, y);
        }

        public static int Click()
        {
            char button = mouseAndKeyboardInput.mouseButton;
            int state = mouseAndKeyboardInput.mouseState;

            if (button == 'm')
            {
                switch (state)
                {
                    case 0:     // left click
                    case 32:    // moving
                        return 1;
                    case 1:     // middle click
                    case 33:    // moving
                        return 3;
                    case 2:     // right click
                    case 34:    // moving
                        return 2;
                }
            }

            return 0;
        }

        public static void ResetClicks()
        {
            clicks = 0;
        }
    }
}