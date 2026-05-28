namespace MineSweeper
{
    class ProgramDraw
    {
        private const string winMessage = "You win!";
        //private const string winMessage = "Voitit!";

        private const string lostMessage = "You lost!";
        //private const string lostMessage = "Hävisit!";

        private const string tryAgainMessage = "Right click to try again...";
        //private const string tryAgainMessage = "Oikea painallus yrittääkseen uudelleen...";

        private const string resizeMessage1_1 = "Resize the window to ";

        private const string resizeMessage1_2 = " to continue...";

        private const string resizeMessage2 = "Current window size: ";

        private const string resizeMessage3 = "To use the current window size, you need to reset the game (right click)";

        public static void Write(bool resizeScreen, int gameEnd, bool debugMode = false)
        {
            if (resizeScreen)
            {
                WriteResizeScreen();
            }
            else
            {
                WriteGame();

                if (WindowSizeOK(0, 2))
                    WriteUI(debugMode, gameEnd);
            }
        }

        public static void WriteGame()
        {
            int x = 0;
            int y = 0;

            if (!Program.windowSizeGame)
            {
                (x, y) = CalculateOffset();

                if (WindowSizeOK())
                    GameDraw.drawBorders(x - 2, y - 1);
            }

            GameDraw.drawTiles(x, y);
        }

        public static void WriteEndScreen(int gameEnd)
        {
            if (gameEnd == 1)
                TextScreen(winMessage, tryAgainMessage);
            else
                TextScreen(lostMessage, tryAgainMessage);
        }

        public static void WriteResizeScreen()
        {
            int width = Program.windowSizeGame ? Program.startConsoleWidth : Math.Max(Program.windowWidth, GameWidth() - 3);
            int height = Program.windowSizeGame ? Program.startConsoleHeight : Math.Max(Program.windowHeight, GameHeight() - 1);

            string message3 = Program.windowSizeGame ? resizeMessage3 : "";

            string line1 = resizeMessage1_1 + width + "x" + height + resizeMessage1_2;

            string line2 = resizeMessage2 + Program.windowWidth + "x" + Program.windowHeight;

            TextScreen(line1, line2, message3);
        }

        // i feel like this function needs a bit of cleaning
        public static void WriteUI(bool debugMode, int gameEnd)
        {
            (int x, int y) = CalculateOffset();

            string minesText = formatMineswNumber(Program.remainingMines, Program.mines);

            string clicksText = formatMineswNumber(GameInput.clicks, GameInput.clicks);

            int topTextY = y - 2;

            int minesTextX = x - 2;

            int clicksTextX = x + GameWidth() - 1 - clicksText.Length;

            ConsoleBuffer.SetCursorPosition(minesTextX, topTextY);

            ConsoleBuffer.Write(minesText);

            ConsoleBuffer.SetCursorPosition(clicksTextX, topTextY);

            ConsoleBuffer.Write(clicksText);

            if (gameEnd != 0)
            {
                string gameOverText = gameEnd == 1 ? winMessage : lostMessage;

                string seedText = $"Seed: {GameLogic.seed}";

                string gameInfoText = $"Game info: width x height = {GameLogic.width} x {GameLogic.height}, mines = {GameLogic.mines}";

                int bottomTextY = y + GameHeight();

                int gameOverTextX = CalculateCenterXText(x - 2, GameWidth() + 1, gameOverText.Length);

                int retryTextX = CalculateCenterXText(x - 2, GameWidth() + 1, tryAgainMessage.Length);

                (int x, int y) gameInfoTextXY = (1, Program.windowHeight);

                (int x, int y) seedTextXY = (gameInfoTextXY.x, gameInfoTextXY.y - 1);

                ConsoleBuffer.SetCursorPosition(gameOverTextX, topTextY);

                ConsoleBuffer.Write(gameOverText);

                ConsoleBuffer.SetCursorPosition(retryTextX, bottomTextY);

                ConsoleBuffer.Write(tryAgainMessage);

                ConsoleBuffer.SetCursorPosition(seedTextXY);

                ConsoleBuffer.Write(seedText);

                ConsoleBuffer.SetCursorPosition(gameInfoTextXY);

                ConsoleBuffer.Write(gameInfoText);
            }

            if (debugMode)
                WriteDebug();
        }

        private static void WriteDebug()
        {
            //ConsoleBuffer.SetCursorPosition(0, 1);

            //ConsoleBuffer.Write($"{ConsoleBuffer.LastBufferDifferent()}");

            //ConsoleBuffer.Write($"{clicks}, mines: {remainingMines}, seed: {seed}, mouseState: {mouseAndKeyboardInput.mouseState}, mouseButton: {mouseAndKeyboardInput.mouseButton}");

            //ConsoleBuffer.SetCursorPosition(0, 2);

            //(int width, int height) = GameLogic.getDimensions();

            //ConsoleBuffer.Write($"mines {GameLogic.mines}, tileCounter: {GameLogic.revealedTileCounter}, width: {width}, height: {height}, won: {width * height == GameLogic.revealedTileCounter + GameLogic.mines}");
        }

        public static void TextScreen(string message, string message2, string message3 = "")
        {
            int CenterX = Program.windowWidth / 2;

            int CenterY = Program.windowHeight / 2;

            //WriteScore();

            if (message3 != "")
            {
                ConsoleBuffer.SetCursorPosition(CenterX - message3.Length / 2, CenterY + 1);

                ConsoleBuffer.Write(message3);

                CenterY -= 1;
            }

            ConsoleBuffer.SetCursorPosition(CenterX - message.Length / 2, CenterY);

            ConsoleBuffer.Write(message);

            ConsoleBuffer.SetCursorPosition(CenterX - message2.Length / 2, CenterY + 1);

            ConsoleBuffer.Write(message2);
        }

        // after adding that fix to prevent that crash i feel like this function could be cleaned up
        public static string formatMineswNumber(int input, int max)
        {
            max = max.ToString().Length;

            max = max % 2 == 1 ? max + 1 : max;

            // this is here to prevent a crash if remaining mines go too far to negatives or whatever idk
            int inputL = input.ToString().Length;

            inputL = inputL % 2 == 1 ? inputL + 1 : inputL;

            string text = input.ToString();
            
            text = new string('0', Math.Max(max, inputL) - text.Length) + text;

            return text;
        }

        public static int CalculateCenterXText(int start, int end, int length)
        {
            return start + end / 2 - length / 2;
        }

        // calculate mouse position in game coords
        public static (int x, int y) CalculateMouseGameCoords()
        {
            (int ox, int oy) = Program.windowSizeGame ? (0, 0) : CalculateOffset();

            int x = (mouseAndKeyboardInput.mouseX - ox) / 2;    // x is divided by two because one monospace character is pretty much half as wide as it is long
            int y = mouseAndKeyboardInput.mouseY - oy;

            return (x, y);
        }

        // calculate the offset where the game is written
        public static (int x, int y) CalculateOffset()
        {
            int centerX = Program.windowWidth / 2;
            int centerY = Program.windowHeight / 2;

            centerX = centerX % 2 == 1 ? centerX : centerX - 1;

            int x = centerX - GameLogic.width + 2;    // gameWidth is not divided by two because one monospace character is pretty much half as wide as it is long
            int y = centerY - GameLogic.height / 2 + 1;

            return (x, y);
        }

        // if window size is ok, big enough to draw the game or in windowSizeGame unchanged size
        public static bool WindowSizeOK(int widthTolerance = 0, int heightTolerance = 0)
        {
            if (Program.windowSizeGame)
                return Program.windowWidth == Program.startConsoleWidth && Program.windowHeight == Program.startConsoleHeight;      // window size unchanged
            else
                return Program.windowWidth > GameWidth() + widthTolerance && Program.windowHeight > GameHeight() + heightTolerance; // big enough window
        }

        // made for windowSizeGame mode as off
        public static int GameWidth()
        {
            return GameLogic.width * 2 + 3;
        }

        // made for windowSizeGame mode as off
        public static int GameHeight()
        {
            return GameLogic.height + 1;
        }
    }
}