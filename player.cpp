#include "player.hpp"

Player::Player(std::string path, int fw, int fh) : sprite(texture), animation(fw, fh, 12){
	move_speed = 48;
	drag = 5;
	facing = sf::Angle(sf::radians(-90));
	if(!texture.loadFromFile(path)) throw "could not load player";
}
void Player::update(sf::Time dt) {
	animation.next_frame(dt);
	sprite.setTextureRect(animation.get_frame_rect());

	position += velocity * move_speed * dt.asSeconds();
	sprite.move(velocity * move_speed * dt.asSeconds());

	velocity -= velocity*drag*dt.asSeconds();
}
void Player::set_position(sf::Vector2f pos) {
	position = pos;
	sprite.setPosition(pos);
}
void Player::move(sf::Keyboard::Scan key) {
	switch (key)
	{
	case sf::Keyboard::Scan::Right:
		velocity.x = 1;
		break;
	case sf::Keyboard::Scan::Left:
		velocity.x = -1;
		break;
	case sf::Keyboard::Scan::Up:
		velocity.y = -1;
		break;
	case sf::Keyboard::Scan::Down:
		velocity.y = 1;
		break;
	}

	velocity = velocity.normalized();
}
void Player::rotate(sf::Keyboard::Scan key) {

}