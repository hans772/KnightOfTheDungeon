#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType : char {
	WALL = '#',
	FLOOR = ' ',
	TRAP_SPIKE = '*',
	TRAP_ARROW = '$'
};

sf::Color lerp_color(const sf::Color& a, const sf::Color& b, float t);

TileType charToTileType(char c);

static const int FRAMERATE = 60;

struct Vec2Hash {
	std::size_t operator()(const sf::Vector2f& v) const;
};