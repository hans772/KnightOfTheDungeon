#pragma once

#include "SFML/Graphics.hpp"
#include "animation.hpp"

class Player {
	sf::Texture texture;

public:
	Animation animation;
	sf::Sprite sprite;
	sf::Vector2f position;
	sf::Angle facing;

	Player(std::string path, int fw, int fh, sf::Vector2f pos);
	void update(sf::Time dt);
};
