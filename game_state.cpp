#include "game_state.hpp"
#include <functional>

GameStateManager& GameStateManager::get() {
	static GameStateManager instance;
	return instance;
}

void GameStateManager::push_state(std::unique_ptr<GameState> state) {
	states.push(std::move(state));
}

void GameStateManager::pop_state() {
	if (!states.empty()) states.pop();
}

GameState* GameStateManager::current_state() {
	return states.empty() ? nullptr : states.top().get();
}

MainGame::MainGame(int map_w, int map_h) : 
	mapgen(map_w, map_h),
	tilemap(16, 16),
	player("resources\\assets\\knight.png", 32, 32)
{
	mapgen.generate_map();
	tilemap.generate_tilemap(mapgen.map);
	player.set_center(
		(sf::Vector2f((mapgen.ROOM_WIDTH) * tilemap.tilewidth, (mapgen.ROOM_HEIGHT) * tilemap.tileheight))/2.f);

	key_event_handler.register_event(sf::Keyboard::Scan::Up, std::bind(&Player::move, &player, std::placeholders::_1), true);
	key_event_handler.register_event(sf::Keyboard::Scan::Down, std::bind(&Player::move, &player, std::placeholders::_1), true);
	key_event_handler.register_event(sf::Keyboard::Scan::Left, std::bind(&Player::move, &player, std::placeholders::_1), true);
	key_event_handler.register_event(sf::Keyboard::Scan::Right, std::bind(&Player::move, &player, std::placeholders::_1), true);

	game_view.setCenter(player.position);
	game_view.setSize({ 200, 200 });
}

void MainGame::update(sf::Time dt) {
	sf::FloatRect expected_box = player.collision_box;

	std::vector<Tile>* collisions;

	expected_box.position.x += player.velocity.x * player.move_speed * dt.asSeconds();

	collisions = tilemap.check_collision(expected_box);
	for (Tile tile : *collisions) {
		switch (tile.type)
		{
		case TileType::WALL:
			player.velocity.x = 0;
			break;
		}
	}

	collisions->clear();
	delete collisions;
	
	expected_box.position.x = player.collision_box.position.x;
	expected_box.position.y += player.velocity.y * player.move_speed * dt.asSeconds();
	
	collisions = tilemap.check_collision(expected_box);
	for (Tile tile : *collisions) {
		switch (tile.type)
		{
		case TileType::WALL:
		case TileType::TRAP_ARROW:
			player.velocity.y = 0;
			break;
		}
	}

	collisions->clear();
	delete collisions;

	player.update(dt);

	game_view.setCenter(player.position);
}

void MainGame::render(sf::RenderWindow& window){

	window.draw(tilemap);
	window.draw(player.sprite);
}

