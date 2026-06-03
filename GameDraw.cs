using System.Text;

namespace MineSweeper
{
    class GameDraw
    {
        private static (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor)[] tileAppearance =
        {
            (icon: "[]", fColor: ConsoleBufferColor.Gray,       bColor: ConsoleBufferColor.DarkGray),   // hidden
            (icon: "  ", fColor: ConsoleBufferColor.Black,      bColor: ConsoleBufferColor.Black),      // revealed
            (icon: "[]", fColor: ConsoleBufferColor.Red,        bColor: ConsoleBufferColor.DarkGray),   // flag
            (icon: "??", fColor: ConsoleBufferColor.Black,      bColor: ConsoleBufferColor.DarkGray),   // question mark
            (icon: "[]", fColor: ConsoleBufferColor.DarkGray,   bColor: ConsoleBufferColor.Red),        // misplaced flag
            (icon: "<>", fColor: ConsoleBufferColor.DarkGray,   bColor: ConsoleBufferColor.DarkRed)     // exploded mine
        };
        private static (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor)[] boardAppearance =
        {
            (icon: "  ", fColor: ConsoleBufferColor.Black,      bColor: ConsoleBufferColor.Black),  // 0
            (icon: "01", fColor: ConsoleBufferColor.Blue,       bColor: ConsoleBufferColor.Black),  // 1
            (icon: "02", fColor: ConsoleBufferColor.Green,      bColor: ConsoleBufferColor.Black),  // 2
            (icon: "03", fColor: ConsoleBufferColor.Red,        bColor: ConsoleBufferColor.Black),  // 3
            (icon: "04", fColor: ConsoleBufferColor.Magenta,    bColor: ConsoleBufferColor.Black),  // 4
            (icon: "05", fColor: ConsoleBufferColor.DarkYellow, bColor: ConsoleBufferColor.Black),  // 5
            (icon: "06", fColor: ConsoleBufferColor.Cyan,       bColor: ConsoleBufferColor.Black),  // 6
            (icon: "07", fColor: ConsoleBufferColor.DarkGray,   bColor: ConsoleBufferColor.Black),  // 7
            (icon: "08", fColor: ConsoleBufferColor.DarkRed,    bColor: ConsoleBufferColor.Black),  // 8
            (icon: "<>", fColor: ConsoleBufferColor.DarkGray,   bColor: ConsoleBufferColor.Black)   // 9, mine  alt: 𜷂𜷖
        };
        private static (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor)[] boardAppearanceAlt =
        {
            (icon: "  ", fColor: ConsoleBufferColor.Black,      bColor: ConsoleBufferColor.Black),  // 0
            (icon: "11", fColor: ConsoleBufferColor.Blue,       bColor: ConsoleBufferColor.Black),  // 1
            (icon: "22", fColor: ConsoleBufferColor.Green,      bColor: ConsoleBufferColor.Black),  // 2
            (icon: "33", fColor: ConsoleBufferColor.Red,        bColor: ConsoleBufferColor.Black),  // 3
            (icon: "44", fColor: ConsoleBufferColor.Magenta,    bColor: ConsoleBufferColor.Black),  // 4
            (icon: "55", fColor: ConsoleBufferColor.DarkYellow, bColor: ConsoleBufferColor.Black),  // 5
            (icon: "66", fColor: ConsoleBufferColor.Cyan,       bColor: ConsoleBufferColor.Black),  // 6
            (icon: "77", fColor: ConsoleBufferColor.DarkGray,   bColor: ConsoleBufferColor.Black),  // 7
            (icon: "88", fColor: ConsoleBufferColor.DarkRed,    bColor: ConsoleBufferColor.Black),  // 8
            (icon: "<>", fColor: ConsoleBufferColor.DarkGray,   bColor: ConsoleBufferColor.Black)   // 9, mine  alt: 𜷂𜷖
        };

        // these will store the color codes as RGB for the above arrays for performance optimization reasons and also the icons copied
        private static (string icon, ConsoleBuffer.RGB fColor, ConsoleBuffer.RGB bColor)[] tileAppearanceCached = new (string icon, ConsoleBuffer.RGB, ConsoleBuffer.RGB)[tileAppearance.Length];
        private static (string icon, ConsoleBuffer.RGB fColor, ConsoleBuffer.RGB bColor)[] boardAppearanceCached = new (string icon, ConsoleBuffer.RGB, ConsoleBuffer.RGB)[boardAppearance.Length];

        public static void initDraw()
        {
            for (int i = 0; i < tileAppearance.Length; i++)
            {
                tileAppearanceCached[i].icon = tileAppearance[i].icon[..2];
                tileAppearanceCached[i].fColor = ConsoleBuffer.GetRGBfromPalette(tileAppearance[i].fColor);
                tileAppearanceCached[i].bColor = ConsoleBuffer.GetRGBfromPalette(tileAppearance[i].bColor);
            }

            for (int i = 0; i < boardAppearance.Length; i++)
            {
                boardAppearanceCached[i].icon = Program.tileNumbersStartWithZero ? boardAppearance[i].icon[..2] : boardAppearanceAlt[i].icon[..2];
                boardAppearanceCached[i].fColor = ConsoleBuffer.GetRGBfromPalette(boardAppearance[i].fColor);
                boardAppearanceCached[i].bColor = ConsoleBuffer.GetRGBfromPalette(boardAppearance[i].bColor);
            }
        }

        public static void drawTiles(int xOffset, int yOffset, (int top, int bottom, int left, int right) cuts)
        {
            (int topCut, int bottomCut, int leftCut, int rightCut) = cuts;

            int width = GameLogic.width;
            int height = GameLogic.height;

            for (int y = topCut; y < height - bottomCut; y++)
            {
                int conX = xOffset + leftCut * 2;
                int conY = y + yOffset;

                ConsoleBuffer.SetCursorPosition(conX, conY);

                for (int x = leftCut; x < width - rightCut; x++)
                {
                    string icon;
                    ConsoleBuffer.RGB fColor;
                    ConsoleBuffer.RGB bColor;
                    
                    TileState tileState = GameLogic.getTileState(x, y);

                    if (tileState != TileState.Revealed)
                    {
                        (icon, fColor, bColor) = tileAppearanceCached[(int)tileState];
                    }
                    else
                    {
                        int boardTile = (int)GameLogic.getBoardTile(x, y);

                        (icon, fColor, bColor) = boardAppearanceCached[boardTile];
                    }

                    ConsoleBuffer.SetColor(fColor, bColor);

                    ConsoleBuffer.Write(icon);
                }
            }

            ConsoleBuffer.ResetColor();
        }

        public static void drawBorders(int xOffset, int yOffset, (int top, int bottom, int left, int right) cuts)
        {
            (int topCut, int bottomCut, int leftCut, int rightCut) = cuts;

            int width = GameLogic.width;
            int height = GameLogic.height;

            int x2 = width + 2;
            int y2 = height + 2;

            for (int y = topCut; y < y2 - bottomCut; y++)
            {
                int xStart = xOffset + leftCut * 2;

                int xEnd = xOffset + (x2 - 1) * 2;

                ConsoleBuffer.SetCursorPosition(xStart, y + yOffset);

                ConsoleBuffer.BackgroundColor(ConsoleBufferColor.White);

                if (y == 0 || y == y2 - 1)
                {
                    int length = (x2 - leftCut - rightCut) * 2;

                    length = Math.Max(length, 0);

                    ConsoleBuffer.Write(new string(' ', length));
                }
                else
                {
                    if (leftCut < 1 && rightCut < x2)
                    {
                        ConsoleBuffer.Write("  ");
                    }

                    if (rightCut < 1 && leftCut < x2)
                    {
                        ConsoleBuffer.ResetColor();

                        ConsoleBuffer.SetCursorPosition(xEnd, y + yOffset);

                        ConsoleBuffer.BackgroundColor(ConsoleBufferColor.White);

                        ConsoleBuffer.Write("  ");
                    }
                }

                ConsoleBuffer.ResetColor();
            }
        }
    }
}