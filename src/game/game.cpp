#include "game.h"
#include "constant/constants.h"
#include "cheat/render/renderCheats.h"
#include "cheat/logic/cheat.h"
#include "enums/PublicTile.h"
#include "input/input.h"
#include "logic/logic.h"
#include "render/renderGame.h"
#include <cstddef>
#include <random>
#include <string>

namespace game
{
    using namespace core::render;

//public:
    Game::Game()
    {
        gameLogic = logic::GameLogic(defaultGameConfig());
        gameRender = render::GameRender(&gameLogic);
        gameInput = input::GameInput(&gameLogic);
        cheatsLogic = cheat::logic::CheatsLogic(&gameLogic);
        cheatsRender = cheat::render::CheatsRender(&cheatsLogic);
    }

    void Game::gameLoop()
    {
        update();

        while (!gotExit)
        {
            gameInput.handleInput();

            if (gotResize)
            {
                Renderer::bufferResize();
                Renderer::bufferMarkAllDirty();

                gotResize = 0;
            }

            update();
        
        #ifdef DEBUG
            debugText = "";
        #endif
        }
    }

    core::XY Game::calculateOffset()
    {
        core::XY windowSizes = Renderer::getBufferDimensions();
        core::XY gameSizes = gameLogic.getConfig().sizes;

        core::XY centerPos
        {
            windowSizes.x / 2 + gameOffsets.x,
            windowSizes.y / 2 + gameOffsets.y
        };

        centerPos.x |= 1;   // force the least significant bit to 1 (makes the number odd), 0100 (decimal 4) becomes 0101 (decimal 5) 

        return
        {
            centerPos.x - gameSizes.x,      // gameSizes.x (width of the game) is not divided by two because one monospace character is pretty much half as wide as it is long
            centerPos.y - gameSizes.y / 2
        };
    }

#ifdef DEBUG
    void Game::tileDebugInfo(core::XY xy)
    {
        std::string* debugText = Game::getDebugTextPtr();
        logic::GameLogic* localGameLogic;

        if (showCheatUI)
        {
            localGameLogic = cheatsLogic.getDebugCheatGameLogicPtr();
            xy -= gameLogic.getConfig().borderThickness; // cheat gamelogic doesn't have borders
        }
        else
        {
            localGameLogic = &gameLogic;
        }

        logic::GameLogic::FullTileDataDebug tileData = localGameLogic->getFullTileDataDebug(xy);

        logic::GameLogic::Tile* tile = tileData.tilePtr;
        enums::PublicTile pTile = tileData.pTile;

        *debugText = *debugText + "x, y: " + std::to_string(tileData.xy.x) + ", " + std::to_string(tileData.xy.y) + ", pTile: " + std::to_string(static_cast<int>(pTile));

        if (tile == nullptr)
            return;

        *debugText = *debugText + ", number: " + std::to_string(tile->number) + ", state: " + std::to_string(static_cast<int>(tile->state)) + ", mine: " + std::to_string(static_cast<int>(tile->mine));

        if (!showCheatUI)
            return;

        cheat::logic::CheatsLogic::ChanceTile* chanceTile = cheatsLogic.getChanceTilePtr(xy);

        if (chanceTile == nullptr)
            return;

        *debugText = *debugText + ", chance: " + std::to_string(chanceTile->chance) + ", edgeTile: " + std::to_string(chanceTile->edgeTile) + ", mineArr: " + std::to_string(chanceTile->mineArr) + ", chanceIsTrue, chanceFrom: " + std::to_string(chanceTile->chanceIsTrue) + ", " + std::to_string(chanceTile->chanceFrom);
    }
#endif

//private:
    void Game::update()
    {
        core::XY boardOffsets = calculateOffset();

        gameRender.drawTiles(boardOffsets);

        if (constant::cheatsEnabled && showCheatUI)
            cheatsRender.drawCheatsUI(boardOffsets);

        gameRender.drawUI(boardOffsets);
    #ifdef DEBUG
        gameRender.drawDebug();
    #endif

        Renderer::bufferWrite();
    }

    logic::GameLogic::GameConfig Game::defaultGameConfig()
    {
        using namespace constant;

        std::random_device rd;
        int seed = useFixedSeed ? fixedSeed : rd();
        int mines = minesInDensity ? (gameWidth * gameHeight) * (constant::mines / 100.0) : constant::mines;

        logic::GameLogic::GameConfig gameConfig
        {
            gameWidth,
            gameHeight,
            mines,
            seed,
            borderThickness,
            qMarkEnabled,
            quickReveal,
            revealZeroes,
            placeMisplacedFlags,
            minesReplaceFlags,
            false
        };

        return gameConfig;
    }
}