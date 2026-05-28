// yep, cheats. can be enabled by changing a variable in Program and pressing a button on your keyboard or whatever

using System.CodeDom.Compiler;

namespace MineSweeper
{
    class GameSolverCheat
    {
        private static int cheatStage = 0;

        private static int[,] visibleBoardTiles;

        private static TileState[,] tileStates;

        private static int width;

        private static int height;

        private static bool initialized = false;

        private static int remainingMines;

        private static bool qMarkEnabled;

        private static bool quickReveal;

        // i could definitely do this system better and maybe do object oriented programming but i'm too lazy to think
        public static void InitCheats()
        {
            visibleBoardTiles = new int[GameLogic.width, GameLogic.height];

            tileStates = new TileState[GameLogic.width, GameLogic.height];

            width = GameLogic.width;

            height = GameLogic.height;

            remainingMines = GameLogic.mines;

            qMarkEnabled = GameLogic.qMarkEnabled;

            quickReveal = GameLogic.quickReveal;

            initialized = true;
        }

        // we are hackers omg so scary
        public static void Cheat()
        {
            if (!initialized)
                InitCheats();

            switch (cheatStage)
            {
                case 0:
                    UpdateVisibleBoard();
                    PlaceFlagsSimple();
                    break;
                case 1:
                    RevealTilesSimple();
                    break;
            }

            cheatStage++;

            if (cheatStage > 1)
                cheatStage = 0;
        }

        // this function also does tileStates because why not
        public static void UpdateVisibleBoard()
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                {
                    TileState tileState = GameLogic.getTileState(x, y);
                    int boardTile = (int)GameLogic.getBoardTile(x, y);

                    tileStates[x, y] = tileState;
                    if (tileState != TileState.Hidden)      // in this cheat we don't know the board tiles in unrevealed tiles
                        visibleBoardTiles[x, y] = boardTile;
                    else
                        visibleBoardTiles[x, y] = -2;    // -2 is unknown
                }
        }

        public static void PlaceFlagsSimple()
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                {
                    TileState tileState = tileStates[x, y];
                    int boardTile = visibleBoardTiles[x, y];

                    if (boardTile != -2 && boardTile != 0)
                    {
                        FlagSolver(x, y);
                    }
                }
        }

        // reveals tiles in a simple way
        public static void RevealTilesSimple()
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                {
                    TileState tileState = tileStates[x, y];
                    int boardTile = visibleBoardTiles[x, y];

                    if (boardTile != -2 && boardTile != 0)
                    {
                        TileRevealer(x, y);
                    }
                }
        }

        public static void PlaceFlagsSmart()
        {
            for (int x = 0; x < width; x++)         // width, x axis
                for (int y = 0; y < height; y++)    // height, y axis
                {
                    TileState tileState = tileStates[x, y];
                    int boardTile = visibleBoardTiles[x, y];

                    if (boardTile != -2 && boardTile != 0)
                    {
                        SmartFlagSolver(x, y);
                    }
                }
        }

        // reveals tiles if it's safe to reveal them, copied from quickRevealTiles() and i'm too lazy to edit the comments
        public static void TileRevealer(int x, int y)
        {
            int nFlags = 0;
            bool nUnrevealed = false;

            // two passes, first pass checks if the appropriate amount of flags are placed to neighbouring tiles and second one actually reveals them
            for (int i = 0; i < 2; i++)
            {
                // utilize quick reveal system if available
                if (i == 1 && quickReveal)
                {
                    GameInput.LeftClick(x, y);
                }

                for (int ix = -1; ix <= 1; ix++)
                    for (int iy = -1; iy <= 1; iy++)
                    {
                        int nx = x + ix;    // nx, neighbour x
                        int ny = y + iy;    // ny, neighbour y

                        if (!GameLogic.inBounds(nx, ny))
                            continue;

                        TileState tileState = tileStates[nx, ny];
                        
                        // first pass, calculate flags
                        if (i == 0)
                        {
                            if (tileState == TileState.Flag)
                                nFlags++;

                            if (tileState == TileState.Hidden)
                                nUnrevealed = true;
                        }

                        // second pass, check if the neighbour tile is not a revealed tile, and if true, reveal the tile
                        else if (tileState == TileState.Hidden)
                        {
                            if (!quickReveal)
                                GameInput.LeftClick(nx, ny);
                            else
                                LeftClick(nx, ny);  // reveal only locally if quick reveal system
                        }
                    }

                if (nFlags != visibleBoardTiles[x, y] || !nUnrevealed)  // if the neighboring flags amount doesn't match what the board tile says, return 0 (neutral)
                    return;
            }
        }

        // simple "solver" that solves if flags should be placed around a revealed non zero tile and if should then it will place them
        public static void FlagSolver(int x, int y)
        {
            int nUnrevealed = 0;

            // two passes, first pass checks if tile number matches the amount of neighbouring unrevealed tiles and if yes, then second pass will place flags on them
            for (int i = 0; i < 2; i++)
            {
                for (int ix = -1; ix <= 1; ix++)
                    for (int iy = -1; iy <= 1; iy++)
                    {
                        int nx = x + ix;    // nx, neighbour x
                        int ny = y + iy;    // ny, neighbour y

                        if (!GameLogic.inBounds(nx, ny))
                            continue;

                        TileState tileState = tileStates[nx, ny];

                        // first pass, unrevealed
                        if (i == 0)
                        {
                            if (tileState != TileState.Revealed)
                                nUnrevealed++;
                        }

                        // second pass, check if the neighbour tile is a regular hidden tile, and if true, place a flag
                        else if (tileState == TileState.Hidden)
                        {
                            RightClick(nx, ny);
                            GameInput.RightClick(nx, ny);
                        }
                    }

                // if the neighboring unrevealed tiles amount doesn't match what the board tile says, return
                if (nUnrevealed != visibleBoardTiles[x, y])  
                    return;
            }
        }

        // smart flag solver
        public static void SmartFlagSolver(int x, int y)
        {
            // implement
        }

        public static void revealTile(int x, int y)
        {
            TileState tileState = tileStates[x, y];

            if (tileState == TileState.Hidden)   // not revealed
            {
                tileStates[x, y] = TileState.Revealed; // revealed
            }
        }

        public static void LeftClick(int x, int y)
        {
            revealTile(x, y);

            /*switch (GameLogic.revealTile(x, y))
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
            }*/
        }

        // copy of the function in GameInput so the cheat can keep track of stuff in here locally
        public static void RightClick(int x, int y)
        {
            remainingMines -= placeFlag(x, y);
        }

        // copy of the function in GameLogic so the cheat can keep track of stuff in here locally
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
    }
}