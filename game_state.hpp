#pragma once

#include <SFML/Graphics.hpp>
#include "map_gen.hpp"
#include "player.hpp"
#include "tilemap.hpp"
#include "globals.hpp"
#include "event_handler.hpp"
#include <stack>
class GameState {
public:
	KeyEventHandler key_event_handler;
	sf::View game_view;

public:
	virtual ~GameState() =  default;
	virtual void update(sf::Time dt) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};

class GameStateManager {
public:
	static GameStateManager& get();

	void push_state(std::unique_ptr<GameState> game_state);
	void pop_state();

	GameState* current_state();
private:
	GameStateManager() = default;

	GameStateManager(const GameStateManager&) = delete;
	GameStateManager& operator=(const GameStateManager&) = delete;

	std::stack<std::unique_ptr<GameState>> states;
};

class MainGame : public GameState {
	MapGenerator mapgen;
	TileMap tilemap;
	Player player;


public:
	MainGame(int map_w, int map_h);
	virtual void update(sf::Time dt) override;
	virtual void render(sf::RenderWindow &window) override;
};
