#pragma once

#include <vector>
#include <string>
#include <map>
#include "globals.hpp"

class MapGenerator {

public:
	static const int ROOM_WIDTH = 15;
	static const int ROOM_HEIGHT = 15;

	std::vector<std::vector<TileType>> map;
	std::map<std::string, std::vector<std::vector<TileType>>> presets;
	std::vector<std::string> preset_keys;

	int map_width;
	int map_height;

	int grid_width;
	int grid_height;

	bool presets_loaded;

	MapGenerator(int grid_width, int grid_height);

	void generate_map();

private:
	void load_presets();
	void remove_barrier(std::pair<int, int> from, std::pair<int, int> to);
	void open_room(bool** map, int i, int j, std::vector<std::pair<int, int>>* stack);
	bool in_bounds(std::pair<int, int> loc);

};