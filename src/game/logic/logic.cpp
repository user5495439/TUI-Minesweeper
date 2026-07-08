#include "logic.h"

namespace game::logic
{
//public:
    GameLogic::GameLogic(GameConfig Config) : config(Config)
    {
        vectorSizes = config.sizes + config.borderThickness * 2; // left & right border, top & bottom border

        boardStartPos = config.borderThickness;
        boardEndPos = boardStartPos + config.sizes;

        board = std::vector<Tile>(vectorSizes.x * vectorSizes.y);

        rng.seed(config.seed);

        revealedTileCounter = 0;
        gameMoves = 0;
        remainingMines = config.mines;
        gameStatus = enums::GameStatus::Ongoing;

        generateMines();
        generateNeighbours();
        generateBorder();
    }

    void GameLogic::revealTile(core::XY xy)
    {
        RevealResult result = tileReveal(xy);

        switch (result)
        {
            case RevealResult::Neutral:
                break;
            case RevealResult::Safe:
                gameMoves++;
                break;
            case RevealResult::Won:
                gameOver(enums::GameStatus::Won);
                break;
            case RevealResult::Lost:
                gameOver(enums::GameStatus::Lost);
                break;
        }
    }

    void GameLogic::placeFlag(core::XY xy)
    {
        Tile* tile = getTilePtr(xy);

        if (tile == nullptr)
            return;

        switch (tile->state)
        {
            case TileState::Hidden:
                tile->state = TileState::Flag;
                remainingMines -= 1;
                return;
            case TileState::Revealed:
                return;
            case TileState::Flag:
                if (config.qMarkEnabled)
                    tile->state = TileState::QMark;
                else
                    tile->state = TileState::Hidden;

                remainingMines += 1;
                return;
            case TileState::QMark:
                tile->state = TileState::Hidden;
                return;
            default:    // invalid value
                return;
        }
    }

    bool GameLogic::inBounds(core::XY xy)
    {
        return xy.x >= 0 && xy.x < vectorSizes.x && xy.y >= 0 && xy.y < vectorSizes.y;
    }

    void GameLogic::setTile(core::XY xy, int number, TileState state)
    {
        if (config.editBoard == false)
            return;

        Tile* tile = getTilePtr(xy);

        if (tile == nullptr)
            return;

        tile->number = number;
        tile->state = state;
    }

//private:
    enums::PublicTile GameLogic::tileToPublicTile(const Tile* tile)
    {
        if (tile == nullptr) return enums::PublicTile::OOB;

        bool condition = tile->state == TileState::Revealed;

        int index = condition ? tile->number : static_cast<int>(tile->state) + 10; // TileState starts at number 10 in PublicTile

        return static_cast<enums::PublicTile>(index);
    }

    GameLogic::Tile GameLogic::publicTileToTile(const enums::PublicTile pTile)
    {
        Tile tile;

        int pTileInt = static_cast<int>(pTile);

        if (pTileInt < 10)
        {
            tile.number = pTileInt;
            tile.state = TileState::Revealed;
        }
        else
        {
            tile.number = BoardTile::Unknown;
            tile.state = static_cast<TileState>(pTileInt - 10);
        }

        return tile;
    }

    RevealResult GameLogic::tileReveal(core::XY xy)
    {
        Tile* tile = getTilePtr(xy);

        if (tile == nullptr)
            return RevealResult::Neutral;

        RevealResult result = RevealResult::Neutral;

        if (tile->state == TileState::Hidden)
        {
            tile->state = TileState::Revealed;
            revealedTileCounter++;

            if (tile->number == BoardTile::Mine)
            {
                tile->state = TileState::ExplodedMine;
                
                result = RevealResult::Lost;
            }
            else
            {
                if (config.sizes.x * config.sizes.y == revealedTileCounter + config.mines)
                {
                    result = RevealResult::Won;
                }
                else if (tile->number == 0)
                {
                    if (config.revealZeroes) result = massTileReveal(xy, true);
                }
                else
                {
                    result = RevealResult::Safe;
                }
            }
        }
        else if (tile->state == TileState::Revealed && config.quickReveal)
        {
            result = massTileReveal(xy, false);
        }

        return result;
    }

    RevealResult GameLogic::massTileReveal(core::XY xy, bool skipFlagsCheck)
    {
        Tile* tile = getTilePtr(xy);

        if (tile == nullptr)
            return RevealResult::Neutral;

        int nFlags = 0;
        int nQmarks = 0;
        bool safeResult = false;

        // two passes, first pass checks if the appropriate amount of flags are placed to neighbouring tiles and second one actually reveals them
        for (int i = skipFlagsCheck; i < 2; i++)
        {
            for (core::XY ixy { -1, -1 }; ixy.x <= 1; ixy.x++)
                for (ixy.y = -1; ixy.y <= 1; ixy.y++)
                {
                    if (ixy.x == 0 && ixy.y == 0)
                        continue;

                    // nxy, neighbour x and y
                    core::XY nxy
                    {
                        xy.x + ixy.x,
                        xy.y + ixy.y 
                    };

                    // nTile, neighbour tile
                    Tile* nTile = getTilePtr(nxy);

                    if (nTile == nullptr)
                        continue;
                    
                    // first pass, calculate flags and question marks
                    if (i == 0)
                    {
                        if (nTile->state == TileState::Flag)
                            nFlags++;

                        if (nTile->state == TileState::QMark)
                            nQmarks++;
                    }

                    // second pass, check if the neighbour tile is a hidden tile, and if true, reveal the tile
                    else if (nTile->state == TileState::Hidden)
                    {
                        RevealResult result = tileReveal(nxy);

                        if (result == RevealResult::Safe)
                            safeResult = true;

                        if (result != RevealResult::Neutral && result != RevealResult::Safe) // ignore outputs neutral and safe
                            return result;
                    }
                }

            if (
                i == 0 &&                         // if first pass (count flags and question marks)
                nFlags != tile->number &&         // and number of flags doesn't match what the board tile says
                (nFlags > tile->number ||         // and number of flags is higher than what the board tile says
                nFlags + nQmarks <= tile->number) // or number of flags and question marks is smaller or equal than what the board tile says
            )
                return RevealResult::Neutral;     // return neutral
        }

        return (nFlags + nQmarks > 0 && safeResult) ? RevealResult::Safe : RevealResult::Neutral;  // return neutral if no neighbouring flags or question marks and safeResult is false (means the tile is a zero or no new tiles revealed)
    }

    void GameLogic::placeFlagsOnMines()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                Tile* tile = getTilePtr(xy);

                if (tile->state == TileState::Flag)        // skip already placed flags
                    continue;

                if (tile->number == BoardTile::Mine)       // if current tile is a mine
                    tile->state = TileState::Flag;         // place a flag on it
            }
    }

    void GameLogic::revealMines()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                Tile* tile = getTilePtr(xy);

                if (tile->state == TileState::ExplodedMine)            // skip exploded mines
                    continue;

                if (tile->number == BoardTile::Mine)              // if current tile is a mine
                {
                    if (config.minesReplaceFlags || tile->state != TileState::Flag)
                        tile->state = TileState::Revealed;         // reveal it
                }

                // if current tile has a misplaced flag, place a misplaced flag on it
                else if (config.placeMisplacedFlags && tile->state == TileState::Flag) 
                    tile->state = TileState::MisplacedFlag;
            }
    }

    void GameLogic::generateMines()
    {
        core::XY xy;

        for (int i = 0; i < config.mines; i++)
        {
            xy.x = std::uniform_int_distribution<int>(boardStartPos.x, boardEndPos.x - 1)(rng);
            xy.y = std::uniform_int_distribution<int>(boardStartPos.y, boardEndPos.y - 1)(rng);

            int* number = &getTilePtr(xy)->number;

            if (*number != BoardTile::Mine)
                *number = BoardTile::Mine;
            else
                i--;
        }
    }

    void GameLogic::generateNeighbours()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                int* number = &getTilePtr(xy)->number;

                if (*number == BoardTile::Mine) // don't do anything if tile is a mine
                    continue;
                
                // check tiles in a 3x3 grid around the current tile
                for (core::XY ixy { -1, -1 }; ixy.x <= 1; ixy.x++)
                    for (ixy.y = -1; ixy.y <= 1; ixy.y++)
                    {
                        if (ixy.x == 0 && ixy.y == 0)
                            continue;

                        // nxy, neighbour x and y
                        core::XY nxy
                        {
                            xy.x + ixy.x,
                            xy.y + ixy.y 
                        };

                        // nTile, neighbour tile
                        Tile* nTile = getTilePtr(nxy);

                        if (nTile == nullptr)
                            continue;

                        // check if the neighbour tile is a mine, and if it is, add 1 to the current tile
                        if (nTile->number == BoardTile::Mine)
                            *number += 1;
                    }
            }
    }

    void GameLogic::generateBorder()
    {
        core::XY xyTemp;

        for (int i = 0; i < config.borderThickness; i++)
        {
            // top and bottom borders
            for (core::XY xy { 0, i }; xy.x < vectorSizes.x; xy.x++)
            {
                xyTemp = xy;

                board[getBoardIndex(xyTemp)] = borderTile;

                xyTemp.y = vectorSizes.y - 1 - i;

                board[getBoardIndex(xyTemp)] = borderTile;
            }

            // left and right
            for (core::XY xy { i, config.borderThickness }; xy.y < vectorSizes.y - config.borderThickness; xy.y++)
            {
                xyTemp = xy;

                board[getBoardIndex(xyTemp)] = borderTile;

                xyTemp.x = vectorSizes.x - 1 - i;

                board[getBoardIndex(xyTemp)] = borderTile;
            }
        }
    }

    void GameLogic::gameOver(enums::GameStatus status)
    {
        gameStatus = status;

        switch (gameStatus)
        {
            default:
            case enums::GameStatus::Ongoing:
                break;
            case enums::GameStatus::Won:
                placeFlagsOnMines();
                break;
            case enums::GameStatus::Lost:
                revealMines();
                break;
        }
    }
}