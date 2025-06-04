#include "tilemap.hpp"
#include "iostream"
#include <random>

std::unordered_map<TileType, std::unordered_map<TileState, std::vector<sf::IntRect>>> Tile::tilemap;
sf::Texture Tile::tile_atlas;
int Tile::tile_width, Tile::tile_height;

Tile::Tile(TileType type, TileState state , sf::Vector2f position, int index = -1) : sprite(tile_atlas) {
	std::vector<sf::IntRect> rectvect = tilemap[type][state];
	setPosition(position);
	collidable = false;

	if (index >= 0) sprite.setTextureRect(rectvect[index]);
	else {
		if (rectvect.size() > 1) {
			std::random_device rd;
			std::mt19937 gen(rd());

			std::uniform_int_distribution<> dist(0, rectvect.size() - 1);

			sprite.setTextureRect(rectvect[dist(gen)]);
		}
		else {
			sprite.setTextureRect(rectvect[0]);

		}

	}	
}

void Tile::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(sprite, states);
}

void Tile::add_state(TileType type, TileState state, int row, int column, int number) {
	
	for (int i = 0; i < number; i++) {
		tilemap[type][state].push_back(sf::IntRect({tile_width*(column+i), tile_height * row}, {tile_width, tile_height}));

	}
}

void TileMap::load_tile_textures() {
	if (!(Tile::tile_atlas.loadFromFile("resources\\assets\\moderntiles.png"))) return;
	Tile::tile_width = 16;
	Tile::tile_height = 16;

	Tile::add_state(TileType::FLOOR, TileState::DEFAULT, 1, 0, 3);
	Tile::add_state(TileType::WALL, TileState::DEFAULT, 0, 0, 16);
	Tile::add_state(TileType::TRAP_SPIKE, TileState::DEFAULT, 2, 0, 1);
	Tile::add_state(TileType::TRAP_SPIKE, TileState::SPIKE_ACTIVATED, 2, 1, 1);
	Tile::add_state(TileType::TRAP_ARROW, TileState::DEFAULT, 3, 0, 1);
}

TileMap::TileMap(int tilewidth, int tileheight) : tileheight(tileheight), tilewidth(tilewidth) {}

void TileMap::generate_tilemap(std::vector<std::vector<TileType>>& map){
	for (int j = 0; j < map.size(); j++) {
		tiles.push_back(std::vector<Tile>());
		for (int i = 0; i < map[j].size(); i++) {
			TileType type = map[j][i];
			TileState state = TileState::DEFAULT;
			int bitmask = -1;
			switch (type) {
			case TileType::WALL: {
				bitmask++;
				if (j > 0 && map[j - 1][i] == TileType::WALL) bitmask |= 1;  
				if (i < map[0].size() - 1 && map[j][i + 1] == TileType::WALL) bitmask |= 2;
				if (j < map.size() - 1 && map[j + 1][i] == TileType::WALL) bitmask |= 4;  
				if (i > 0 && map[j][i - 1] == TileType::WALL) bitmask |= 8; 

				std::cout << bitmask;
			}
			}
			Tile newtile = Tile(type, state, sf::Vector2f(i * tilewidth, j * tileheight), bitmask);
			tiles.back().push_back(newtile);
		}
	}
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (const auto& vector : tiles) {
		for (const Tile& tile : vector) {
			target.draw(tile, states);
		}
	}
}