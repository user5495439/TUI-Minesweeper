#pragma once

namespace game::constant
{
    inline constexpr int mines = 99;    // 99 mines for 32x16 map in minesweeper.online
    inline constexpr bool minesInDensity = false;    // if enabled, the mines count will instead be percentage of the map covered in mines
    inline constexpr int gameWidth = 32;
    inline constexpr int gameHeight = 16;
    inline constexpr int borderThickness = 1;
    inline constexpr bool canMoveMap = true;
    inline constexpr bool cheatsEnabled = true;
    inline constexpr bool qMarkEnabled = true;
    inline constexpr bool quickReveal = true;
    inline constexpr bool revealZeroes = true;
    inline constexpr bool placeMisplacedFlags = true;
    inline constexpr bool minesReplaceFlags = false;

    //inline constexpr bool tileNumbersStartWithZero = true;  // tiles are numbered as "05" instead of "55"

    inline constexpr char kbResetCamera = 'f';
    inline constexpr char kbExit = 3;   // 3 = ctrl + c

    inline constexpr char minesText[] = "Mines: ";
    inline constexpr char movesText[] = "Moves: ";
    inline constexpr char winText[] = "You win!";
    inline constexpr char lostText[] = "You lost!";
    inline constexpr char retryText[] = "Right click to try again...";
    inline constexpr char gameOverSeedText[] = "Seed: ";
    inline constexpr char gameOverInfoText1[] = "Game info: width x height = ";
    inline constexpr char gameOverInfoText2[] = " x ";
    inline constexpr char gameOverInfoText3[] = ", mines = ";
}