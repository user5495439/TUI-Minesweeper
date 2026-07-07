#pragma once

#include "../../core/xy.h"
#include "../enums/BoardTile.h"
#include "../enums/GameStatus.h"
#include "../enums/RevealResult.h"
#include "../enums/TileState.h"
#include "../constant/constants.h"
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
        };

        // in this C++ version i'll just use a struct instead of two separate arrays because it will be cleaner this way i think
        struct Tile
        {
            int number;    // enums::BoardTile
            enums::TileState state;
        };

        GameLogic() {};
        GameLogic(GameConfig Config);
        
        static constexpr Tile borderTile
        {
            enums::BoardTile::Unknown,
            enums::TileState::Border
        };

        static constexpr Tile oobTile
        {
            enums::BoardTile::Unknown,
            enums::TileState::OOB
        };

        void revealTile(core::XY xy);
        void placeFlag(core::XY xy);
        bool inBounds(core::XY xy);  // returns true if coordinate is in game bounds and false if not
        Tile getTile(core::XY xy) { return inBounds(xy) ? board[getBoardIndex(xy)] : oobTile; };
        Tile* getTilePtr(core::XY xy) { return inBounds(xy) ? &board[getBoardIndex(xy)] : nullptr; };
        const GameConfig& getConfig() { return config; }
        core::XY getVectorDimensions() { return vectorSizes; }
        enums::GameStatus getGameStatus() { return gameStatus; }
        int getMoves() { return gameMoves; }
        int getRemainingMines() { return remainingMines; }
    private:
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

        enums::RevealResult tileReveal(core::XY xy);
        enums::RevealResult massTileReveal(core::XY xy, bool skipFlagsCheck); // quick reveal system so it's less annoying to play to some

        void placeFlagsOnMines();
        void revealMines();

        void generateMines();
        void generateNeighbours();
        void generateBorder();
        void gameOver(enums::GameStatus status);
        int getBoardIndex(core::XY xy) { return xy.y * vectorSizes.x + xy.x; };
    };
}