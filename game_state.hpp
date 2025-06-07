#pragma once

#include <SFML/Graphics.hpp>
#include "map_gen.hpp"
#include "player.hpp"
#include "tilemap.hpp"
#include "globals.hpp"
#include "event_handler.hpp"
#include <stack>
#include <unordered_set>
#include "entity.hpp"

class GameState {
public:
	EventHandler event_handler;
	sf::View game_view;

public:
	virtual ~GameState() =  default;
	virtual void update(sf::Time dt, const sf::WindowBase &relative_to_window) = 0;
	virtual void render(sf::RenderWindow& window) = 0;
};

class MainGame : public GameState {
	MapGenerator mapgen;
	TileMap tilemap;
	Player player;

	sf::RenderTexture darkness;

	float camera_shake_offset;

	std::vector<Arrow> arrows;

	sf::Vector2f view_pos;
	sf::Vector2f view_vel;
	sf::Vector2f view_acc;

	bool game_won;

	void set_player_damage_color();
	void summon_arrows_at_tile(Tile *);
	std::pair<std::vector<sf::Vector2f>, std::unordered_set<Tile*>> cast_rays();

public:
	MainGame(int map_w, int map_h);
	virtual void update(sf::Time dt, const sf::WindowBase &relative_to_window) override;
	virtual void render(sf::RenderWindow &window) override;
};

class GameOver : public GameState {
	sf::Texture game_over_image;
	sf::RectangleShape rect_shape;
	
	float progress;

public:
	GameOver(int winw, int winh, std::string);
	virtual void update(sf::Time dt, const sf::WindowBase &relative_to_window) override;
	virtual void render(sf::RenderWindow& window) override;
};

enum class StateActionType { Push, Pop, Change };

struct StateAction {
	StateActionType type;
	std::unique_ptr<GameState> new_state = nullptr;
};

class GameStateManager {
public:
	static GameStateManager& get();

	void push_state(std::unique_ptr<GameState> game_state);
	void pop_state();
	void change_state(std::unique_ptr<GameState> game_state);

	void apply_queued_actions();

	GameState* current_state();
private:
	GameStateManager() = default;

	GameStateManager(const GameStateManager&) = delete;
	GameStateManager& operator=(const GameStateManager&) = delete;

	std::stack<std::unique_ptr<GameState>> states;
	std::vector<StateAction> queued_actions;
};
