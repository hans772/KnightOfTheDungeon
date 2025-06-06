#include "globals.hpp"

TileType char_to_tile_type(char c) {
	switch (c) {
	case ' ': return TileType::FLOOR;
	case '#': return TileType::WALL;
	case '*': return TileType::TRAP_SPIKE;
	case '$': return TileType::TRAP_ARROW;
	case 'R': return TileType::REWARD;
	case 'P': return TileType::POTION;
	case 'E': return TileType::EYE;
	default: throw "Invalid tile character";
	}
}

sf::Color lerp_color(const sf::Color& a, const sf::Color& b, float t){
    auto lerp = [](uint8_t start, uint8_t end, float t) -> uint8_t  {
        return static_cast<uint8_t>(start + t * (end - start));
        };

    return sf::Color(
        lerp(a.r, b.r, t),
        lerp(a.g, b.g, t),
        lerp(a.b, b.b, t),
        lerp(a.a, b.a, t)
    );
}

std::size_t Vec2Hash::operator()(const sf::Vector2f& v) const {
	return std::hash<float>()(v.x) ^ std::hash<float>()(v.y);
}