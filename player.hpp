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
	sf::Vector2f facing;
	float move_speed;
	float drag;
	int height;
	int width;
	int lives_remaining;

	bool invulnerable;

	sf::Vector2f velocity;

	Player(std::string path, int fw, int fh);
	void update(sf::Time dt);
	void set_position(sf::Vector2f pos);
	void set_center(sf::Vector2f);
	void set_angle_vector(sf::Vector2f);
	void move(sf::Keyboard::Scan key);
	void move_offset(sf::Vector2f);
	void rotate(sf::Angle);
	void hurt();
	void stop_invulnerability();
	void die();

	sf::Vector2f get_center();

};
