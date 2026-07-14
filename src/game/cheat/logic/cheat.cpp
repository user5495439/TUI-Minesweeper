#include "cheat.h"
#include <cmath>
#include <cstdint>

namespace game::cheat::logic
{
    CheatsLogic::CheatsLogic(GameLogic* GameLogic) : gameLogic(GameLogic)
    {
        GameLogic::GameConfig config = gameLogic->getConfig();
        gameConfig = config;

        sizes = config.sizes;
        boardStartPos = config.borderThickness;
        boardEndPos = boardStartPos + sizes;

        chancesBoard = std::vector<ChanceTile>(sizes.x * sizes.y);

        config.borderThickness = 0;
        config.mines = 0;
        config.editBoard = true;

        cheatGameLogic = logic::GameLogic(config);

        reset();
    }

    void CheatsLogic::cheat()
    {
        reset();

        findEdgeTiles();

        bool result;
        while (result)
        {
            result = simpleHundredChanceSolver();
            result += simpleZeroChanceSolver();
        }

        isolatedEdgeCellsSolver();
        probabilityStuff();
    }

    void CheatsLogic::reset()
    {
        copyBoard(gameLogic);

        chancesBoard = std::vector<ChanceTile>(sizes.x * sizes.y);
        edgeArr.clear();
    }

    void CheatsLogic::findEdgeTiles()
    {
        loopEachTile([&](core::XY* xy) -> void
        {
            GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);

            if (tile->state == logic::TileState::Revealed)
                return;

            loopNeighbourTiles(*xy, [&](core::XY* nxy) -> void
            {
                // nTile, neighbour tile
                GameLogic::Tile* nTile = cheatGameLogic.getTilePtr(*nxy);

                if (nTile == nullptr)
                    return;

                if (nTile->state == TileState::Revealed)
                {
                    chancesBoard[getBoardIndex(*xy)].edgeTile = true;
                    *nxy += 2;
                }
            });
        });
    }

    void CheatsLogic::copyBoard(GameLogic* sourceBoard)
    {
        for (core::XY xy = boardStartPos; xy.x < boardEndPos.x; xy.x++) // width, x axis
            for (xy.y = boardStartPos.y; xy.y < boardEndPos.y; xy.y++)  // height, y axis
            {
                core::XY targetXY = xy - boardStartPos;

                enums::PublicTile pTile = sourceBoard->getPublicTile(xy);

                *cheatGameLogic.getTilePtr(targetXY) = GameLogic::publicTileToTile(pTile);
            }
    }

    bool CheatsLogic::simpleHundredChanceSolver()
    {
        bool result = false;

        loopEachTile([&](core::XY* xy) -> void
        {
            int nHidden = 0;
            std::vector<core::XY> xys;
            GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);

            if (tile->state != logic::TileState::Revealed || tile->number == 0)
                return;

            loopNeighbourTiles(*xy, [&](core::XY* nxy) -> void
            {
                // nChanceTile, neighbour chance tile
                ChanceTile* nChanceTile = getChanceTilePtr(*nxy);

                if (nChanceTile == nullptr)
                    return;

                if (nChanceTile->edgeTile == true && (nChanceTile->chance == -1 || nChanceTile->chance == 100))
                {
                    nHidden++;
                    xys.push_back(*nxy);
                }
            });

            if (nHidden == tile->number)
            {
                for (size_t i = 0; i < xys.size(); i++)
                {
                    ChanceTile* chanceTile = getChanceTilePtr(xys[i]);

                    if (chanceTile->chance != 100)
                    {
                        chanceTile->chance = 100;
                        result = true;
                    }
                }
            }
        });

        return result;
    }

    bool CheatsLogic::simpleZeroChanceSolver()
    {
        bool result = false;

        loopEachTile([&](core::XY* xy) -> void
        {
            int nHundred = 0;
            std::vector<core::XY> xys;
            GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);

            if (tile->state != logic::TileState::Revealed || tile->number == 0)
                return;

            loopNeighbourTiles(*xy, [&](core::XY* nxy) -> void
            {
                // nChanceTile, neighbour chance tile
                ChanceTile* nChanceTile = getChanceTilePtr(*nxy);

                if (nChanceTile == nullptr)
                    return;

                if (nChanceTile->chance == 100)
                {
                    nHundred++;
                }
                else
                {
                    if (nChanceTile->edgeTile == false || nChanceTile->chance != -1)
                        return;

                    xys.push_back(*nxy);
                }
            });

            if (nHundred == tile->number)
            {
                for (size_t i = 0; i < xys.size(); i++)
                {
                    ChanceTile* chanceTile = getChanceTilePtr(xys[i]);

                    if (chanceTile->chance != 0)
                    {
                        chanceTile->chance = 0;
                        result = true;
                    }
                }
            }
        });

        return result;
    }

    void CheatsLogic::isolatedEdgeCellsSolver()
    {
        loopEachTile([this](core::XY* xy) -> void
        {
            int nEdge = 0;
            std::vector<core::XY> xys;
            GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);

            if (tile->state != logic::TileState::Revealed || tile->number == 0)
                return;

            // count neighbour tiles that are edge tiles
            loopNeighbourTiles(*xy, [&](core::XY* nxy) -> void
            {
                // nChanceTile, neighbour chance tile
                ChanceTile* nChanceTile = getChanceTilePtr(*nxy);

                if (nChanceTile == nullptr)
                    return;

                if (nChanceTile->edgeTile)
                {
                    nEdge++;
                    xys.push_back(*nxy);
                }
            });

            if (nEdge > 2)
            {
                int amount = 0;

                for (size_t i = 0; i < xys.size(); i++)
                {
                    // sxy, select x and y
                    core::XY sxy = xys[i];

                    int nRevealed = 0;

                    // count neighbour tiles that are revealed
                    loopNeighbourTiles(sxy, [&](core::XY* nxy) -> void
                    {
                        // nTile, neighbour tile
                        GameLogic::Tile* nTile = cheatGameLogic.getTilePtr(*nxy);

                        if (nTile == nullptr)
                            return;

                        if (nTile->state == logic::TileState::Revealed)
                            nRevealed++;
                    });

                    if (nRevealed == 1)
                        amount++;
                }

                if (amount == nEdge)
                {
                    int8_t chance = std::round((static_cast<double>(tile->number) / nEdge) * 100);

                    // for all neighbours
                    loopNeighbourTiles(*xy, [&](core::XY* nxy) -> void
                    {
                        // nChanceTile, neighbour chance tile
                        ChanceTile* nChanceTile = getChanceTilePtr(*nxy);

                        if (nChanceTile == nullptr || nChanceTile->edgeTile == false || nChanceTile->chance != -1)
                            return;

                        nChanceTile->chance = chance;
                    });
                }
            }
        });
    }

    core::XY CheatsLogic::getNextEdgeTile(core::XY Xy)
    {
        for (core::XY xy = Xy; xy.y < sizes.y; xy.y++)
        {
            for (xy.x = Xy.x; xy.x < sizes.x; xy.x++)
            {
                ChanceTile* chanceTile = getChanceTilePtr(xy);

                if (chanceTile->edgeTile == true && chanceTile->chance == -1)
                    return xy;
            }

            Xy.x = 0;
        }

        return { -1, -1 };
    }

    void CheatsLogic::probabilityStuff()
    {
        std::vector<ArrangementTile> arrBoard;

        // exy, edge x and y
        core::XY exy = getNextEdgeTile({0, 0});

        while (exy.y > -1)
        {
            arrBoard.push_back( { exy, IsMine::Unknown } );

            //getChanceTilePtr(exy) -> edgeTile = false;    // debug
            
            exy = getNextEdgeTile( { exy.x + 1, exy.y } );
        }

        if (arrBoard.size() > 0)
        {
            generateArrangements(&arrBoard, 0 );
            probabilityCalculation();
        }
        else
        {
            int nonEdge = nonEdgeCount();
            int nHundred = hundredCount();
            int remainingMines = gameConfig.mines - nHundred;

            loopEachTile([&](core::XY* xy) -> void
            {
                GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);
                ChanceTile* chanceTile = getChanceTilePtr(*xy);

                if (tile->state != logic::TileState::Revealed && chanceTile->edgeTile == false)
                    chanceTile->chance = std::round(static_cast<double>(remainingMines) / nonEdge * 100);
            });
        }
    }

    void CheatsLogic::generateArrangements(std::vector<ArrangementTile>* arrBoard, int index)
    {
        core::XY xy = (*arrBoard)[index].xy;

        if (canBeMine(arrBoard, xy))
        {
            std::vector<ArrangementTile> patternYes = *arrBoard;
            
            patternYes[index].mineStatus = IsMine::Yes;

            if (index + 1 < arrBoard->size())    // safer than the index < grid.length - 1 in js, also there might or might not be a bug here (even in the js version)
                generateArrangements(&patternYes, index + 1);
            else
                edgeArr.push_back(patternYes);
        }

        if (canNotBeMine(arrBoard, xy))
        {
            std::vector<ArrangementTile> patternNo = *arrBoard;
            
            patternNo[index].mineStatus = IsMine::No;

            if (index + 1 < arrBoard->size())
                generateArrangements(&patternNo, index + 1);
            else
                edgeArr.push_back(patternNo);
        }
    }

    bool CheatsLogic::canBeMine(std::vector<ArrangementTile>* arrBoard, core::XY xy)
    {
        bool result = true;

        loopNeighbourTiles(xy, [&](core::XY* nxy) -> void
        {
            // nTile, neighbour tile
            GameLogic::Tile* nTile = cheatGameLogic.getTilePtr(*nxy);

            if (nTile == nullptr)
                return;

            if (nTile->state == logic::TileState::Revealed && nTile->number <= mineCount(arrBoard, *nxy) + probabilityHundredCount(*nxy))
            {
                result = false;
                *nxy += 2;
            }
        });

        return result;
    }

    bool CheatsLogic::canNotBeMine(std::vector<ArrangementTile>* arrBoard, core::XY xy)
    {
        bool result = true;

        loopNeighbourTiles(xy, [&](core::XY* nxy) -> void
        {
            // nTile, neighbour tile
            GameLogic::Tile* nTile = cheatGameLogic.getTilePtr(*nxy);

            if (nTile == nullptr)
                return;

            int nEdge = 0;

            // count neighbour tiles that are edge tiles
            loopNeighbourTiles(*nxy, [&](core::XY* nnxy) -> void
            {
                // nChanceTile, neighbour chance tile
                ChanceTile* nChanceTile = getChanceTilePtr(*nnxy);

                if (nChanceTile == nullptr)
                    return;

                if (nChanceTile->edgeTile)
                    nEdge++;
            });

            if (nTile->state == logic::TileState::Revealed && nTile->number >= nEdge - noMineCount(arrBoard, *nxy) - probabilityZeroCount(*nxy))
            {
                result = false;
                *nxy += 2;
            }
        });

        return result;
    }

    int CheatsLogic::mineCount(std::vector<ArrangementTile>* arrBoard, core::XY xy)
    {
        int count = 0;

        for (size_t i = 0; i < arrBoard->size(); i++)
        {
            ArrangementTile* arrTile = &(*arrBoard)[i];

            if (arrTile->mineStatus != IsMine::Yes)
                continue;

            loopNeighbourTiles(xy, [&](core::XY* nxy) -> void
            {
                if (arrTile->xy == *nxy)
                    count++;
            });
        }

        return count;
    }

    int CheatsLogic::noMineCount(std::vector<ArrangementTile>* arrBoard, core::XY xy)
    {
        int count = 0;

        for (size_t i = 0; i < arrBoard->size(); i++)
        {
            ArrangementTile* arrTile = &(*arrBoard)[i];

            if (arrTile->mineStatus != IsMine::No)
                continue;

            loopNeighbourTiles(xy, [&](core::XY* nxy) -> void
            {
                if (arrTile->xy == *nxy)
                    count++;
            });
        }

        return count;
    }

    int CheatsLogic::nonEdgeCount()
    {
        int count = 0;

        loopEachTile([&](core::XY* xy) -> void
        {
            GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);
            ChanceTile* chanceTile = getChanceTilePtr(*xy);

            if (tile->state != logic::TileState::Revealed && chanceTile->edgeTile == false)
                count++;
        });

        return count;
    }

    int CheatsLogic::hundredCount()
    {
        int count = 0;

        loopEachTile([&](core::XY* xy) -> void
        {
            ChanceTile* chanceTile = getChanceTilePtr(*xy);

            if (chanceTile->chance == 100)
                count++;
        });

        return count;
    }

    int CheatsLogic::probabilityHundredCount(core::XY xy)
    {
        int nHundred = 0;

        loopNeighbourTiles(xy, [&](core::XY* nxy) -> void
        {
            // nChanceTile, neighbour chance tile
            ChanceTile* nChanceTile = getChanceTilePtr(*nxy);

            if (nChanceTile == nullptr)
                return;

            if (nChanceTile->chance == 100)
                nHundred++;
        });

        return nHundred;
    }

    int CheatsLogic::probabilityZeroCount(core::XY xy)
    {
        int nZero = 0;

        loopNeighbourTiles(xy, [&](core::XY* nxy) -> void
        {
            // nChanceTile, neighbour chance tile
            ChanceTile* nChanceTile = getChanceTilePtr(*nxy);

            if (nChanceTile == nullptr)
                return;

            if (nChanceTile->chance == 0)
                nZero++;
        });

        return nZero;
    }

    void CheatsLogic::probabilityCalculation()
    {
        uint64_t arrCount = 0;
        int nonEdge = nonEdgeCount();
        int nHundred = hundredCount();

        for (size_t i = 0; i < edgeArr.size(); i++)
        {
            std::vector<ArrangementTile>* arrBoard = &edgeArr[i];

            int minesPlaced = 0;

            for (size_t j = 0; j < arrBoard->size(); j++)
            {
                ArrangementTile* arrTile = &(*arrBoard)[j];

                if (arrTile->mineStatus == IsMine::Yes)
                    minesPlaced++;
            }

            int remainingMines = gameConfig.mines - minesPlaced - nHundred;

            if (remainingMines >= 0 && remainingMines <= nonEdge)
            {
                uint64_t nonEdgeCombinations = combinations(static_cast<uint>(nonEdge), static_cast<uint>(remainingMines));

                for (size_t i = 0; i < arrBoard->size(); i++)
                {
                    ArrangementTile* arrTile = &(*arrBoard)[i];

                    if (arrTile->mineStatus != IsMine::Yes)
                        continue;

                    ChanceTile* chanceTile = getChanceTilePtr(arrTile->xy);

                    chanceTile->mineArr += nonEdgeCombinations;
                }

                arrCount += nonEdgeCombinations;

                loopEachTile([&](core::XY* xy) -> void
                {
                    GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);

                    if (tile->state == logic::TileState::Revealed)
                        return;

                    ChanceTile* chanceTile = getChanceTilePtr(*xy);

                    if (chanceTile->edgeTile == true)
                        return;

                    chanceTile->mineArr += remainingMines / nonEdge * nonEdgeCombinations;
                });
            }
        }

        // prevent divide by zero and other weird behavior if things go wonky
        if (arrCount <= 0)
            return;

        constexpr bool allProbability = true;

        loopEachTile([&](core::XY* xy) -> void
        {
            GameLogic::Tile* tile = cheatGameLogic.getTilePtr(*xy);
            ChanceTile* chanceTile = getChanceTilePtr(*xy);

            if (chanceTile->edgeTile == true && chanceTile->chance == -1)
            {
                int8_t edgeChance;

                edgeChance = std::round((static_cast<double>(chanceTile->mineArr) / arrCount) * 100);

                if (allProbability == false && (edgeChance == 100 || edgeChance == 0))
                    chanceTile->chance = edgeChance;

                if (allProbability)
                    chanceTile->chance = edgeChance;
            }

            if (tile->state != logic::TileState::Revealed && chanceTile->edgeTile == false && chanceTile->chance == -1)
            {
                int8_t nonEdgeChance;

                nonEdgeChance = std::round((static_cast<double>(chanceTile->mineArr) / arrCount) * 100);

                if (allProbability == false && (nonEdgeChance == 100 || nonEdgeChance == 0))
                    chanceTile->chance = nonEdgeChance;

                if (allProbability)
                    chanceTile->chance = nonEdgeChance;
            }
        });
    }

    /*
    uint64_t CheatsLogic::productRange(uint a, uint b)
    {
        uint64_t prd = a;
        uint i = a;

        while (i++ < b)
            prd *= i;

        return prd;
    }

    uint64_t CheatsLogic::combinations(uint n, uint r)
    {
        if (n == r || r == 0)
            return 1;

        if (r < n - r)
            r = n - r;

        return productRange(r + 1, n) / productRange(1, n - r);
    }
    */

    uint64_t CheatsLogic::combinations(uint n, uint r)
    {
        if (r > n)
            return 0;

        if (r > n - r)
            r = n - r;

        uint64_t result = 1;

        for (uint i = 1; i <= r; i++)
        {
            result = result * (n - r + i) / i;
        }

        return result;
    }
}