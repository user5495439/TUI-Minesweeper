#pragma once

// so the cheats in this C++ version are more advanced and work with probability
// currently very messy and needs refactoring and clean up
// has a couple of bugs and makes the program crash sometimes

#include "../../logic/logic.h"
#include <cstdint>

namespace game::cheat::logic
{
    using namespace ::game::logic;

    class CheatsLogic
    {
    public:
        CheatsLogic() {};
        CheatsLogic(GameLogic* GameLogic);

        struct ChanceTile
        {
            int8_t chance = -1;
            bool edgeTile;
            uint64_t mineArr;
        };

        void cheat();
        void reset();
        core::XY getBoardStartPos() { return boardStartPos; };
        core::XY getSizes() { return sizes; };
        ChanceTile* getChanceTilePtr(core::XY xy) { return inBounds(xy) ? &chancesBoard[getBoardIndex(xy)] : nullptr; }
    private:
        enum class IsMine
        {
            Unknown,
            No,
            Yes
        };

        struct ArrangementTile
        {
            core::XY xy;
            IsMine mineStatus;
        };

        GameLogic::GameConfig gameConfig;
        std::vector<ChanceTile> chancesBoard;
        std::vector<std::vector<ArrangementTile>> edgeArr;
        GameLogic* gameLogic;
        GameLogic cheatGameLogic;
        core::XY boardStartPos;
        core::XY boardEndPos;
        core::XY sizes;

        void findEdgeTiles();
        void copyBoard(GameLogic* sourceBoard);
        bool simpleHundredChanceSolver();
        bool simpleZeroChanceSolver();
        void isolatedEdgeCellsSolver();
        core::XY getNextEdgeTile(core::XY Xy);
        void probabilityStuff();
        void generateArrangements(std::vector<ArrangementTile>* arrBoard, int index);
        bool canBeMine(std::vector<ArrangementTile>* arrBoard, core::XY xy);
        bool canNotBeMine(std::vector<ArrangementTile>* arrBoard, core::XY xy);
        int mineCount(std::vector<ArrangementTile>* arrBoard, core::XY xy);
        int noMineCount(std::vector<ArrangementTile>* arrBoard, core::XY xy);
        int nonEdgeCount();
        int hundredCount();
        int probabilityHundredCount(core::XY xy);
        int probabilityZeroCount(core::XY xy);
        void probabilityCalculation();
        uint64_t combinations(uint n, uint r);
        bool inBounds(core::XY xy) { return xy.x >= 0 && xy.x < sizes.x && xy.y >= 0 && xy.y < sizes.y; }
        int getBoardIndex(core::XY xy) { return xy.y * sizes.x + xy.x; };

        template<typename Func>
        void loopEachTile(Func function)
        {
            for (core::XY xy = {0, 0}; xy.y < sizes.y; xy.y++)
                for (xy.x = 0; xy.x < sizes.x; xy.x++)
                    function(&xy);
        }

        template<typename Func>
        void loopNeighbourTiles(core::XY xy, Func function)
        {
            core::XY xyStart = xy + core::XY{ -1, -1 };
            core::XY xyEnd = xy + core::XY{ 1, 1 };

            for (core::XY nxy = xyStart; nxy.y <= xyEnd.y; nxy.y++)
                for (nxy.x = xyStart.x; nxy.x <= xyEnd.x; nxy.x++)
                {
                    if (nxy == xy)
                        continue;

                    function(&nxy);
                }
        }
    };
}