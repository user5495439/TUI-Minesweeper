#pragma once

#include "../../core/xy.h"
#include "enums/BoardTile.h"
#include "enums/RevealResult.h"
#include "enums/TileState.h"
#include "../enums/PublicTile.h"
#include "../enums/GameStatus.h"
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

            // allows setTile() to work
            bool editBoard;
        };

        GameLogic() {};
        GameLogic(GameConfig Config);

        void revealTile(core::XY xy);
        void placeFlag(core::XY xy);
        bool inBounds(core::XY xy);  // returns true if coordinate is in game bounds and false if not
        enums::PublicTile getTile(core::XY xy) { return tileToPublicTile(getTilePtr(xy)); };
        const GameConfig& getConfig() { return config; }
        core::XY getVectorDimensions() { return vectorSizes; }
        enums::GameStatus getGameStatus() { return gameStatus; }
        int getMoves() { return gameMoves; }
        int getRemainingMines() { return remainingMines; }

        void setTile(core::XY xy, int number, TileState state);
    private:

        // in this C++ version i'll just use a struct instead of two separate arrays because it will be cleaner this way i think
        struct Tile
        {
            int number;    // enums::BoardTile
            TileState state;
        };

        static constexpr Tile borderTile
        {
            BoardTile::Unknown,
            TileState::Border
        };

        static constexpr Tile oobTile
        {
            BoardTile::Unknown,
            TileState::OOB
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

        Tile* getTilePtr(core::XY xy) { return inBounds(xy) ? &board[getBoardIndex(xy)] : nullptr; };
        enums::PublicTile tileToPublicTile(const Tile* tile);
        Tile publicTileToTile(const enums::PublicTile pTile);
        RevealResult tileReveal(core::XY xy);
        RevealResult massTileReveal(core::XY xy, bool skipFlagsCheck); // quick reveal system so it's less annoying to play to some
        void placeFlagsOnMines();
        void revealMines();
        void generateMines();
        void generateNeighbours();
        void generateBorder();
        void gameOver(enums::GameStatus status);
        int getBoardIndex(core::XY xy) { return xy.y * vectorSizes.x + xy.x; };
    };
}