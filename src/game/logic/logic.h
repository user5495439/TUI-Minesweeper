#pragma once

#include "../../core/xy.h"
#include "enums/MineState.h"
#include "enums/RevealResult.h"
#include "enums/TileState.h"
#include "../enums/PublicTile.h"
#include "../enums/GameStatus.h"
#include "../constant/constants.h"
#include <functional>
#include <random>
#include <vector>

namespace game::logic
{
    class GameLogic
    {
    public:
        struct GameConfig
        {
        public:
            core::XY sizes;
            int mines;
            int seed;
            int borderThickness;
            bool qMarkEnabled;
            bool quickReveal;
            bool revealZeroes;

            // settings for revealMines()
            bool placeMisplacedFlags;
            bool minesReplaceFlags;

            // allows getTilePtr() to return a pointer
            bool editBoard;
        };

        // in this C++ version i'll just use a struct instead of two separate arrays because it will be cleaner this way i think
        struct Tile
        {
            int number;
            TileState state;
            MineState mine;
        };

    #ifdef DEBUG
        struct FullTileDataDebug
        {
            Tile* tilePtr;
            enums::PublicTile pTile;
            core::XY xy;
        };
    #endif

        static constexpr int invalidNumber = -1;

        GameLogic() {};
        GameLogic(GameConfig Config);

        void revealTile(core::XY xy);
        void placeFlag(core::XY xy);
        bool inBounds(core::XY xy);  // returns true if coordinate is in game bounds and false if not
        static enums::PublicTile tileToPublicTile(const Tile* tile);
        static Tile publicTileToTile(const enums::PublicTile pTile);
        enums::PublicTile getPublicTile(core::XY xy) { return tileToPublicTile(tilePtr(xy)); };
        const GameConfig& getConfig() { return config; }
        core::XY getVectorDimensions() { return vectorSizes; }
        enums::GameStatus getGameStatus() { return gameStatus; }
        int getMoves() { return gameMoves; }
        int getRemainingMines() { return remainingMines; }

        Tile* getTilePtr(core::XY xy) { return config.editBoard ? tilePtr(xy) : nullptr; };

    #ifdef DEBUG
        FullTileDataDebug getFullTileDataDebug(core::XY xy) { return { tilePtr(xy), getPublicTile(xy), xy }; };
    #endif
    private:

        static constexpr Tile borderTile
        {
            invalidNumber,
            TileState::Border,
            MineState::Unknown
        };

        static constexpr Tile oobTile
        {
            invalidNumber,
            TileState::OOB,
            MineState::Unknown
        };

        enums::GameStatus gameStatus;
        std::vector<Tile> board;
        GameConfig config;
        std::mt19937 rng{};
        core::XY boardStartPos;
        core::XY boardEndPos;
        core::XY vectorSizes;
        int revealedTileCounter;
        int gameMoves;
        int remainingMines;
    #ifdef DEBUG
        std::function<double()> rngDebug;
    #endif

        Tile* tilePtr(core::XY xy) { return inBounds(xy) ? &board[getBoardIndex(xy)] : nullptr; };
        RevealResult tileReveal(core::XY xy);
        RevealResult massTileReveal(core::XY xy, bool skipFlagsCheck); // quick reveal system so it's less annoying to play to some
        void placeFlagsOnMines();
        void revealMines();
        void generateMines();
        void generateNeighbours();
        void generateBorder();
        void gameOver(enums::GameStatus status);
        int getBoardIndex(core::XY xy) { return xy.y * vectorSizes.x + xy.x; };
        int randomProvider(int a, int b);
        void randomProviderSeed(int seed);
    #ifdef DEBUG
        std::function<double()> mulberry32(uint32_t seed);
    #endif
    };
}