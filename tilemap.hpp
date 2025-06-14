#pragma once

#include <SFML/Graphics.hpp>
#include "globals.hpp"
#include <unordered_map>
#include <vector>

enum class TileState {
	DEFAULT,
	WALL_FOOT,
	SPIKE_ACTIVATED,
	POTION_USED,
	EYE_USED
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

	TileType type;
	TileState state;

	Tile(TileType type, TileState state, sf::Vector2f position, int index = -1);

	bool collidable;
	bool opaque;

	sf::FloatRect get_bounds();
	std::vector<sf::Vector2f> get_vertices(sf::Vector2f);
	
	void set_color(sf::Color);
	void set_state(TileState state, int index = -1);
	static void add_state(TileType type, TileState state, int row, int column, int number);

};

class TileMap : public sf::Drawable {
public:
	int tilewidth, tileheight;

	TileMap(int tilewidth, int tile_height);
	
	void generate_tilemap(std::vector<std::vector<TileType>>& map);

	std::vector<std::vector<Tile>> tiles;

	static void load_tile_textures();
	std::vector<Tile*> check_collision( const sf::FloatRect &player_bounds);

	Tile* get_tile_at_pos(sf::Vector2f);

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};