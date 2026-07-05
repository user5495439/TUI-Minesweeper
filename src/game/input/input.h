#pragma once

#include "../../core/xy.h"
#include <string>
#include "../../core/input/input.h"

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

        static void inputInit();
        static void handleInput();
        static const MouseAndKeyboardInput::MouseInput* getmInput() { return &mInput; }
        static const std::string* getKbInput() { return &kbInput; }
        static const InputType* getInputType() { return &inputType; }
        static const core::XY* getMouseGamePos() { return &mouseGamePos; }
    #ifdef DEBUG
        static const std::string* getUnfiltered() { return &unfiltered; }   // for debugging
    #endif

    private:
        static inline MouseAndKeyboardInput::MouseInput mInput{};
        static inline std::string kbInput = "";
        static inline InputType inputType = InputType::none;
        static inline core::XY mousePos{};
        static inline core::XY mouseGamePos{};
        static inline core::XY wasMousePos{};
        static inline core::XY mouseMovedPos{};
    #ifdef DEBUG
        static inline std::string unfiltered = "";
    #endif

        static void leftClick();
        static void middleClick();
        static void rightClick();
        static InputType readInput();
        static void handleKb();
        static void handleMouse();
        static core::XY calculateMouseGameCoords();
    };
}