namespace MineSweeper
{
    class GameInput
    {
        private static int wasMouseX = 0;

        private static int wasMouseY = 0;

        public static int mouseMovedX { get; private set; } = 0;

        public static int mouseMovedY { get; private set; } = 0;

        private static int _offsetX = 0;

        public static int offsetX { get; private set; } = 0;

        public static int offsetY { get; private set; } = 0;

        public static int clicks { get; private set; } = 0;

        private static char kbInput;

        private static bool gotKbInput = false;

        public static void InitInput()
        {
            mouseAndKeyboardInput.enableExitHandling();
            mouseAndKeyboardInput.enableRawMode();
            mouseAndKeyboardInput.enableMouseEventTracking();
            mouseAndKeyboardInput.enableAltScreenBuffer();
        }

        public static void Input(bool resizeScreen, int gameEnd, bool debugMode = false)
        {
            string kbInputString = mouseAndKeyboardInput.KbInput ?? "";   // mouseAndKeyboardInput.KbInput can be null (at the start of the program at least) because i coded stuff in a bit of a weird way i guess

            if (string.IsNullOrEmpty(kbInputString))
                gotKbInput = false;
            else
                gotKbInput = true;

            // only the last character from keyboard input will be handled, this behavior might cause issues but everything is way easier to code this way
            foreach (char c in kbInputString)
            {
                kbInput = c;
            }

            if (resizeScreen)
            {
                ResizeInput();
            }
            else
            {
                if (!Program.windowSizeGame)
                    MoveMapInput();

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

        public static void MoveMapInput()
        {
            int mouseState = mouseAndKeyboardInput.mouseState;

            int mouseX = mouseAndKeyboardInput.mouseX;

            int mouseY = mouseAndKeyboardInput.mouseY;

            if (gotKbInput && kbInput == 'f')
            {
                _offsetX = 0;
                offsetY = 0;
            }

            if (Program.canMoveMap)
            {
                if (mouseState == 1)
                {
                    mouseMovedX = 0;
                    mouseMovedY = 0;
                }
                else
                if (mouseState == 33)
                {
                    mouseMovedX = mouseX - wasMouseX;
                    mouseMovedY = mouseY - wasMouseY;

                    _offsetX += mouseMovedX;
                    offsetY += mouseMovedY;
                }

                wasMouseX = mouseX;
                wasMouseY = mouseY;
            }

            offsetX = _offsetX & ~1; 
        }

        public static void InGameInput()
        {
            (int x, int y) = ProgramDraw.CalculateMouseGameCoords();

            if (gotKbInput && kbInput == 'c' && Program.cheats)
            {
                GameSolverCheat.Cheat();
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
                    Program.InitGame();
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

                    Program.InitGame();
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