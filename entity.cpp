#include "entity.hpp"

sf::Texture Arrow::arrow_tex;

Arrow::Arrow(sf::Vector2f start_pos, sf::Angle dir) : sf::Sprite(arrow_tex) {
	
	position = start_pos;
    direction = sf::Vector2f({ 1.f, 0.f }).rotatedBy(dir);
	setOrigin(sf::Vector2f(arrow_tex.getSize()) / 2.f); // set origin to center of the sprite
	rotate(dir);

	move_speed = 30;

}
void Arrow::update(sf::Time dt) {
	position += direction * (float)move_speed * dt.asSeconds();
	setPosition(position);
}

sf::Vector2f Arrow::get_tip_position() {
	return position + direction * sf::Vector2f(arrow_tex.getSize()).x/2.f;
}

void Arrow::load_arrow_textures() {
	arrow_tex.loadFromFile("resources\\assets\\arrow.png");
}