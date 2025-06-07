#pragma once

#include <SFML/Graphics.hpp>

class Arrow : public sf::Sprite {
	static sf::Texture arrow_tex;

	sf::Vector2f position;
	sf::Vector2f direction;

	int move_speed;

public:
	Arrow(sf::Vector2f start_pos, sf::Angle direction);
	void update(sf::Time dt);
	sf::Vector2f get_tip_position();
	static void load_arrow_textures();
};