#include "globals.hpp"

TileType charToTileType(char c) {
	switch (c) {
	case ' ': return TileType::FLOOR;
	case '#': return TileType::WALL_VERT;
	case '=': return TileType::WALL_HOR;
	case '*': return TileType::TRAP_SPIKE;
	case '$': return TileType::TRAP_ARROW;
	default: throw "Invalid tile character";
	}
}
