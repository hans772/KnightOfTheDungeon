#include "game_state.hpp"
#include <functional>
#include <random>
#include <iostream>
#include <unordered_set>

GameStateManager& GameStateManager::get() {
	static GameStateManager instance;
	return instance;
}

// the gamestate manager adds each action to queue, and after each game loop, all queued actions will be executed
// else there is a possibility that the referenced game state gets deleted throwing an error

void GameStateManager::push_state(std::unique_ptr<GameState> state) {
	queued_actions.push_back({ StateActionType::Push, std::move(state) });
}

void GameStateManager::pop_state() {
	queued_actions.push_back({ StateActionType::Pop});
}

void GameStateManager::change_state(std::unique_ptr<GameState> game_state) {
	queued_actions.push_back({ StateActionType::Change, std::move(game_state) });
}

void GameStateManager::apply_queued_actions() {
	for (auto& action : queued_actions) {
		switch (action.type) {
		case StateActionType::Push:
			states.push(std::move(action.new_state));
			break;
		case StateActionType::Pop:
			if (!states.empty())
				states.pop();
			break;
		case StateActionType::Change:
			if (!states.empty())
				states.pop();
			states.push(std::move(action.new_state));
			break;
		}
	}
	queued_actions.clear();
}

GameState* GameStateManager::current_state() {
	return states.empty() ? nullptr : states.top().get();
}

MainGame::MainGame(int map_w, int map_h): 
	mapgen(map_w, map_h),
	tilemap(16, 16),
	darkness({200, 200}),
	player("resources\\assets\\knight.png", 32, 32)
{
	mapgen.generate_map();
	tilemap.generate_tilemap(mapgen.map);

	// sets player pos and camera to the center of the start room

	player.set_center(
		(sf::Vector2f((mapgen.ROOM_WIDTH) * tilemap.tilewidth, (mapgen.ROOM_HEIGHT) * tilemap.tileheight))/2.f);
	view_pos = player.get_center();

	Arrow::load_arrow_textures();

	// scheduling spawn arrow events for each arrow trap tile
	for (int j = 0; j < mapgen.map_height; j ++ ) {
		for (int i = 0; i < mapgen.map_width; i++) {
			if (tilemap.tiles[j][i].type == TileType::TRAP_ARROW) {
				event_handler.schedule_event([j, i, this]() {this->summon_arrows_at_tile(&this->tilemap.tiles[j][i]); }, 2, 0, true);
			}
		}
	}

	game_won = false;

	//player movement

	event_handler.register_key_event(sf::Keyboard::Scan::Up, std::bind(&Player::move, &player, std::placeholders::_1), true);
	event_handler.register_key_event(sf::Keyboard::Scan::Down, std::bind(&Player::move, &player, std::placeholders::_1), true);
	event_handler.register_key_event(sf::Keyboard::Scan::Left, std::bind(&Player::move, &player, std::placeholders::_1), true);
	event_handler.register_key_event(sf::Keyboard::Scan::Right, std::bind(&Player::move, &player, std::placeholders::_1), true);

	game_view.setCenter(player.position);
	game_view.setSize({ 200, 200 });

	darkness.clear(sf::Color::Transparent);
}

// gameover could be win or lose

GameOver::GameOver(int winw, int winh, std::string name) {
	if (!game_over_image.loadFromFile("resources\\assets\\" + name)) throw "Could not load Game Over Screen";
	rect_shape.setSize(sf::Vector2f({ (float)winw, (float)winh }));
	rect_shape.setTexture(&game_over_image);
	rect_shape.setFillColor(sf::Color::Black);
	game_view.setCenter(sf::Vector2f({ (float)winw, (float)winh }) / 2.f);
	progress = 0;
}

// all 4 sides of tile
void MainGame::summon_arrows_at_tile(Tile * tile) {
	arrows.push_back(Arrow(tile->getPosition() + tile->get_bounds().size / 2.f + sf::Vector2f(0, tilemap.tileheight), sf::degrees(90)));
	arrows.push_back(Arrow(tile->getPosition() + tile->get_bounds().size / 2.f + sf::Vector2f(0, -tilemap.tileheight), sf::degrees(-90)));
	arrows.push_back(Arrow(tile->getPosition() + tile->get_bounds().size / 2.f + sf::Vector2f(tilemap.tilewidth, 0), sf::degrees(0)));
	arrows.push_back(Arrow(tile->getPosition() + tile->get_bounds().size / 2.f + sf::Vector2f(-tilemap.tilewidth, 0) , sf::degrees(-180)));
}

// casts a ray every 1 degree for the whole fov of the player
// on hitting an opaque block it stops casting and adds the end pos and the opaque block to a vector
// vector is returned in the end

std::pair<std::vector<sf::Vector2f>, std::unordered_set<Tile*>> MainGame::cast_rays() {
	sf::Angle fov_start = player.fov * -0.5f;
	sf::Angle fov_end = player.fov * 0.5f;
	sf::Angle step = sf::degrees(1);

	sf::Vector2f origin = player.get_center();
	std::vector<sf::Vector2f> vecs;
	std::unordered_set<Tile*> touches;

	float max_length = 75.0f;
	sf::Angle max_angle;

	for (sf::Angle a = fov_start; a <= fov_end; a += step) {
		for (float offset : {-0.0001f, 0.0f, 0.0001f}) {
			sf::Vector2f dir = player.facing.rotatedBy(a+sf::radians(offset));
			sf::Vector2f ray;


			for (float len = 1; len < max_length;) {
				ray = origin + dir * len;

				Tile* tile = tilemap.get_tile_at_pos(ray);
				if (tile == nullptr) {
					break;
				}
				if (tile && tile->opaque) {
					if (touches.find(tile) == touches.end()) touches.insert(tile);
					vecs.push_back(ray);
					break;
				}
				len += max_length / 120;
				if (len >= max_length) vecs.push_back(origin + dir * max_length);
			}
		}
		
	}
	return { vecs , touches };
}

void MainGame::set_player_damage_color() {
	player.sprite.setColor(sf::Color::Red);
	sf::Sprite* player_sprite = &player.sprite;
	event_handler.schedule_event([player_sprite]() {player_sprite->setColor(sf::Color::White); }, 0.2f, 0);
}


void MainGame::update(sf::Time dt, const sf::WindowBase& relative_to_window) {
	sf::FloatRect expected_box = player.collision_box;
	std::vector<Tile*> collisions;

	if (!player.lives_remaining || game_won) {
		player.update(dt);
		return;
	}

	sf::Vector2u win_size = relative_to_window.getSize();
	sf::Vector2f player_pos_wrt_window = sf::Vector2f({ win_size.x / 2.f , win_size.y / 2.f}) + player.get_center() - view_pos;
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(relative_to_window);

	sf::Vector2f disp = sf::Vector2f({ (float)mouse_pos.x, (float)mouse_pos.y }) - player_pos_wrt_window;
	player.set_angle_vector(disp);

	// checking x collisions;

	expected_box.position.x += player.velocity.x * player.move_speed * dt.asSeconds();
	collisions = tilemap.check_collision(expected_box);
	for (Tile *tile : collisions) {
		switch (tile->type)
		{
		case TileType::WALL:
		case TileType::TRAP_ARROW:
			player.velocity.x = 0;
			break;
		}
	}
	
	// checking y collisions

	expected_box.position.x = player.collision_box.position.x;
	expected_box.position.y += player.velocity.y * player.move_speed * dt.asSeconds();
	collisions = tilemap.check_collision(expected_box);
	for (Tile *tile : collisions) {
		switch (tile->type)
		{
		case TileType::WALL:
		case TileType::TRAP_ARROW:
			player.velocity.y = 0;
			break;
		}
	}

	// checking general collisions

	expected_box.position.x += player.velocity.x * player.move_speed * dt.asSeconds();
	collisions = tilemap.check_collision(expected_box);
	
	for (Tile *tile : collisions) {
		switch (tile->type)
		{
		case TileType::TRAP_SPIKE:
			if (!player.invulnerable) {
				player.hurt();
				event_handler.schedule_event(std::bind(&MainGame::set_player_damage_color, this), 0.4f, 5);
				event_handler.schedule_event(std::bind(&Player::stop_invulnerability, &player), 2, 0);
				camera_shake_offset += 2;

				if (!player.lives_remaining) {
					player.die();
					event_handler.schedule_event([]() {
						GameStateManager::get().pop_state();
						}, 2, 0);
				}
			}
			tile->set_state(TileState::SPIKE_ACTIVATED);
			event_handler.schedule_event(
				[tile]() {tile->set_state(TileState::DEFAULT); }, 2, 0
			);
			break;
		case TileType::REWARD: {
			game_won = true;
			player.win();
			sf::Vector2u win_size = relative_to_window.getSize();;

			event_handler.schedule_event([win_size]() {
				GameStateManager::get().pop_state();
				GameStateManager::get().pop_state();
				}, 2, 0);
			break;
		}
		case TileType::POTION:
			player.move_speed *= 2;
			tile->collidable = false;
			tile->set_state(TileState::POTION_USED);
			break;
		case TileType::EYE:
			player.fov = sf::degrees(360);
			tile->collidable = false;
			tile->set_state(TileState::EYE_USED);
			break;
		}
	}

	// checking arrow collisions

	auto it = arrows.begin();
	while (it != arrows.end()) {
		it->update(dt);
		sf::Vector2f pos = it->get_tip_position();
		if (tilemap.get_tile_at_pos(pos)->opaque) {
			it = arrows.erase(it);
		}
		else if (player.check_collision(pos)) {
			if (!player.invulnerable) {
				player.hurt();
				event_handler.schedule_event(std::bind(&MainGame::set_player_damage_color, this), 0.4f, 5);
				event_handler.schedule_event(std::bind(&Player::stop_invulnerability, &player), 2, 0);
				camera_shake_offset += 2;

				if (!player.lives_remaining) {
					player.die();
					event_handler.schedule_event([]() {
						GameStateManager::get().pop_state();
						}, 2, 0);
				}

				it = arrows.erase(it);

			}
		}
		else it++;
	}

	player.update(dt);

	view_vel = (player.get_center() - view_pos) * 1.5f;

	view_vel += view_acc * dt.asSeconds();
	view_pos += view_vel * dt.asSeconds();

	camera_shake_offset -= camera_shake_offset * dt.asSeconds() * 2; 
	if (camera_shake_offset > 1) {
		view_pos += (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2 - 1) * camera_shake_offset * sf::Vector2f({ 1.f,1.f });
	}
	else camera_shake_offset = 0;

	game_view.setCenter(view_pos);
}

void MainGame::render(sf::RenderWindow& window){

	window.draw(tilemap);
	window.draw(player.sprite);

	for (Arrow arrow : arrows) {
		window.draw(arrow);
	}

	darkness.clear(sf::Color::Black);
	// creating a small radius around the player so that the sprite is visible

	sf::CircleShape circle;
	circle.setPosition(player.position - (view_pos - game_view.getSize() / 2.f));
	circle.setPointCount(50);
	circle.setFillColor(sf::Color::White);
	circle.setRadius(16.f);


	// creating atriangle fan out of all the ray endpoints
	sf::VertexArray cone(sf::PrimitiveType::TriangleFan);
	cone.append({ player.get_center() - (view_pos - game_view.getSize() / 2.f), sf::Color::White });

	std::pair<std::vector<sf::Vector2f>, std::unordered_set<Tile*>> colls = cast_rays();

	for (const sf::Vector2f &vert : colls.first) {
		cone.append({ vert - (view_pos - game_view.getSize() / 2.f), sf::Color::White});
	}
	for (Tile* tile : colls.second) {
		sf::RectangleShape rect;
		rect.setPosition(tile->getPosition()-(view_pos - game_view.getSize()/2.f));
		rect.setSize(tile->get_bounds().size);
		rect.setFillColor(sf::Color::White);

		darkness.draw(rect);

	}
	// every collided tile is also fully visible to identify walls and arrow traps

	darkness.draw(cone);
	darkness.draw(circle);
	sf::Sprite mask_sprite = sf::Sprite(darkness.getTexture());
	
	// createsa black and whit emask of visible area to the player
	// this is then multiplied on top of the actual screen to create a darkness effect
	// hence if i chose to make entities they can be draewn on the actual screen and then the darkness on top to hide them in the dark

	mask_sprite.setScale({ 1.f, -1.f });
	mask_sprite.setOrigin({ 0.f, mask_sprite.getLocalBounds().size.y});
	mask_sprite.setPosition((view_pos - game_view.getSize() / 2.f));

	window.draw(mask_sprite, sf::BlendMultiply);

}

// increasing the brightness of the game over screen as time passes
void GameOver::update(sf::Time dt, const sf::WindowBase &relative_to_window) {
	if (progress < 1) {
		progress += 0.2f * dt.asSeconds();
		rect_shape.setFillColor(lerp_color(sf::Color::Black, sf::Color::White, progress));
	}
}

void GameOver::render(sf::RenderWindow& window) {
	window.draw(rect_shape);
}
