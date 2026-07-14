#pragma once

#include "../core/xy.h"
#include "input/input.h"
#include "logic/logic.h"
#include "render/renderGame.h"
#include "cheat/logic/cheat.h"
#include "cheat/render/renderCheats.h"
#include <csignal>

namespace game
{
    class Game
    {
    public:
        Game();

        static void gameLoop();
        static core::XY calculateOffset();
        static logic::GameLogic* getGameLogic() { return &gameLogic; };
        static void resetGameOffsets() { gameOffsets = {0, 0}; _gameOffsets = {0, 0}; }
        static void addToGameOffsets(core::XY xy) { _gameOffsets += xy; gameOffsets = _gameOffsets; gameOffsets.x |= 1; }    // _offsetX & ~1 in C# version
        static void cheat() { if (constant::cheatsEnabled) cheatsLogic.cheat(); }
        static void cheatReset() { if (constant::cheatsEnabled) cheatsLogic.reset(); }
        static bool* getShowCheatUIPtr() { return &showCheatUI; }
        static void gameRestart() { gameLogic = logic::GameLogic(defaultGameConfig()); cheatsLogic = cheat::logic::CheatsLogic(&gameLogic); };
        static void resizeGame(int) { gotResize = 1; }
        static void exitGame() { gotExit = true; }
    #ifdef DEBUG
        static std::string* getDebugTextPtr() { return &debugText; };
    #endif

    private:
        static inline logic::GameLogic gameLogic;
        static inline render::GameRender gameRender;
        static inline input::GameInput gameInput;
        static inline cheat::logic::CheatsLogic cheatsLogic;
        static inline cheat::render::CheatsRender cheatsRender;
        static inline core::XY gameOffsets{};
        static inline core::XY _gameOffsets{};
        static inline int gameMoves = 0;
        static inline int remainingMines = 0;
        static inline bool showCheatUI = false;
        static volatile inline sig_atomic_t gotResize = 0;
        static inline bool gotExit = false;
    #ifdef DEBUG
        inline static std::string debugText = "";
    #endif

        static void update();
        static logic::GameLogic::GameConfig defaultGameConfig();
    };
}