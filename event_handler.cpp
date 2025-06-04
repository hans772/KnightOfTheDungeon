#include "event_handler.hpp"

void KeyEventHandler::register_event(sf::Keyboard::Scan key, Callback callback, bool repeat) {
	callbacks[key].push_back({ callback, repeat });
}
void KeyEventHandler::handle_single_events(const sf::Event::KeyPressed* keypress) {
	const auto &iterator = callbacks.find(keypress->scancode);
	if (iterator != callbacks.end()) {
		for (auto& pair : (iterator->second)) {
			if (!pair.second) (pair.first)(keypress->scancode);
		}
	}
}
void KeyEventHandler::handle_repeat_events() {
	for (auto& keyval : callbacks) {
		if (sf::Keyboard::isKeyPressed(keyval.first)) {
			for (auto& pair : keyval.second) {
				if (pair.second) (pair.first)(keyval.first);
			}
		}
	}
}