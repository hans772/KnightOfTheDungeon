#pragma once
#include <vector>

enum class TileType : char {
	WALL = '#',
	FLOOR = ' ',
	TRAP_SPIKE = '*',
	TRAP_ARROW = '$'
};

TileType charToTileType(char c);

static const int FRAMERATE = 60;