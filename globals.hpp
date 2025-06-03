#pragma once
#include <vector>

enum class TileType : char {
	WALL_VERT = '#',
	WALL_HOR = '=',
	FLOOR = ' ',
	TRAP_SPIKE = '*',
	TRAP_ARROW = '$'
};

TileType charToTileType(char c);

static const int FRAMERATE = 60;