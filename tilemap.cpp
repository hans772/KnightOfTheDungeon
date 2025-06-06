#include "tilemap.hpp"
#include "iostream"
#include <random>

std::unordered_map<TileType, std::unordered_map<TileState, std::vector<sf::IntRect>>> Tile::tilemap;
sf::Texture Tile::tile_atlas;
int Tile::tile_width, Tile::tile_height;

Tile::Tile(TileType type, TileState state , sf::Vector2f position, int index) : sprite(tile_atlas), type(type), state(state) {
	move(position);
	collidable = false;
	opaque = false;
	set_state(state, index);
}

void Tile::set_state(TileState nstate, int index) {
	state = nstate;

	std::vector<sf::IntRect> rectvect = tilemap[type][state];
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

std::vector<sf::Vector2f> Tile::get_vertices(sf::Vector2f pl_pos) {
	std::vector<sf::Vector2f> vertices;
	sf::Vector2f pos = getPosition();
	sf::FloatRect bounds = get_bounds();
	vertices.push_back(pos);
	vertices.push_back(pos + sf::Vector2f({bounds.size.x, 0}));
	vertices.push_back(pos + sf::Vector2f({ 0, bounds.size.y }));
	vertices.push_back(pos + bounds.size);

	return vertices;
}

sf::FloatRect Tile::get_bounds() {
	return sprite.getGlobalBounds();
}

void Tile::set_color(sf::Color color) {
	sprite.setColor(color);
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
	Tile::add_state(TileType::REWARD, TileState::DEFAULT, 1, 4, 1);
	Tile::add_state(TileType::POTION, TileState::DEFAULT, 1, 5, 1);
	Tile::add_state(TileType::EYE, TileState::DEFAULT, 1, 6, 1);
	Tile::add_state(TileType::POTION, TileState::POTION_USED, 1, 1, 1);
	Tile::add_state(TileType::EYE, TileState::EYE_USED, 1, 1, 1);
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

			}
			}
			Tile newtile = Tile(type, state, sf::Vector2f(i * tilewidth, j * tileheight), bitmask);
			switch (type) {
			case TileType::WALL:
			case TileType::TRAP_ARROW:
				newtile.opaque = true;
			case TileType::TRAP_SPIKE:
			case TileType::REWARD:
			case TileType::POTION:
			case TileType::EYE:
				newtile.collidable = true;
			}
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

std::vector<Tile*> TileMap::check_collision(const sf::FloatRect &player_bounds) {
	sf::Vector2f player_pos = player_bounds.getCenter();
	sf::Vector2i prob_t_index = { (int)player_pos.x / tilewidth, (int)player_pos.y / tileheight };

	int error = 2;

	std::vector<Tile*> col_tiles;

	for (int j = std::max(prob_t_index.y - error, 0); j < std::min(prob_t_index.y + error, (int)tiles.size()); j++) {
		for (int i = std::max( prob_t_index.x - error, 0 ); i < std::min(prob_t_index.x + error, (int)tiles[0].size()); i++) {
			if (tiles[j][i].collidable && sf::FloatRect(tiles[j][i].getPosition(), sf::Vector2f({(float)tilewidth, (float)tileheight})).findIntersection(player_bounds)) {
				col_tiles.push_back(&tiles[j][i]);
			}
		}
	}

	return col_tiles;
}

Tile* TileMap::get_tile_at_pos(sf::Vector2f pos) {
	if (pos.x < 0 || pos.x > tilewidth * tiles[0].size()) return nullptr;
	if (pos.y < 0 || pos.y > tileheight * tiles.size()) return nullptr;
	return &tiles[pos.y / tileheight][pos.x / tilewidth];
}