# TUI-Minesweeper
A TUI minesweeper game made with nice features

description:
- a minesweeper game for terminal emulators that aims to be somewhat polished and customizable

features:
- mouse input
- has like a "quick tile reveal" system similar minesweeper.online where if you click on a revealed non zero tile it will reveal the tiles next to it if the amount of flags matches the tile's number
- lots of settings to tweak to customize the behavior of the game, no config file stuff implemented yet so you have to recompile
- press c for legit cheats (unless disabled). what i mean by legit cheats is that the cheats can only see what you can see, so the cheats can't just magically know where mines are
- the game ragebaits you by giving you a chance to hit a mine on the first click
- isn't like made completely with AI, i have used a tiny bit of AI only for stuff i haven't figured out how to implement

problems:
- the code is wannabe yandere simulator code
- single threaded and non object oriented fuckery
- various bugs if you set windowSizeGame boolean to true
- possibly could use more enums
- no timer

feature ideas:
- more complex cheats
- add a feature where you can't hit a mine when clicking on a tile when starting the game, instead you will always reveal a zero tile
- add config stuff and maybe an ingame settings screen so you don't have to recompile this project to change settings
- maybe add like fancy stuff with mouse, like tiles get highlighted when hovering over them or tiles the mouse is hovering over get pressed down when pressing and holding on tiles
- (related to config stuff) maybe add mouse interactable GUI, like a restart button
- maybe try to make this more object oriented and clean up the code
- saving system? moving the map with a mouse? infinite MineSweeper levels?

- no gambling setting? what i mean by this is a setting to disable cases where you have to guess which tile is a mine
