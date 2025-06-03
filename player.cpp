#include "player.hpp"

Player::Player(std::string path, int fw, int fh, sf::Vector2f pos) : sprite(texture), animation(fw, fh, 12) {
	position = pos;
	facing = sf::Angle(sf::radians(-90));
	if(!texture.loadFromFile(path)) throw "could not load player";
}
void Player::update(sf::Time dt) {
	animation.next_frame(dt);
	sprite.setTextureRect(animation.get_frame_rect());
}