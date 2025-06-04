#pragma once

#include "SFML/Graphics.hpp"
#include "animation.hpp"

class Player {
	sf::Texture texture;

public:
	Animation animation;
	sf::Sprite sprite;
	sf::FloatRect collision_box;
	sf::Vector2f position;
	sf::Angle facing;
	float move_speed;
	float drag;
	int height;
	int width;

	sf::Vector2f velocity;

	Player(std::string path, int fw, int fh);
	void update(sf::Time dt);

	void set_position(sf::Vector2f pos);
	void set_center(sf::Vector2f);
	void move(sf::Keyboard::Scan key);
	void move_offset(sf::Vector2f);
	void rotate(sf::Keyboard::Scan key);
};
