# DungeonCrawler

This game is a **dungeon-crawler** style game, built purely in **C++** using **SFML**. It provides a challenging but fun and entertaining game to play in your free time.

## Dependencies

- Built using SFML 3.0.0
- Tested using MSVS 2022.

## Features

- Randomly generated dungeons: Dungeons can range from 3x3 to 5x5 and paths are also randomized each time.
- Scalable event handling.
- Ray Tracing to determine player FOV.
- 4 Bit masking for wall textures.
- Powerups which buff the player allowing for double speed and extra vision.
- Traps which hinder the player adding to the difficulty of the game.

## Installation

- Clone the repository
```bash
git clone https://github.com/hans772/KnightOfTheDungeon.git
```
- Create a new VS Project.
- Set up the project for `SFML-3.0.0`.
- Into the project folder, add the .dll files provided by SFML

```bash
sfml-audio-3
sfml-graphics-3
sfml-system-3
sfml-window-3
sfml-network-3
```

- Add the code files into the project and the resources folder.
- Build the project in `Release` configuration.

