#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>

class Animation {
	float frame;
	int frame_width, frame_height;

	std::string current_state;
	int current_row;
	int current_max_frames;
	int cur_anim_speed;

	std::unordered_map < std::string, std::unordered_map<std::string, int> > anim_states;

public:
	Animation(int fw, int fh);

	void set_state(std::string state);
	void next_frame(sf::Time dt);
	void set_frame(int f);
	void add_state(std::string state, int row, int frames, int speed);
	sf::IntRect get_frame_rect();
};