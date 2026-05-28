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
                        return input.ToString() + input.ToString();
                    else
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

        private static (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor) tileVisualData(int boardTile, TileState tileState)
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

        // buffer ception, idk why i decided to make a buffer system here when my ConsoleBuffer already has a buffer system, well i guess it has some very minor performance benefits at least
        public static void drawTiles(int xOffset, int yOffset)
        {
            int width = GameLogic.width;
            int height = GameLogic.height;

            for (int y = 0; y < height; y++)
            {
                string buffer = "";
                ConsoleBufferColor? previousFColor = null;
                ConsoleBufferColor? previousBColor = null;

                int conX = xOffset;
                int conY = y + yOffset;

                for (int x = 0; x < width; x++)
                {
                    int boardTile = (int)GameLogic.getBoardTile(x, y);
                    TileState tileState = GameLogic.getTileState(x, y);

                    (string icon, ConsoleBufferColor fColor, ConsoleBufferColor bColor) = tileVisualData(boardTile, tileState);

                    if (previousBColor == null || previousBColor != bColor)
                    {
                        if (bColor == ConsoleBufferColor.Black && useDefaultTerminalBGColor)
                            buffer += ConsoleBuffer.GetOldANSIColor(ConsoleBufferColor.Background, true);
                        else
                            buffer += ConsoleBuffer.GetBackgroundColor(bColor);

                        previousBColor = bColor;
                    }

                    if (previousFColor == null || previousFColor != fColor)
                    {
                        buffer += ConsoleBuffer.GetForegroundColor(fColor);

                        previousFColor = fColor;
                    }

                    buffer += icon;
                }

                buffer += ConsoleBuffer.GetResetColor();    // reset color when moving on to making the next buffer

                ConsoleBuffer.SetCursorPosition(conX, conY);
                ConsoleBuffer.Write(buffer);
            }
        }

        public static void drawBorders(int xOffset, int yOffset)
        {
            int width = GameLogic.width;
            int height = GameLogic.height;

            int x2 = width + 2;
            int y2 = height + 2;

            for (int x = 0; x < x2; x++)
                for (int y = 0; y < y2; y++)
                {
                    if (x == 0 || x == x2 - 1 || y == 0 || y == y2 - 1)
                    {
                        int conX = x * 2 + xOffset;
                        int conY = y + yOffset;

                        ConsoleBuffer.SetCursorPosition(conX, conY);

                        ConsoleBuffer.BackgroundColor(ConsoleBufferColor.White);

                        ConsoleBuffer.Write("  ");

                        ConsoleBuffer.ResetColor();
                    }
                }
        }
    }
}