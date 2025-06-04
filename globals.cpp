#include "globals.hpp"

TileType charToTileType(char c) {
	switch (c) {
	case ' ': return TileType::FLOOR;
	case '#': return TileType::WALL;
	case '*': return TileType::TRAP_SPIKE;
	case '$': return TileType::TRAP_ARROW;
	default: throw "Invalid tile character";
	}
}
