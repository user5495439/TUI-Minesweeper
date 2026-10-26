#include "logic.h"
#include "enums/MineState.h"
#include "enums/TileState.h"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace game::logic
{
//public:
    GameLogic::GameLogic(GameConfig Config) : config(Config)
    {
        vectorSizes = config.sizes + config.borderThickness * 2; // left & right border, top & bottom border

        boardStartPos = config.borderThickness;
        boardEndPos = boardStartPos + config.sizes;

        board = std::vector<Tile>(vectorSizes.x * vectorSizes.y);

        randomProviderSeed(config.seed);

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
        Tile* tile = tilePtr(xy);

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

    enums::PublicTile GameLogic::tileToPublicTile(const Tile* tile)
    {
        if (tile == nullptr) return enums::PublicTile::OOB;

        bool mineCondition = tile->mine == MineState::Yes;

        bool revealedCondition = tile->state == TileState::Revealed;

        int index = revealedCondition ? (mineCondition ? static_cast<int>(enums::PublicTile::Mine) : tile->number) : static_cast<int>(tile->state) + 10; // TileState starts at number 10 in PublicTile

        return static_cast<enums::PublicTile>(index);
    }

    GameLogic::Tile GameLogic::publicTileToTile(const enums::PublicTile pTile)
    {
        Tile tile;

        int pTileInt = static_cast<int>(pTile);

        bool isAMine = pTile == enums::PublicTile::Mine || pTile == enums::PublicTile::RevealedMine;

        if (pTileInt < 10)
        {
            tile.number = isAMine ? invalidNumber : pTileInt;
            tile.state = TileState::Revealed;
            tile.mine = isAMine ? MineState::Yes : MineState::No;
        }
        else
        {
            tile.number = invalidNumber;
            tile.state = static_cast<TileState>(pTileInt - 10);
            tile.mine = isAMine ? MineState::Yes : MineState::Unknown;
        }

        return tile;
    }

//private:
    RevealResult GameLogic::tileReveal(core::XY xy)
    {
        Tile* tile = tilePtr(xy);

        if (tile == nullptr)
            return RevealResult::Neutral;

        RevealResult result = RevealResult::Neutral;

        if (tile->state == TileState::Hidden)
        {
            tile->state = TileState::Revealed;
            revealedTileCounter++;

            if (tile->mine == MineState::Yes)
            {
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
        Tile* tile = tilePtr(xy);

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
                    Tile* nTile = tilePtr(nxy);

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
                Tile* tile = tilePtr(xy);

                if (tile->state == TileState::Flag)        // skip already placed flags
                    continue;

                if (tile->mine == MineState::Yes)          // if current tile is a mine
                    tile->state = TileState::Flag;         // place a flag on it
            }
    }

    void GameLogic::revealMines()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                Tile* tile = tilePtr(xy);

                if (tile->mine == MineState::Yes && tile->state != TileState::Revealed) // if current tile is a mine and it's not Revealed
                {
                    if (config.minesReplaceFlags || tile->state != TileState::Flag)
                        tile->state = TileState::RevealedMine;         // make it into a RevealedMine
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
            xy.y = randomProvider(boardStartPos.y, boardEndPos.y - 1);
            xy.x = randomProvider(boardStartPos.x, boardEndPos.x - 1);

            MineState* mine = &tilePtr(xy)->mine;

            if (*mine != MineState::Yes)
                *mine = MineState::Yes;
            else
                i--;
        }
    }

    void GameLogic::generateNeighbours()
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                int* number = &tilePtr(xy)->number;
                MineState* mine = &tilePtr(xy)->mine;

                //if (*mine == MineState::Yes) // don't do anything if tile is a mine
                    //continue;
                
                // check tiles in a 3x3 grid around the current tile
                for (core::XY ixy { -1, -1 }; ixy.x <= 1; ixy.x++)
                    for (ixy.y = -1; ixy.y <= 1; ixy.y++)
                    {
                        //if (ixy.x == 0 && ixy.y == 0)
                            //continue;

                        // nxy, neighbour x and y
                        core::XY nxy
                        {
                            xy.x + ixy.x,
                            xy.y + ixy.y 
                        };

                        // nTile, neighbour tile
                        Tile* nTile = tilePtr(nxy);

                        if (nTile == nullptr)
                            continue;

                        // check if the neighbour tile is a mine, and if it is, add 1 to the current tile
                        if (nTile->mine == MineState::Yes)
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

    int GameLogic::randomProvider(int a, int b)
    {
    #ifdef DEBUG
        if (constant::debugRandom)
        {
            int range = std::abs(b - a) + 1;

            int unadjusted = std::floor(rngDebug() * range);

            int smaller = std::min(a, b);

            return smaller + unadjusted;
        }
    #endif

        return std::uniform_int_distribution<int>(a, b)(rng);
    }

    void GameLogic::randomProviderSeed(int seed)
    {
    #ifdef DEBUG
        if (constant::debugRandom)
        {
            rngDebug = mulberry32(static_cast<uint32_t>(seed));
            return;
        }
    #endif

        rng.seed(seed);
    }

#ifdef DEBUG
    // chatgpt made this function because this is way too complicated for my beginner ass
    std::function<double()> GameLogic::mulberry32(uint32_t seed) {
        return [seed]() mutable -> double {
            uint32_t t = seed += 0x6D2B79F5;

            t = (t ^ (t >> 15)) * (t | 1);
            t ^= t + ((t ^ (t >> 7)) * (t | 61));

            return static_cast<double>(t ^ (t >> 14)) / 4294967296.0;
        };
    }
#endif
}