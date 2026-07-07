#include "input.h"
#include "../game.h"

namespace game::input
{
//public:
    void GameInput::handleInput()
    {
        inputType = readInput();

        switch (inputType)
        {
            case InputType::keyboard:
                handleKb();
                break;
            case InputType::mouse:
                handleMouse();
                break;
            case InputType::both:
                handleKb();
                handleMouse();
                break;
            case InputType::none:
                break;
        }
    }

//private:
    void GameInput::leftClick()
    {
        if (!gameLogic->inBounds(mouseGamePos) || gameLogic->getGameStatus() != enums::GameStatus::Ongoing)
            return;

        gameLogic->revealTile(mouseGamePos);
    }

    void GameInput::middleClick()
    {
    }

    void GameInput::rightClick()
    {
        if (gameLogic->getGameStatus() != enums::GameStatus::Ongoing)
        {
            Game::gameRestart();
            return;
        }

        if (!gameLogic->inBounds(mouseGamePos))
            return;

        gameLogic->placeFlag(mouseGamePos);
    }

    GameInput::InputType GameInput::readInput()
    {
    #ifndef DEBUG
        std::string unfiltered = "";
    #else
        unfiltered = "";
    #endif

        MouseAndKeyboardInput::stdinRead(unfiltered);

        bool gotMouse = MouseAndKeyboardInput::filterMouseInput(unfiltered, &mInput);
        bool gotKb = MouseAndKeyboardInput::filterKbInput(unfiltered, &kbInput);

        if (gotMouse && gotKb)
            return InputType::both;

        if (gotMouse)
            return InputType::mouse;

        if (gotKb)
            return InputType::keyboard;

        return InputType::none;
    }

    void GameInput::handleKb()
    {
        for (const char& c : kbInput)
        {
            if (c == constant::kbExit)
            {
                Game::exitGame();
            }

            if (c == constant::kbResetCamera)
            {
                Game::resetGameOffsets();
            }
        }
    }

    void GameInput::handleMouse()
    {
        char& button = mInput.mouseButton;
        int& state = mInput.mouseState;
        int& mouseState = mInput.mouseState;
        core::XY& mousePos = mInput.mousePos;

        mouseGamePos = calculateMouseGameCoords();

        if (button == 'm')
        {
            switch (state)
            {
                case 0:     // left click
                case 32:    // moving
                    leftClick();
                    return;
                case 1:     // middle click
                case 33:    // moving
                    middleClick();
                    return;
                case 2:     // right click
                case 34:    // moving
                    rightClick();
                    return;
            }
        }

        // move map
        if (mouseState == 1 || mouseState == 16)    // 1 = middle click, 16 = ctrl + left click (for laptop users)
        {
            mouseMovedPos = {0, 0};
        }
        else if (mouseState == 33 || mouseState == 48)  // 33 = middle click and move, 48 ctrl + left click move
        {
            mouseMovedPos = mousePos - wasMousePos;

            Game::addToGameOffsets(mouseMovedPos);
        }

        wasMousePos = mousePos;
    }

    core::XY GameInput::calculateMouseGameCoords()
    {
        core::XY offsets = game::Game::calculateOffset();
        core::XY mousePos = mInput.mousePos;

        core::XY xy
        {
            (mousePos.x - offsets.x) / 2,   // mousePos.x (mouse X position) is divided by two because one monospace character is pretty much half as wide as it is long
            mousePos.y - offsets.y
        };

        return xy;
    }
}