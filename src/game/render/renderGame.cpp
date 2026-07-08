#include "renderGame.h"
#include "../logic/logic.h"
#include "../game.h"
#include <algorithm>
#include <string>

namespace game::render
{
//public:
    void GameRender::drawTiles(core::XY offsets)
    {
        const core::XY sizes = gameLogic->getVectorDimensions();
        const Sides cuts = getCuts(sizes, offsets);

        for (core::XY xy { cuts.left, cuts.top }; xy.y < sizes.y - cuts.bottom; xy.y++)
        {
            core::XY conXY
            {
                offsets.x + cuts.left * 2,
                xy.y + offsets.y
            };

            Renderer::setCursorPos(conXY);

            for (xy.x = cuts.left; xy.x < sizes.x - cuts.right; xy.x++)
            {
                enums::PublicTile tile = gameLogic->getTile(xy);

                TileAppearance tileAppearance = tileAppearances[static_cast<int>(tile)];

                Renderer::setPaletteColor(tileAppearance.fColor, tileAppearance.bColor);
                Renderer::Write(tileAppearance.icon);
            }
        }

        Renderer::resetColor();
    }

    void GameRender::drawUI(core::XY offsets)
    {
        const logic::GameLogic::GameConfig gameConfig = gameLogic->getConfig();
        const enums::GameStatus gameStatus = gameLogic->getGameStatus();
        const int remainingMines = gameLogic->getRemainingMines();
        const int totalMines = gameConfig.mines;
        const int moves = gameLogic->getMoves();

        const core::XY boardSizes = gameLogic->getVectorDimensions();
        const core::XY windowSizes = Renderer::getBufferDimensions();

        std::string minesString = formatMineswNumber(remainingMines, totalMines);
        std::string movesString = formatMineswNumber(moves, moves);
        std::string minesFixedPosString = constant::minesText + minesString;
        std::string movesFixedPosString = constant::movesText + movesString;

        core::XY minesXY
        {
            offsets.x,
            offsets.y - 1
        };
        core::XY movesXY
        {
            offsets.x + boardSizes.x * 2,
            offsets.y - 1
        };
        core::XY minesFixedXY
        {
            Renderer::leftOffset,
            Renderer::topOffset
        };
        core::XY movesFixedXY
        {
            windowSizes.x + Renderer::leftOffset,
            Renderer::topOffset
        };

        Renderer::setCursorPos(minesXY);
        size_t charsWritten = Renderer::WriteOOB(minesString);

        if (charsWritten != minesString.size())
        {
            Renderer::setCursorPos(minesFixedXY);
            Renderer::Write(minesFixedPosString);
        }
        
        Renderer::setCursorPos(movesXY);
        charsWritten = Renderer::WriteFormatted(movesString, Renderer::TextAlignment::RightToLeft);

        if (charsWritten != movesString.size())
        {
            Renderer::setCursorPos(movesFixedXY);
            Renderer::WriteFormatted(movesFixedPosString, Renderer::TextAlignment::RightToLeft);
        }

        if (gameStatus != enums::GameStatus::Ongoing)
        {
            std::string infoString = constant::gameOverInfoText1 + std::to_string(gameConfig.sizes.x) + constant::gameOverInfoText2 + std::to_string(gameConfig.sizes.y) + constant::gameOverInfoText3 + std::to_string(gameConfig.mines);
            std::string seedString = constant::gameOverSeedText + std::to_string(gameConfig.seed);
            std::string gameOverString = gameStatus == enums::GameStatus::Won ? constant::winText : constant::lostText;
            std::string retryString = constant::retryText;

            core::XY infoXY
            {
                Renderer::leftOffset,
                windowSizes.y
            };
            core::XY seedXY
            {
                infoXY.x,
                infoXY.y - 1
            };
            core::XY gameOverXY
            {
                offsets.x + boardSizes.x,
                offsets.y - 1
            };
            core::XY retryXY
            {
                offsets.x + boardSizes.x,
                offsets.y + boardSizes.y
            };
            core::XY gameOverFixedXY
            {
                Renderer::leftOffset + windowSizes.x / 2,
                Renderer::topOffset
            };
            core::XY retryFixedXY
            {
                windowSizes.x + Renderer::leftOffset,
                windowSizes.y
            };

            Renderer::setCursorPos(infoXY);
            Renderer::Write(infoString);

            Renderer::setCursorPos(seedXY);
            Renderer::Write(seedString);

            Renderer::setCursorPos(gameOverXY);
            charsWritten = Renderer::WriteFormatted(gameOverString, Renderer::TextAlignment::Centered);
            if (charsWritten != gameOverString.size())
            {
                Renderer::setCursorPos(gameOverFixedXY);
                Renderer::WriteFormatted(gameOverString, Renderer::TextAlignment::Centered);
            }

            Renderer::setCursorPos(retryXY);
            charsWritten = Renderer::WriteFormatted(retryString, Renderer::TextAlignment::Centered);
            if (charsWritten != retryString.size())
            {
                Renderer::setCursorPos(retryFixedXY);
                Renderer::WriteFormatted(retryString, Renderer::TextAlignment::RightToLeft);
            }
        }
    }

#ifdef DEBUG
    void GameRender::drawDebug()
    {
        const core::XY windowSizes = Renderer::getBufferDimensions();

        core::XY xy
        {
            windowSizes.x / 2,
            windowSizes.y
        };

        Renderer::setCursorPos(xy);
        Renderer::WriteFormatted(*Game::getDebugTextPtr(), Renderer::TextAlignment::Centered);
    }
#endif

//private:
    GameRender::Sides GameRender::getCuts(core::XY sizes, core::XY offsets)
    {
        core::XY windowSizes = Renderer::getBufferDimensions();

        windowSizes.x |= 1;     // in original C# version: Program.windowWidth & ~1;

        return
        {
            offsets.y < 1 ? (-offsets.y + 1) : 0,
            offsets.y + sizes.y > windowSizes.y ? offsets.y + sizes.y - windowSizes.y - 1 : 0,
            offsets.x < 1 ? (-offsets.x + 1) / 2 : 0,
            offsets.x + sizes.x * 2 > windowSizes.x ? (offsets.x + sizes.x * 2 - windowSizes.x) / 2 : 0
        };
    }

    // after adding that fix to prevent that crash i feel like this function could be cleaned up
    std::string GameRender::formatMineswNumber(int input, size_t max)
    {
        max = std::to_string(max).size();

        max += max & 1; // add the least significant bit to max (rounds up to even number)

        // this is here to prevent crash / undefined behavior if remaining mines go too far to negatives or whatever idk
        size_t inputL = std::to_string(input).size();

        inputL += inputL & 1;

        std::string text = std::to_string(input);

        size_t zeroes = std::max(max, inputL) - text.size();

        text = std::string(zeroes, '0') + text;

        return text;
    }
}