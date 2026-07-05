#pragma once

#include "../core/xy.h"
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

        static void gameStart();
        static void gameInit();
        static void update();
        static core::XY calculateOffset();
        static void resetGameOffsets() { gameOffsets = {0, 0}; _gameOffsets = {0, 0}; }
        static GameStatus getGameStatus() { return gameStatus; }
        static void setGameStatus(GameStatus value) { gameStatus = value; }
        static void addMove() { gameMoves++; }
        static void resetMoves() { gameMoves = 0; }
        static int getMoves() { return gameMoves; }
        static void addToRemainingMines(int value) { remainingMines += value; }
        static void resetRemainingMines() { remainingMines = logic::GameLogic::getSettings().mines; }
        static int getRemainingMines() { return remainingMines; }
        static void addToGameOffsets(core::XY xy) { _gameOffsets += xy; gameOffsets = _gameOffsets; gameOffsets.x |= 1; }    // _offsetX & ~1 in C# version
        static void resizeGame(int) { gotResize = 1; }
        static void exitGame() { gotExit = true; }

        static std::string* getDebugTextPtr() { return &debugText; };

    private:
        static inline core::XY gameOffsets{};
        static inline core::XY _gameOffsets{};
        static inline GameStatus gameStatus{};
        static inline int gameMoves = 0;
        static inline int remainingMines = 0;
        static volatile inline sig_atomic_t gotResize = 0;
        static inline bool gotExit = false;

        inline static std::string debugText = "";

        static void gameLoop();
    };
}