# TUI-Minesweeper
A TUI minesweeper game made with nice features

description:
- a minesweeper game with mouse support for terminal emulators that uses no third party libraries and aims to be somewhat polished and customizable
- linux only program

features:
- mouse input
- has like a "quick tile reveal" system similar minesweeper.online where if you click on a revealed non zero tile it will reveal the tiles next to it if the amount of flags matches the tile's number
- you can move the camera / map
- big levels (1024x1024 and bigger) actually work and are playable, although a bit laggy if zoomed out much
- lots of settings to tweak to customize the behavior of the game in the source code
- legit cheats (unless disabled). what i mean by legit cheats is that the cheats can only see what you can see, so the cheats can't just magically know where mines are
- the game ragebaits you by giving you a chance to hit a mine on the first click
- only a little bit of AI used, i have used a tiny bit of AI only for stuff i haven't figured out how to implement

controls and info:
- top left side number tells the amount of mines
- top right side number tells the amount of moves
- c for cheats
- f to reset camera
- left click on a tile to reveal it
- right click to place a flag or question mark
- middle click for moving the camera
- click on a revealed non-zero tile to reveal tiles next to it

problems:
- you have to recompile to change settings
- the code is wannabe yandere simulator code
- you can't play the game purely with a keyboard as of right now
- game can be flickery if you move the camera very quickly
- single threaded and non object oriented fuckery
- various bugs if you set windowSizeGame boolean to true
- the borders are buggy at the left side
- possibly could use more enums and comments in the code
- no timer

feature ideas:
- more complex cheats
- add a feature where you can't hit a mine when clicking on a tile when starting the game, instead you will always reveal a zero tile
- add config stuff and maybe an ingame settings screen so you don't have to recompile this project to change settings
- maybe add like fancy stuff with mouse, like tiles get highlighted when hovering over them or tiles the mouse is hovering over get pressed down when pressing and holding on tiles
- (related to config stuff) maybe add mouse interactable GUI, like a restart button
- clean up the code
- saving system? infinite MineSweeper levels?

- no gambling setting? what i mean by this is a setting to disable cases where you have to guess which tile is a mine

compiling and running:
install dotnet 10.0 sdk and runtime, then run these in a terminal emulator window:
git clone https://github.com/user5495439/TUI-Minesweeper.git
cd TUI-Minesweeper
dotnet build -c Release
bin/Release/net10.0/MineSweeper

you can copy the binary (bin/Release/net10.0/MineSweeper) to somewhere else if you want, it shouldn't need any dependencies other than dotnet runtime

if you want a binary without any dependencies, not even dotnet then download the binary from the Releases tab, to run that open a terminal emulator window in your Downloads directory and run:
chmod +x ./MineSweeper
./MineSweeper

windows:
- you can maybe get the game to work in wsl but i haven't tried it and it might work shittily there
