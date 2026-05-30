namespace MineSweeper
{
    class GameDraw
    {
        private const bool useDefaultTerminalBGColor = false;

        private static string cellIcons(int input)
        {
            switch (input)
            {
                case -1:    // mine
                    return "🯧🯦";    // alt: 𜷂𜷖
                case 0:
                    return "  ";
                default:
                    if (input >= 1 && input <= 8)   // the numbers
                        if (Program.tileNumbersStartWithZero)
                            return '0' + input.ToString();
                        else
                            return input.ToString() + input.ToString();
                        return "??";
            }
        }

        private static ConsoleBufferColor boardFColors(int input)
        {
            switch (input)
            {
                case (int)BoardTile.Mine:    // mine
                    return ConsoleBufferColor.DarkGray;
                case 0:
                    return ConsoleBufferColor.Black;
                case 1:
                    return ConsoleBufferColor.Blue;
                case 2:
                    return ConsoleBufferColor.Green;
                case 3:
                    return ConsoleBufferColor.Red;
                case 4:
                    return ConsoleBufferColor.Magenta;
                case 5:
                    return ConsoleBufferColor.DarkYellow;
                case 6:
                    return ConsoleBufferColor.Cyan;
                case 7:
                    return ConsoleBufferColor.DarkGray;
                case 8:
                    return ConsoleBufferColor.DarkRed;
                default:
                    return ConsoleBufferColor.Black;
            }
        }

        private static ConsoleBufferColor boardBColors(int input)
        {
            switch (input)
            {
                default:
                    return ConsoleBufferColor.Black;
            }
        }

        private static (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor) tileAppearance(int boardTile, TileState tileState)
        {
            string icon;
            ConsoleBufferColor fColor;
            ConsoleBufferColor bColor;

            switch (tileState)
            {
                case TileState.Hidden:     // not revealed
                    icon = "[]";

                    fColor = ConsoleBufferColor.Gray;
                    bColor = ConsoleBufferColor.DarkGray;
                    break;
                case TileState.Revealed:     // revealed
                    icon = cellIcons(boardTile);

                    fColor = boardFColors(boardTile);
                    bColor = boardBColors(boardTile);
                    break;
                case TileState.Flag:     // flag
                    icon = "[]";

                    fColor = ConsoleBufferColor.Red;
                    bColor = ConsoleBufferColor.DarkGray;
                    break;
                case TileState.QMark:     // question mark
                    icon = "??";

                    fColor = ConsoleBufferColor.Black;
                    bColor = ConsoleBufferColor.DarkGray;
                    break;
                case TileState.MisplacedFlag:     // misplaced flag
                    icon = "[]";

                    fColor = ConsoleBufferColor.DarkGray;
                    bColor = ConsoleBufferColor.Red;
                    break;
                case TileState.ExplodedMine:     // clicked mine
                    icon = "🯧🯦";

                    fColor = ConsoleBufferColor.DarkGray;
                    bColor = ConsoleBufferColor.DarkRed;
                    break;
                default:    // invalid
                    icon = "??";

                    fColor = ConsoleBufferColor.White;
                    bColor = ConsoleBufferColor.Black;
                    break;
            }

            return (icon, fColor, bColor);
        }

        public static void drawTiles(int xOffset, int yOffset, (int top, int bottom, int left, int right) cuts)
        {
            (int topCut, int bottomCut, int leftCut, int rightCut) = cuts;

            int width = GameLogic.width;
            int height = GameLogic.height;

            for (int y = topCut; y < height - bottomCut; y++)
            {
                ConsoleBufferColor? previousFColor = null;
                ConsoleBufferColor? previousBColor = null;

                int conX = xOffset + leftCut * 2;
                int conY = y + yOffset;

                ConsoleBuffer.SetCursorPosition(conX, conY);

                for (int x = leftCut; x < width - rightCut; x++)
                {
                    int boardTile = (int)GameLogic.getBoardTile(x, y);
                    TileState tileState = GameLogic.getTileState(x, y);

                    (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor) = tileAppearance(boardTile, tileState);

                    if (previousBColor == null || previousBColor != bColor)
                    {
                        if (bColor == ConsoleBufferColor.Black && useDefaultTerminalBGColor)
                            ConsoleBuffer.OldANSIColor(ConsoleBufferColor.Background, true);
                        else
                            ConsoleBuffer.BackgroundColor(bColor);

                        previousBColor = bColor;
                    }

                    if (previousFColor == null || previousFColor != fColor)
                    {
                        ConsoleBuffer.ForegroundColor(fColor);

                        previousFColor = fColor;
                    }

                    ConsoleBuffer.Write(icon);
                }

                ConsoleBuffer.ResetColor();    // reset color when moving on to next line
            }
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