namespace MineSweeper
{
    class GameLogic
    {
        public static int mines { get; private set; }

        public static int width { get; private set; }
        
        public static int height { get; private set; }

        public static int seed { get; private set; }

        public static int revealedTileCounter { get; private set; }

        public static bool qMarkEnabled { get; private set; }   // if a question mark can be placed by right clicking a flag

        public static bool quickReveal { get; private set; }    // if tiles can be quickly revealed by clicking on an already revealed tile

        public static bool revealZeroes { get; private set; }

        private static BoardTile[,] boardTiles;         // -1 = mine, 0 - 8 = neighbour indicators

        private static TileState[,] tileStates;      // 0 = not revealed, 1 = revealed, 2 = flag, (3 = question mark)

        private static Random rng;

        public static void initGame(int Width, int Height, int Mines, int Seed, bool QMarkEnabled = true, bool QuickReveal = true, bool RevealZeroes = true)
        {
            boardTiles = new BoardTile[Width, Height];

            tileStates = new TileState[Width, Height];

            rng = new Random(Seed);


            width = Width;
            height = Height;

            seed = Seed;

            mines = Mines;

            revealedTileCounter = 0;

            qMarkEnabled = QMarkEnabled;

            quickReveal = QuickReveal;

            revealZeroes = RevealZeroes;

            generateMines();

            generateNeighbours();
        }

        private static void generateMines()
        {
            for (int i = 0; i < mines; i++)
            {
                int x = rng.Next(width);
                int y = rng.Next(height);

                if (boardTiles[x, y] != BoardTile.Mine)
                    boardTiles[x, y] = BoardTile.Mine;
                else
                    i--;
            }
        }

        private static void generateNeighbours()
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                    if (boardTiles[x, y] != BoardTile.Mine)          // don't do anything if tile is a mine
                        // check tiles in a 3x3 grid around the current tile
                        for (int ix = -1; ix <= 1; ix++)
                            for (int iy = -1; iy <= 1; iy++)
                            {
                                int nx = x + ix;    // nx, neighbour x
                                int ny = y + iy;    // ny, neighbour y

                                // check if the neighbour tile is in bounds and that it's a mine, and if true, add 1 to the current tile
                                if (inBounds(nx, ny) && boardTiles[nx, ny] == BoardTile.Mine)
                                    boardTiles[x, y] += 1;
                            }
        }

        // returns 1 if safe, -1 if mine, 0 if no action and 2 if won (no non mine tiles remain)
        public static int revealTile(int x, int y)
        {
            TileState tileState = tileStates[x, y];
            BoardTile boardTile = boardTiles[x, y];

            if (tileState == TileState.Hidden)   // not revealed
            {
                tileStates[x, y] = TileState.Revealed; // revealed

                if (boardTile == BoardTile.Mine)  // mine
                {
                    tileStates[x, y] = TileState.ExplodedMine; // clicked mine
                    return -1;
                }
                else
                {
                    revealedTileCounter++;

                    if (width * height == revealedTileCounter + mines)
                        return 2;

                    if (boardTiles[x, y] == 0 && revealZeroes)
                        return revealZeroTiles(x, y);
                    
                    return 1;
                }
            }
            else if (tileState == TileState.Revealed && quickReveal)
            {
                return quickRevealTiles(x, y);
            }

            return 0;
        }

        // quick reveal system so it's less annoying to play to some
        // same return stuff as revealTile()
        public static int quickRevealTiles(int x, int y)
        {
            int nFlags = 0;
            int nQmarks = 0;
            bool oneResult = false;

            // two passes, first pass checks if the appropriate amount of flags are placed to neighbouring tiles and second one actually reveals them
            for (int i = 0; i < 2; i++)
            {
                for (int ix = -1; ix <= 1; ix++)
                    for (int iy = -1; iy <= 1; iy++)
                    {
                        int nx = x + ix;    // nx, neighbour x
                        int ny = y + iy;    // ny, neighbour y

                        if (!inBounds(nx, ny))
                            continue;

                        TileState tileState = tileStates[nx, ny];
                        
                        // first pass, calculate flags
                        if (i == 0)
                        {
                            if (tileState == TileState.Flag)
                                nFlags++;

                            if (tileState == TileState.QMark)      // i left this here if i want to incorporate question marks into the quickReveal system somehow
                                nQmarks++;
                        }

                        // second pass, check if the neighbour tile is not a revealed tile, and if true, reveal the tile
                        else if (tileState != TileState.Revealed)
                        {
                            int result = revealTile(nx, ny);

                            if (result == 1)
                                oneResult = true;

                            if (result != 0 && result != 1) // ignore outputs 0 and 1 (neutral and safe)
                                return result;
                        }
                    }

                if (nFlags != (int)boardTiles[x, y])  // if the neighboring flags amount doesn't match what the board tile says, return 0 (neutral)
                    return 0;
            }

            return (nFlags > 0 && oneResult) ? 1 : 0;  // return 0 if no neighbouring flags and oneResult is false (means the tile is a zero or no new tiles revealed)
        }

        // again, same return stuff as revealTile() except no 1 (safe click) return
        public static int revealZeroTiles(int x, int y)
        {
            for (int ix = -1; ix <= 1; ix++)
                for (int iy = -1; iy <= 1; iy++)
                {
                    int nx = x + ix;    // nx, neighbour x
                    int ny = y + iy;    // ny, neighbour y

                    if ((x != nx || y != ny) && inBounds(nx, ny) && tileStates[nx, ny] != TileState.Revealed)
                    {
                        int result = revealTile(nx, ny);

                        if (result != 0 && result != 1)
                            return result;
                    }
                }

            return 0;
        }

        // reveals all mines
        public static void revealMines(bool misplacedFlags = true, bool minesReplaceFlags = false)
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                {
                    TileState tileState = tileStates[x, y];
                    BoardTile boardTile = boardTiles[x, y];

                    if (tileState == TileState.ExplodedMine)            // skip exploded mines
                        continue;

                    if (boardTile == BoardTile.Mine)              // if current tile is a mine
                    {
                        if (minesReplaceFlags || tileState != TileState.Flag)
                            tileStates[x, y] = TileState.Revealed;         // reveal it
                    }

                    // if current tile has a misplaced flag, place a misplaced flag on it
                    else if (misplacedFlags && tileState == TileState.Flag) 
                        tileStates[x, y] = TileState.MisplacedFlag;
                }
        }

        // places flags on all mines, intended to run when the game ends (or if you're naughty, you will use this to cheat)
        public static void placeFlagsOnMines()
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                {
                    TileState tileState = tileStates[x, y];
                    BoardTile boardTile = boardTiles[x, y];

                    if (tileState == TileState.Flag)        // skip already placed flags
                        continue;

                    if (boardTile == BoardTile.Mine)          // if current tile is a mine
                        tileStates[x, y] = TileState.Flag;         // place a flag on it
                }
        }

        // returns 1 if placed a flag, -1 if removed and 0 if neither
        public static int placeFlag(int x, int y)
        {
            TileState tileState = tileStates[x, y];

            switch (tileState)
            {
                case TileState.Hidden:     // not revealed
                    tileStates[x, y] = TileState.Flag;
                    return 1;
                case TileState.Revealed:     // revealed
                    return 0;
                case TileState.Flag:     // flag
                    if (qMarkEnabled)
                        tileStates[x, y] = TileState.QMark;
                    else
                        tileStates[x, y] = TileState.Hidden;
                    return -1;
                case TileState.QMark:     // question mark
                    tileStates[x, y] = TileState.Hidden;
                    return 0;
                default:    // invalid value
                    return 0;
            }
        }

        // returns true if coordinate is in game bounds and false if not
        public static bool inBounds(int x, int y)
        {
            return x >= 0 && x < width && y >= 0 && y < height;
        }

        public static BoardTile getBoardTile(int x, int y)
        {
            return boardTiles[x, y];
        }

        public static TileState getTileState(int x, int y)
        {
            return tileStates[x, y];
        }
    }
}