using System.ComponentModel;
using System.Reflection;
using System.Runtime.InteropServices;

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

        public static string debugText { get; set; } = "";

        private static int offsetX = 0;

        private static int offsetY = 0;

        public static (int top, int bottom, int left, int right) bCuts { get; private set; } = (0, 0, 0, 0);

        public static void InitDraw()
        {
            GameDraw.initDraw();
        }

        public static void Write(bool resizeScreen, int gameEnd, bool debugMode = false)
        {
            if (resizeScreen)
            {
                WriteResizeScreen();
            }
            else
            {
                WriteGame();

                if (Program.canMoveMap || WindowSizeOK(0, 2))
                    WriteUI(debugMode, gameEnd);
            }
        }

        public static void WriteGame()
        {
            int width = GameLogic.width;
            int height = GameLogic.height;

            int x = 0;
            int y = 0;

            if (!Program.windowSizeGame)
            {
                (x, y) = CalculateOffset();

                int bX = x - 2;
                int bY = y - 1;
                int bWidth = width + 2;
                int bHeight = height + 2;

                bCuts = CalculateCuts(bWidth, bHeight, bX, bY);

                GameDraw.drawBorders(x - 2, y - 1, bCuts);
            }

            GameDraw.drawTiles(x, y, CalculateCuts(width, height, x, y));
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

            int clicksTextX = x + GameWidth() - 1;

            WriteText(minesText, minesTextX, topTextY, false, false);

            WriteText(clicksText, clicksTextX, topTextY, true, false);

            if (gameEnd != 0)
            {
                string gameOverText = gameEnd == 1 ? winMessage : lostMessage;

                string seedText = $"Seed: {GameLogic.seed}";

                string gameInfoText = $"Game info: width x height = {GameLogic.width} x {GameLogic.height}, mines = {GameLogic.mines}";

                int bottomTextY = y + GameHeight();

                (int x, int y) gameInfoTextXY = (1, Program.windowHeight);

                (int x, int y) seedTextXY = (gameInfoTextXY.x, gameInfoTextXY.y - 1);

                WriteText(gameOverText, x + (GameWidth() - 1) / 2, topTextY, false, true);

                WriteText(tryAgainMessage, x + (GameWidth() - 1) / 2, bottomTextY, false, true);

                WriteText(seedText, seedTextXY, false, false);

                WriteText(gameInfoText, gameInfoTextXY, false, false);

                ConsoleBuffer.SetCursorPosition(gameInfoTextXY);

                ConsoleBuffer.Write(gameInfoText);
            }

            if (debugMode)
                WriteDebugUI();
        }

        // returns true if visible, false if hidden
        private static bool WriteText(string text, int x, int y, bool rtlText = false, bool centerText = false)
        {
            int windowWidth = Program.windowWidth;
            int windowHeight = Program.windowHeight;

            if (y < 1 || y > windowHeight)
                return false;

            int leftCut = 0;
            int rightCut = 0;

            int length = text.Length;
            int endX = x + length;

            // calculate x
            if (rtlText)
                x -= length;
            else if (centerText)
                x -= length / 2;

            // if string start or string end is out of bounds, add values to leftCut and rightCut appropriately
            if (x < 1)
            {
                leftCut = -x + 1;
            }
            if (endX > windowWidth)
            {
                rightCut = endX - windowWidth - 1;
            }

            // if the cut variables are longer than string length the string must be hidden
            if (leftCut > length || rightCut > length)
                return false;

            // make sure x is in bounds
            x = Math.Max(x, 1);

            text = text[leftCut..^rightCut];    // cut the beginning and end of the string

            ConsoleBuffer.SetCursorPosition(x, y);
            ConsoleBuffer.Write(text);

            return true;
        }

        private static bool WriteText(string text, (int x, int y) xy, bool rtlText = false, bool centerText = false)
        {
            return WriteText(text, xy.x, xy.y, rtlText, centerText);
        }

        private static void WriteDebugUI()
        {
            ConsoleBuffer.SetCursorPosition(1, 1);

            (int x, int y) = CalculateMouseGameCoords();

            ConsoleBuffer.Write(debugText);
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

        // calculates necessary cuts which are made to the board or the borders
        public static (int top, int bottom, int left, int right) CalculateCuts(int width, int height, int x, int y)
        {
            int windowWidth = Program.windowWidth & ~1;
            int windowHeight = Program.windowHeight;

            int top = y < 1 ? (-y + 1) : 0;
            int bottom = y + height > windowHeight + 1 ? y + height - windowHeight - 1 : 0;
            int left = x < 1 ? (-x + 1) / 2 : 0;
            int right = x + width * 2 > windowWidth + 2 ? (x + width * 2 - windowWidth - 1) / 2 : 0;

            return (
                top,
                bottom,
                left,
                right
            );
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
            int centerX = Program.windowWidth / 2 + GameInput.offsetX;
            int centerY = Program.windowHeight / 2 + GameInput.offsetY;

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

        public static int GameWidth()
        {
            return GameLogic.width * 2 + 3;
        }

        public static int GameHeight()
        {
            return GameLogic.height + 1;
        }
    }
}