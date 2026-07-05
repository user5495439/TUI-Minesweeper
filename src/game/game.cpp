#include "game.h"
#include "constant/constants.h"
#include "input/input.h"
#include "render/renderGame.h"
#include <random>

namespace game
{
    using namespace core::render;

//public:
    void Game::gameStart()
    {
        gameInit();
        input::GameInput::inputInit();
        update();
        gameLoop();
    }

    void Game::gameInit()
    {
        using namespace constant;

        std::random_device rd;
        int seed = rd();
        int mines = minesInDensity ? (gameWidth * gameHeight) * (constant::mines / 100.0) : constant::mines;

        logic::GameLogic::GameSettings settings
        {
            gameWidth,
            gameHeight,
            mines,
            seed,
            qMarkEnabled,
            quickReveal,
            revealZeroes,
            placeMisplacedFlags,
            minesReplaceFlags
        };

        logic::GameLogic::generateBoard(settings);

        resetRemainingMines();
        resetMoves();
    }

    void Game::update()
    {
        core::XY boardOffsets = calculateOffset();

        render::GameRender::drawTiles(boardOffsets);
        render::GameRender::drawUI(boardOffsets);
    #ifdef DEBUG
        render::GameRender::drawDebug();
    #endif

        Renderer::bufferWrite();
    }

    core::XY Game::calculateOffset()
    {
        core::XY windowSizes = Renderer::getBufferDimensions();
        core::XY gameSizes = logic::GameLogic::getGameDimensions();

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
    void Game::gameLoop()
    {
        while (!gotExit)
        {
            input::GameInput::handleInput();

            if (gotResize)
            {
                Renderer::bufferResize();

                Renderer::bufferMarkAllDirty();

                gotResize = 0;
            }

            update();
        }
    }
}