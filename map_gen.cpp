#include "map_gen.hpp"
#include <iostream>
#include <algorithm>
#include <random>
#include <fstream>
#include "json.hpp"

MapGenerator::MapGenerator(int grid_width, int grid_height)
	: grid_height(grid_height), grid_width(grid_width) {

	// initialize map size
	map_height = grid_height * (ROOM_HEIGHT - 1) + 1;
	map_width = grid_width * (ROOM_WIDTH - 1) + 1;
	presets_loaded = false;

}

void MapGenerator::load_presets() {
	presets_loaded = true;
	std::ifstream presets_f("resources/presets/rooms.json");
	nlohmann::json presets_json;
	presets_f >> presets_json; 

	for (const auto &preset : presets_json["rooms"]) {
		std::cout << "Loaded Preset : " << preset["name"] << '\n';
		preset_keys.push_back(preset["name"]);
		for (const std::string& row : preset["layout"]) {
			presets[preset["name"]].push_back(std::vector<TileType>());

			std::transform(
				row.begin(), row.end(),
				std::back_inserter(presets[preset["name"]].back()),
				char_to_tile_type
			);
		}
	}
}

bool MapGenerator::in_bounds(std::pair<int, int> loc) {
	return ((loc.first >= 0) && (loc.first < grid_width)) && ((loc.second >= 0) && (loc.second < grid_height));
}

void MapGenerator::remove_barrier(std::pair<int, int> from, std::pair<int, int> to) {
	int room1_x = from.first * (ROOM_WIDTH - 1) + ROOM_WIDTH / 2;
	int room1_y = from.second * (ROOM_HEIGHT - 1) + ROOM_HEIGHT / 2;

	int room2_x = to.first * (ROOM_WIDTH - 1) + ROOM_WIDTH / 2;
	int room2_y = to.second * (ROOM_HEIGHT - 1) + ROOM_HEIGHT / 2;

	// The barrier lies between the centers
	int wall_x = (room1_x + room2_x) / 2;
	int wall_y = (room1_y + room2_y) / 2;

	map[wall_y][wall_x] = TileType::FLOOR;
}

 void MapGenerator::open_room(bool** grid, int i, int j, std::vector<std::pair<int, int>>* stack) {
	grid[j][i] = true;
	std::vector<std::pair<int, int>> neighbours = { {i - 1, j}, {i + 1, j}, {i, j + 1}, {i, j - 1} };

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(neighbours.begin(), neighbours.end(), gen);

	for (int _ = 0; _ < 4; _++) {
		std::pair<int, int> neighbour = neighbours[_];

		if (in_bounds(neighbour)) {
			if (grid[neighbour.second][neighbour.first]) continue;
			stack->push_back({ i, j });
			remove_barrier({ i , j }, neighbour);

			open_room(grid, neighbour.first, neighbour.second, stack);

		}
	}

	if (stack->size()) {
		std::pair<int, int> room = stack->back();
		stack->pop_back();
		open_room(grid, room.first, room.second, stack);
	}
}

// creating grid for map, and then using a DFS algorithm to create a path
// map will be a GWxGH grid of rooms where each room is RWxRH
// first we generate a map of size MW x MH and then place the walls as they are always in the same place
// then when the DFS 'removes the wall' between two cells, we remove the walls from the actual map
void MapGenerator::generate_map(){

	if (!presets_loaded) load_presets();

	int** preset_grid = new int* [grid_height];
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(2, preset_keys.size()-1);

	for (int gr = 0; gr < grid_height; gr++) {
		preset_grid[gr] = new int[grid_width];
		for (int gc = 0; gc < grid_width; gc++) {
			if (!gr && !gc) preset_grid[gr][gc] = (std::find(preset_keys.begin(), preset_keys.end(), "Start")) - preset_keys.begin();
			else if (gr == grid_width - 1 && gc == grid_height - 1) preset_grid[gr][gc] = (std::find(preset_keys.begin(), preset_keys.end(), "End")) - preset_keys.begin();
			else preset_grid[gr][gc] = dist(gen);
		}
	}

	for (int j = 0; j < map_height; j++) {
		map.push_back(std::vector<TileType>());
		for (int i = 0; i < map_width; i++) {
			// i = x coord, j = y coord
			// vertical walls are given priority
			
			// iterate through tiles priority wise
			if (!(i % (ROOM_WIDTH-1)) || !(j % (ROOM_HEIGHT - 1))) {
				map[j].push_back(TileType::WALL);
			}
			else {
				map[j].push_back(
				presets[
					preset_keys[
						preset_grid[j / (ROOM_HEIGHT - 1)][i / (ROOM_WIDTH - 1)]
					]
				][j % (ROOM_HEIGHT - 1) - 1][i % (ROOM_WIDTH - 1) - 1]
				);
			}
		}
	}

	bool** grid = new bool* [grid_height];
	std::vector<std::pair<int, int>> stack;
	for (int gr = 0; gr < grid_height; gr++) {
		grid[gr] = new bool[grid_width] {false};
	}

	open_room(grid, 0, 0, &stack);

	for (int y = 0; y < map_height; y++) {
		for (int x = 0; x < map_width; x++) {

			std::cout << (char)(map[y][x]);
		
		}

		std::cout << '\n';
	}
}

