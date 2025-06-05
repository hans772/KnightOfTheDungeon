#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>
#include <functional>

using KeyCallback = std::function<void(sf::Keyboard::Scan)>;
using SchedCallback = std::function<void()>;

struct KeyEvent {
	KeyCallback callback;
	bool repeat;
};

struct ScheduledEvent {
	SchedCallback callback;
	int repeats;
	bool indefinite;
	float seconds_interval;
};


class EventHandler {

	std::unordered_map<sf::Keyboard::Scan, std::vector<KeyEvent>> key_events;
	std::vector<ScheduledEvent> scheduled_events;
	std::vector<sf::Keyboard::Key> pressed_keys;

	float elapsed_time = 0;

public:
	void register_key_event(sf::Keyboard::Scan key, KeyCallback callback, bool repeat);
	void schedule_event(SchedCallback callback, float interval, int repeats, bool indefinite = false);
	void handle_single_k_events(const sf::Event::KeyPressed*);
	void handle_repeat_k_events();
	void run_scheduled_events(sf::Time dt);
};