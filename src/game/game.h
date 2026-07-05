#pragma once

#include "../core/xy.h"
#include "input/input.h"
#include "logic/logic.h"
#include "render/renderGame.h"
#include <csignal>

namespace game
{
    class Game
    {
    public:
        enum class GameStatus
        {
            Ongoing,
            Won,
            Lost
        };

        Game();

        static void gameRestart();
        static logic::GameLogic::GameConfig getGameConfig();
        static void update();
        static void gameLoop();
        static core::XY calculateOffset();
        static logic::GameLogic* getGameLogic() { return &gameLogic; };
        static void resetGameOffsets() { gameOffsets = {0, 0}; _gameOffsets = {0, 0}; }
        static GameStatus getGameStatus() { return gameStatus; }
        static void setGameStatus(GameStatus value) { gameStatus = value; }
        static void addMove() { gameMoves++; }
        static void resetMoves() { gameMoves = 0; }
        static int getMoves() { return gameMoves; }
        static void addToRemainingMines(int value) { remainingMines += value; }
        static void resetRemainingMines() { remainingMines = gameLogic.getConfig().mines; }
        static int getRemainingMines() { return remainingMines; }
        static void addToGameOffsets(core::XY xy) { _gameOffsets += xy; gameOffsets = _gameOffsets; gameOffsets.x |= 1; }    // _offsetX & ~1 in C# version
        static void resizeGame(int) { gotResize = 1; }
        static void exitGame() { gotExit = true; }
    #ifdef DEBUG
        static std::string* getDebugTextPtr() { return &debugText; };
    #endif

    private:
        static inline logic::GameLogic gameLogic;
        static inline render::GameRender gameRender;
        static inline input::GameInput gameInput;
        static inline core::XY gameOffsets{};
        static inline core::XY _gameOffsets{};
        static inline GameStatus gameStatus{};
        static inline int gameMoves = 0;
        static inline int remainingMines = 0;
        static volatile inline sig_atomic_t gotResize = 0;
        static inline bool gotExit = false;
    #ifdef DEBUG
        inline static std::string debugText = "";
    #endif
    };
}