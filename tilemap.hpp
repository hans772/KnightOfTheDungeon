#pragma once

#include <SFML/Graphics.hpp>
#include "globals.hpp"
#include <unordered_map>
#include <vector>

enum class TileState {
	DEFAULT,
	WALL_FOOT,
	SPIKE_ACTIVATED
};

class Tile : public sf::Drawable, public sf::Transformable{
private:
	
	static const sf::Texture def_texture;

	sf::Sprite sprite;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:

	static std::unordered_map<TileType, std::unordered_map<TileState, std::vector<sf::IntRect>>> tilemap;
	static sf::Texture tile_atlas;
	static int tile_width, tile_height;

	Tile(TileType type, TileState state, sf::Vector2f position);

	std::string state;

	bool collidable;

	static void add_state(TileType type, TileState state, int row, int column, int number);
};

class TileMap : public sf::Drawable {
public:
	int tilewidth, tileheight;

	TileMap(int tilewidth, int tileheight, std::vector<std::vector<TileType>> &map);
	std::vector<std::vector<Tile>> tiles;

	static void load_tile_textures();

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};