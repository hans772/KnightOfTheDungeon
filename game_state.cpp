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
	player.set_position(
		(sf::Vector2f((mapgen.ROOM_WIDTH-2) * tilemap.tilewidth, (mapgen.ROOM_HEIGHT-2) * tilemap.tileheight) - player.sprite.getScale())/2.f);
	player.animation.add_state("idle", 0, 4, 4);
	player.animation.add_state("walk_right", 1, 8, 8);
	player.animation.add_state("walk_left", 3, 8, 8);
	player.animation.add_state("walk_top", 2, 8, 8);
	player.animation.add_state("walk_down", 4, 8, 8);

	key_event_handler.register_event(sf::Keyboard::Scan::Up, std::bind(&Player::move, &player, std::placeholders::_1), true);
	key_event_handler.register_event(sf::Keyboard::Scan::Down, std::bind(&Player::move, &player, std::placeholders::_1), true);
	key_event_handler.register_event(sf::Keyboard::Scan::Left, std::bind(&Player::move, &player, std::placeholders::_1), true);
	key_event_handler.register_event(sf::Keyboard::Scan::Right, std::bind(&Player::move, &player, std::placeholders::_1), true);

	game_view.setCenter(player.position);
	game_view.setSize({ 200, 200 });
}

void MainGame::update(sf::Time dt) {
	player.update(dt);
	game_view.setCenter(player.position);
}

void MainGame::render(sf::RenderWindow& window){

	window.draw(tilemap);
	window.draw(player.sprite);
}

