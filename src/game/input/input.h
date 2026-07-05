#pragma once

#include "../../core/xy.h"
#include "../../core/input/input.h"
#include "../logic/logic.h"
#include <string>

namespace game::input
{
    using namespace core::input;

    class GameInput
    {
    public:
        enum class InputType
        {
            keyboard,
            mouse,
            none,
            both
        };

        GameInput() { MouseAndKeyboardInput::autoInitialize(); };
        GameInput(logic::GameLogic* GameLogic) { gameLogic = GameLogic; };

        void handleInput();
        const MouseAndKeyboardInput::MouseInput* getmInput() { return &mInput; }
        const std::string* getKbInput() { return &kbInput; }
        const InputType* getInputType() { return &inputType; }
        const core::XY* getMouseGamePos() { return &mouseGamePos; }
    #ifdef DEBUG
        const std::string* getUnfiltered() { return &unfiltered; }   // for debugging
    #endif

    private:
        logic::GameLogic* gameLogic = nullptr;
        MouseAndKeyboardInput::MouseInput mInput{};
        std::string kbInput = "";
        InputType inputType = InputType::none;
        core::XY mousePos{};
        core::XY mouseGamePos{};
        core::XY wasMousePos{};
        core::XY mouseMovedPos{};
    #ifdef DEBUG
        std::string unfiltered = "";
    #endif

        void leftClick();
        void middleClick();
        void rightClick();
        InputType readInput();
        void handleKb();
        void handleMouse();
        core::XY calculateMouseGameCoords();
    };
}