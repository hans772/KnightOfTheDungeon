#include "event_handler.hpp"
#include <iostream>

// due to the way SFML is setup, it is easier to use dedicated functions to handle specific types of functions
// rather to template is
// due to this being a small scale project, i chose to do this instead of templating

void EventHandler::register_key_event(sf::Keyboard::Scan key, KeyCallback callback, bool repeat) {
	key_events[key].push_back({ callback, repeat });
}

void EventHandler::schedule_event(SchedCallback callback, float interval, int repeats, bool indefinite) {
	scheduled_events.push_back({ callback, repeats, indefinite, interval + elapsed_time, interval });
}
//k_events => key events
void EventHandler::handle_single_k_events(const sf::Event::KeyPressed* keypress) {
	const auto &iterator = key_events.find(keypress->scancode);
	if (iterator != key_events.end()) {
		for (auto& event : (iterator->second)) {
			if (!event.repeat) (event.callback)(keypress->scancode);
		}
	}
}
void EventHandler::handle_repeat_k_events() {
	for (auto& keyval : key_events) {
		if (sf::Keyboard::isKeyPressed(keyval.first)) {
			for (KeyEvent& event : keyval.second) {
				if (event.repeat) (event.callback)(keyval.first);
			}
		}
	}
}

void EventHandler::run_scheduled_events(sf::Time dt) {
	elapsed_time += dt.asSeconds();
	std::vector<int> removes;
	for (int i = 0; i < scheduled_events.size(); i++) {
		ScheduledEvent *event = &scheduled_events[i];
		if (event->next_execute < elapsed_time) {
			event->callback();
			if (event->repeats || event->indefinite) {
				event->next_execute = elapsed_time+event->seconds_interval;
				event->repeats--;
			}
			else {
				removes.push_back(i);
			}
		}
	}

	for (auto r = removes.rbegin(); r != removes.rend(); r++) {
		scheduled_events.erase(scheduled_events.begin() + (*r));
	}
}