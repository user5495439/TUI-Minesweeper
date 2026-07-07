# TUI-Minesweeper
Copyright (c) 2026 Anttoni (user5495439)

Licensed under the GPL-3.0-or-later. See LICENSE for details.

A text-based user interface Minesweeper for linux terminal emulators that uses no third party libraries

## Description
minesweeper game with mouse support for terminal emulators that uses no third party libraries and aims to be somewhat polished and customizable

currently linux only

only a little bit of AI used, i have used AI only to learn C++ and for stuff i haven't been able to figure out how to implement

## Features
- mouse input
- has like a "quick tile reveal" system similar minesweeper.online where if you click on a revealed non zero tile it will reveal the tiles next to it if the amount of flags matches the tile's number
- you can move the camera / map
- big levels (1024x1024 and bigger) work and are playable. currently if you are planning on playing a big level, set minesInDensity to true and keep mines above 10 otherwise the game can crash
- you have a couple of settings to tweak to customize the behavior of the game in the source code
- the game ragebaits you by giving you a chance to hit a mine on the first click

## Problems
- you have to recompile to change settings
- the code is somewhat wannabe yandere simulator code
- you can't play the game purely with a keyboard as of right now
- no timer

## Recommended terminals
from what I have tested, this game runs the best in **Kitty**, **XTerm** and **Gnome Terminal**. what i mean with the best is good performance and no terminal specific weird behavior.

terminals where the game doesn't run optimally: **Konsole** (stuttering, performance), **Cool Retro Term** (segmentation fault when zooming out), **QTerminal** (weird mouse behavior)

## Info and default controls
- the default map configuration is 32 x 16 with 99 mines
- top left side number tells the amount of mines
- top right side number tells the amount of moves
- ctrl+c for exit
- f to reset camera
- left click on a tile to reveal it
- right click to place a flag or question mark
- middle click for moving the camera
- click on a revealed tile to reveal tiles next to it if the correct amount of flags are neighbouring it

## Changing settings
currently you can change settings by editing a file at src/game/constant/constant.h, after that compile the game (read compiling and running)

## Feature ideas
- reimplement cheats and make them even better. cheats existed in the old C# version
- add a feature where you can't hit a mine when clicking on a tile when starting the game, instead you will always reveal a zero tile
- add config stuff and maybe an ingame settings screen so you don't have to recompile this project to change settings
- maybe add like fancy stuff with mouse, like tiles get highlighted when hovering over them or tiles the mouse is hovering over get pressed down when pressing and holding on tiles
- (related to config stuff) maybe add mouse interactable GUI, like a restart button
- clean up and optimize the code even more
- saving system? infinite minesweeper levels?

- no gambling setting? what i mean by this is a setting to disable cases where you have to guess which tile is a mine

## Compiling and running
install g++ (or gcc depending on your distro) and make. if you do not know how to install these two, ask chatgpt for help

then open up a terminal emulator and type:
```
git clone https://github.com/user5495439/TUI-Minesweeper.git
cd TUI-Minesweeper
make
build/minesweeper
```

you can copy the binary (```build/minesweeper```) to somewhere else if you want, it doesn't need any dependencies

## Running on windows
you can maybe get the game to work in wsl but i haven't tried that and it might work poorly there