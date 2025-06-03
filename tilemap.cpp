#include "tilemap.hpp"
#include "iostream"
#include <random>

std::unordered_map<TileType, std::unordered_map<TileState, std::vector<sf::IntRect>>> Tile::tilemap;
sf::Texture Tile::tile_atlas;
int Tile::tile_width, Tile::tile_height;

Tile::Tile(TileType type, TileState state , sf::Vector2f position) : sprite(tile_atlas) {
	std::vector<sf::IntRect> rectvect = tilemap[type][state];

	if (rectvect.size() > 1) {
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_int_distribution<> dist(0, rectvect.size() - 1);

		sprite.setTextureRect(rectvect[dist(gen)]);
	}
	else {
		sprite.setTextureRect(rectvect[0]);

	}
	
	setPosition(position);
	collidable = false;
}

void Tile::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	target.draw(sprite, states);
}

void Tile::add_state(TileType type, TileState state, int row, int column, int number) {
	
	for (int i = 0; i < number; i++) {
		tilemap[type][state].push_back(sf::IntRect({tile_width*column, tile_height * row}, {tile_width, tile_height}));

	}
}

void TileMap::load_tile_textures() {
	if (!(Tile::tile_atlas.loadFromFile("resources\\assets\\bricktiles.png"))) return;
	Tile::tile_width = 16;
	Tile::tile_height = 16;

	Tile::add_state(TileType::FLOOR, TileState::DEFAULT, 2, 0, 1);
	Tile::add_state(TileType::WALL_HOR, TileState::WALL_FOOT, 0, 2, 2);
	Tile::add_state(TileType::WALL_VERT, TileState::WALL_FOOT, 0, 2, 2);
	Tile::add_state(TileType::WALL_HOR, TileState::DEFAULT, 0,0,2);
	Tile::add_state(TileType::WALL_VERT, TileState::DEFAULT, 0, 0, 2);
	Tile::add_state(TileType::TRAP_SPIKE, TileState::DEFAULT, 1, 0, 1);
	Tile::add_state(TileType::TRAP_SPIKE, TileState::SPIKE_ACTIVATED, 1, 1, 1);
	Tile::add_state(TileType::TRAP_ARROW, TileState::DEFAULT, 3, 0, 1);
}

TileMap::TileMap(int tilewidth, int tileheight, std::vector<std::vector<TileType>> &map) : tileheight(tileheight), tilewidth(tilewidth) {
	for (int j = 0; j < map.size(); j++) {
		tiles.push_back(std::vector<Tile>());
		for (int i = 0; i < map[j].size(); i++) {
			TileType type = map[j][i];
			TileState state = TileState::DEFAULT;
			
			switch (type) {
			case TileType::WALL_HOR:
			case TileType::WALL_VERT:
				if ((j<map.size()-1) && (map[j + 1][i] == TileType::FLOOR|| map[j + 1][i] == TileType::TRAP_SPIKE || map[j + 1][i] == TileType::TRAP_ARROW)) {
					state = TileState::WALL_FOOT;
				}
			}
			Tile newtile = Tile(type, state, sf::Vector2f(i * tilewidth, j * tileheight));
			newtile.setScale({(float)tilewidth/16, (float)tileheight/16});
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