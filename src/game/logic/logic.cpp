#include "logic.h"

namespace game::logic
{
//public:
    void GameLogic::generateBoard(GameSettings Settings)
    {
        settings = Settings;

        constexpr int borderThickness = constant::borderThickness;

        vectorSizes =
        {
            settings.width + borderThickness * 2,  // left & right border,
            settings.height + borderThickness * 2  // top & bottom border
        };

        boardStartPos =
        {
            borderThickness,
            borderThickness
        };

        boardEndPos = 
        {
            boardStartPos.x + settings.width,
            boardStartPos.y + settings.height
        };

        board = std::vector<Tile>(vectorSizes.x * vectorSizes.y);

        rng.seed(settings.seed);

        revealedTileCounter = 0;

        generateMines();
        generateNeighbours();
        generateBorder();
    }

    enums::RevealResult GameLogic::revealTile(core::XY xy)
    {
        Tile* tile = &board[getBoardIndex(xy)];

        if (tile->state == enums::TileState::Hidden)   // not revealed
        {
            tile->state = enums::TileState::Revealed; // revealed

            if (tile->number == enums::BoardTile::Mine)  // mine
            {
                tile->state = enums::TileState::ExplodedMine; // clicked mine
                return enums::RevealResult::Lost;
            }
            else
            {
                revealedTileCounter++;

                if (settings.width * settings.height == revealedTileCounter + settings.mines)
                    return enums::RevealResult::Won;

                if (tile->number == 0 && settings.revealZeroes)
                    return revealZeroTiles(xy);
                
                return enums::RevealResult::Safe;
            }
        }
        else if (tile->state == enums::TileState::Revealed && settings.quickReveal)
        {
            return quickRevealTiles(xy);
        }

        return enums::RevealResult::Neutral;
    }

    enums::RevealResult GameLogic::quickRevealTiles(core::XY xy)
    {
        int nFlags = 0;
        //int nQmarks = 0;
        bool safeResult = false;

        // two passes, first pass checks if the appropriate amount of flags are placed to neighbouring tiles and second one actually reveals them
        for (int i = 0; i < 2; i++)
        {
            for (core::XY ixy { -1, -1 }; ixy.x <= 1; ixy.x++)
                for (ixy.y = -1; ixy.y <= 1; ixy.y++)
                {
                    // nxy, neighbour x and y
                    core::XY nxy
                    {
                        xy.x + ixy.x,
                        xy.y + ixy.y 
                    };

                    if (!inBounds(nxy))
                        continue;

                    // nTile, neighbour tile
                    Tile* nTile = &board[getBoardIndex(nxy)];
                    
                    // first pass, calculate flags
                    if (i == 0)
                    {
                        if (nTile->state == enums::TileState::Flag)
                            nFlags++;

                        //if (nTile->state == enums::TileState::QMark)      // i left this here if i want to incorporate question marks into the quickReveal system somehow
                            //nQmarks++;
                    }

                    // second pass, check if the neighbour tile is not a revealed tile, and if true, reveal the tile
                    else if (nTile->state != enums::TileState::Revealed)
                    {
                        enums::RevealResult result = revealTile(nxy);

                        if (result == enums::RevealResult::Safe)
                            safeResult = true;

                        if (result != enums::RevealResult::Neutral && result != enums::RevealResult::Safe) // ignore outputs neutral and safe
                            return result;
                    }
                }

            if (nFlags != board[getBoardIndex(xy)].number)  // if the neighboring flags amount doesn't match what the board tile says, return neutral
                return enums::RevealResult::Neutral;
        }

        return (nFlags > 0 && safeResult) ? enums::RevealResult::Safe : enums::RevealResult::Neutral;  // return neutral if no neighbouring flags and oneResult is false (means the tile is a zero or no new tiles revealed)
    }

    // this method can probably be somehow combined with the quick reveal system method
    enums::RevealResult GameLogic::revealZeroTiles(core::XY xy)
    {
        for (core::XY ixy { -1, -1 }; ixy.x <= 1; ixy.x++)
            for (ixy.y = -1; ixy.y <= 1; ixy.y++)
            {
                // nxy, neighbour x and y
                core::XY nxy
                {
                    xy.x + ixy.x,
                    xy.y + ixy.y 
                };

                if (!inBounds(nxy))
                    continue;

                // nTile, neighbour tile
                Tile* nTile = &board[getBoardIndex(nxy)];

                if ((xy.x != nxy.x || xy.y != nxy.y) && nTile->state != enums::TileState::Revealed)
                {
                    enums::RevealResult result = revealTile(nxy);

                    if (result != enums::RevealResult::Neutral && result != enums::RevealResult::Safe)
                        return result;
                }
            }

        return enums::RevealResult::Neutral;
    }

    void GameLogic::revealMines()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                Tile* tile = &board[getBoardIndex(xy)];

                if (tile->state == enums::TileState::ExplodedMine)            // skip exploded mines
                    continue;

                if (tile->number == enums::BoardTile::Mine)              // if current tile is a mine
                {
                    if (settings.minesReplaceFlags || tile->state != enums::TileState::Flag)
                        tile->state = enums::TileState::Revealed;         // reveal it
                }

                // if current tile has a misplaced flag, place a misplaced flag on it
                else if (settings.placeMisplacedFlags && tile->state == enums::TileState::Flag) 
                    tile->state = enums::TileState::MisplacedFlag;
            }
    }

    void GameLogic::placeFlagsOnMines()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                Tile* tile = &board[getBoardIndex(xy)];

                if (tile->state == enums::TileState::Flag)        // skip already placed flags
                    continue;

                if (tile->number == enums::BoardTile::Mine)       // if current tile is a mine
                    tile->state = enums::TileState::Flag;         // place a flag on it
            }
    }

    int GameLogic::placeFlag(core::XY xy)
    {
        Tile* tile = &board[getBoardIndex(xy)];

        switch (tile->state)
        {
            case enums::TileState::Hidden:     // not revealed
                tile->state = enums::TileState::Flag;
                return 1;
            case enums::TileState::Revealed:   // revealed
                return 0;
            case enums::TileState::Flag:       // flag
                if (settings.qMarkEnabled)
                    tile->state = enums::TileState::QMark;
                else
                    tile->state = enums::TileState::Hidden;
                return -1;
            case enums::TileState::QMark:      // question mark
                tile->state = enums::TileState::Hidden;
                return 0;
            default:    // invalid value
                return 0;
        }
    }

    bool GameLogic::inBounds(core::XY xy)
    {
        return xy.x >= 0 && xy.x < vectorSizes.x && xy.y >= 0 && xy.y < vectorSizes.y;
    }

    GameLogic::Tile GameLogic::getTile(core::XY xy)
    {
        return inBounds(xy) ? board[getBoardIndex(xy)] : oobTile;
    }

//private:
    int GameLogic::getBoardIndex(core::XY xy)
    {
        return xy.y * vectorSizes.x + xy.x;
    }

    void GameLogic::generateMines()
    {
        core::XY xy;

        for (int i = 0; i < settings.mines; i++)
        {
            xy.x = std::uniform_int_distribution<int>(boardStartPos.x, boardEndPos.x - 1)(rng);
            xy.y = std::uniform_int_distribution<int>(boardStartPos.y, boardEndPos.y - 1)(rng);

            int boardIndex = getBoardIndex(xy);

            if (board[boardIndex].number != enums::BoardTile::Mine)
                board[boardIndex].number = enums::BoardTile::Mine;
            else
                i--;
        }
    }

    void GameLogic::generateNeighbours()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                Tile* tile = &board[getBoardIndex(xy)];

                if (tile->number != enums::BoardTile::Mine)          // don't do anything if tile is a mine
                    // check tiles in a 3x3 grid around the current tile
                    for (core::XY ixy { -1, -1 }; ixy.x <= 1; ixy.x++)
                        for (ixy.y = -1; ixy.y <= 1; ixy.y++)
                        {
                            // nxy, neighbour x and y
                            core::XY nxy
                            {
                                xy.x + ixy.x,
                                xy.y + ixy.y 
                            };

                            if (!inBounds(nxy))
                                continue;

                            // nTile, neighbour tile
                            Tile* nTile = &board[getBoardIndex(nxy)];

                            // check if the neighbour tile is a mine, and if it is, add 1 to the current tile
                            if (nTile->number == enums::BoardTile::Mine)
                                tile->number += 1;
                        }
            }
    }

    void GameLogic::generateBorder()
    {
        constexpr int borderThickness = constant::borderThickness;

        core::XY xyTemp;

        for (int i = 0; i < borderThickness; i++)
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
            for (core::XY xy { i, borderThickness }; xy.y < vectorSizes.y - borderThickness; xy.y++)
            {
                xyTemp = xy;

                board[getBoardIndex(xyTemp)] = borderTile;

                xyTemp.x = vectorSizes.x - 1 - i;

                board[getBoardIndex(xyTemp)] = borderTile;
            }
        }
    }
}