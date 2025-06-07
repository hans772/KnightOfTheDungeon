#include "player.hpp"

Player::Player(std::string path, int fw, int fh) : sprite(texture), animation(fw, fh){
	move_speed = 48;
	height = fh;
	width = fw;
	drag = 5;
	facing = sf::Vector2f({ 0.f, 1.f });
	fov = sf::degrees(120);
	if(!texture.loadFromFile(path)) throw "could not load player";

	invulnerable = false;

	animation.add_state("idle", 0, 4, 4);
	animation.add_state("walk_right", 1, 8, 8);
	animation.add_state("walk_left", 3, 8, 8);
	animation.add_state("win", 12, 8, 3.9);
	animation.add_state("die", 15, 8, 3.9);

	// add state name, row, number of frames, frame per sec

	lives_remaining = 3;

	collision_box = sf::FloatRect({ 0, 0 }, { 10, 8 });
	collision_box.position += sf::Vector2f({ fw / 2.f - 5.0f, fh / 2.f });
}
void Player::update(sf::Time dt) {

	animation.next_frame(dt);
	sprite.setTextureRect(animation.get_frame_rect());

	if (!lives_remaining) return;

	//alive functions

	if (velocity.length() > 0) {
		animation.set_state(velocity.x > 0 ? "walk_right" : "walk_left");
	} else animation.set_state("idle");

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

void Player::set_angle_vector(sf::Vector2f angle) {
	facing = angle.normalized();
}

sf::Vector2f Player::get_center() {
	return position + sf::Vector2f({ width / 2.f, height / 2.f });
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

bool Player::check_collision(sf::Vector2f pos) {
	return ((collision_box.position.x < pos.x) && (pos.x < collision_box.position.x + collision_box.size.x) &&
		(collision_box.position.y < pos.y) && (pos.y < collision_box.position.y + collision_box.size.y));
}

void Player::rotate(sf::Angle angle) {
	facing = facing.rotatedBy(angle);
}

void Player::hurt() {
	invulnerable = true;
	lives_remaining--;
}

void Player::stop_invulnerability() {
	invulnerable = false;
}

void Player::die() {
	animation.set_state("die");
	animation.set_frame(0);
}

void Player::win() {
	animation.set_state("win");
	animation.set_frame(0);
}
