#pragma once

#include "../../core/xy.h"
#include "../enums/BoardTile.h"
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

        enums::RevealResult revealTile(core::XY xy);
        enums::RevealResult quickRevealTiles(core::XY xy); // quick reveal system so it's less annoying to play to some
        enums::RevealResult revealZeroTiles(core::XY xy);  // this never returns RevealResult::Safe
        void revealMines();
        void placeFlagsOnMines();
        int placeFlag(core::XY xy);
        bool inBounds(core::XY xy);  // returns true if coordinate is in game bounds and false if not
        Tile getTile(core::XY xy);
        const GameConfig& getConfig() { return config; }
        core::XY getGameDimensions() { return config.sizes; }
        core::XY getVectorDimensions() { return vectorSizes; }
    private:
        std::vector<Tile> board;
        GameConfig config;
        std::mt19937 rng{};
        core::XY boardStartPos;
        core::XY boardEndPos;
        core::XY vectorSizes;
        int revealedTileCounter;

        int getBoardIndex(core::XY xy);
        void generateMines();
        void generateNeighbours();
        void generateBorder();
    };
}