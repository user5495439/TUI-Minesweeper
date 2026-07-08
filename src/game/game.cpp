#include "game.h"
#include "constant/constants.h"
#include "input/input.h"
#include "logic/logic.h"
#include "render/renderGame.h"
#include <random>

namespace game
{
    using namespace core::render;

//public:
    Game::Game()
    {
        gameLogic = logic::GameLogic(defaultGameConfig());
        gameRender = render::GameRender(&gameLogic);
        gameInput = input::GameInput(&gameLogic);
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

//private:
    void Game::update()
    {
        core::XY boardOffsets = calculateOffset();

        gameRender.drawTiles(boardOffsets);
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
        int seed = rd();
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