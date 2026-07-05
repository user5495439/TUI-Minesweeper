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
        struct GameSettings
        {
        public:
            int width;
            int height;
            int mines;
            int seed;
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
        
        constexpr static Tile borderTile
        {
            enums::BoardTile::Unknown,
            enums::TileState::Border
        };

        constexpr static Tile oobTile
        {
            enums::BoardTile::Unknown,
            enums::TileState::OOB
        };

        static void generateBoard(GameSettings Settings);    // creates the board according to sizes in GameSettings and places mines accoring to the seed
        static enums::RevealResult revealTile(core::XY xy);
        static enums::RevealResult quickRevealTiles(core::XY xy); // quick reveal system so it's less annoying to play to some
        static enums::RevealResult revealZeroTiles(core::XY xy);  // this never returns RevealResult::Safe
        static void revealMines();
        static void placeFlagsOnMines();
        static int placeFlag(core::XY xy);
        static bool inBounds(core::XY xy);  // returns true if coordinate is in game bounds and false if not
        static Tile getTile(core::XY xy);
        static const GameSettings& getSettings() { return settings; }
        static core::XY getGameDimensions() { return {settings.width, settings.height}; }
        static core::XY getVectorDimensions() { return vectorSizes; }
    private:
        static inline std::vector<Tile> board{constant::gameWidth * constant::gameHeight};
        static inline GameSettings settings{};
        static inline std::mt19937 rng{};
        static inline core::XY boardStartPos{};
        static inline core::XY boardEndPos{};
        static inline core::XY vectorSizes{};
        static inline int revealedTileCounter = 0;

        static int getBoardIndex(core::XY xy);
        static void generateMines();
        static void generateNeighbours();
        static void generateBorder();
    };
}