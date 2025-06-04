#include "player.hpp"

Player::Player(std::string path, int fw, int fh) : sprite(texture), animation(fw, fh){
	move_speed = 48;
	height = fh;
	width = fw;
	drag = 5;
	facing = sf::Angle(sf::radians(-90));
	if(!texture.loadFromFile(path)) throw "could not load player";

	animation.add_state("idle", 0, 4, 4);
	animation.add_state("walk_right", 1, 8, 8);
	animation.add_state("walk_left", 3, 8, 8);

	collision_box = sf::FloatRect({ 0, 0 }, { 10, 8 });
	collision_box.position += sf::Vector2f({ fw / 2.f - 5.0f, fh / 2.f });
}
void Player::update(sf::Time dt) {

	if (velocity.length() > 0) {
		animation.set_state(velocity.x > 0 ? "walk_right" : "walk_left");
	} else animation.set_state("idle");

	animation.next_frame(dt);
	sprite.setTextureRect(animation.get_frame_rect());

	move_offset(velocity * move_speed * dt.asSeconds());

	velocity -= velocity*drag*dt.asSeconds();
	if (velocity.length() < 0.1) velocity = { 0,0 };
}

void Player::move_offset(sf::Vector2f offset) {
	position += offset;
	collision_box.position += offset;
	sprite.move(offset);
}
void Player::set_position(sf::Vector2f pos) {

	sf::Vector2f offset = pos - position;
	move_offset(offset);
}

void Player::set_center(sf::Vector2f pos) {
	set_position(pos - sf::Vector2f({width/2.f, height/2.f}));
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
