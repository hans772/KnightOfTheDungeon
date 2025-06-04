#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <functional>

class KeyEventHandler {
	using Callback = std::function<void(sf::Keyboard::Scan)>;
	std::unordered_map<sf::Keyboard::Scan, std::vector<std::pair<Callback, bool>>> callbacks;

	std::vector<sf::Keyboard::Key> pressed_keys;

public:
	void register_event(sf::Keyboard::Scan key, Callback callback, bool repeat);
	void handle_single_events(const sf::Event::KeyPressed*);
	void handle_repeat_events();

};