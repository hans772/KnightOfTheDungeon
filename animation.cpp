#include "animation.hpp"
#include "globals.hpp"
#include <iostream>
Animation::Animation(int fw, int fh) : frame_width(fw), frame_height(fh), frame(0) {}

void Animation::add_state(std::string state, int row, int frames, int speed) {
	anim_states[state]["row"] = row;
	anim_states[state]["max_frames"] = frames;
	anim_states[state]["speed"] = speed;

	// if no current state made, sets the state
	if (!current_state.length()) set_state(state);
}

// the animation loads one sprite sheet, and loops through the same texture and returns a differrent frame each time
// hence we get the frame rect

sf::IntRect Animation::get_frame_rect() {
	return sf::IntRect({ (int)frame * frame_width , current_row * frame_height }, { frame_width, frame_height });
}

void Animation::set_state(std::string state) {
	current_state = state;
	current_row = anim_states[state]["row"];
	current_max_frames = anim_states[state]["max_frames"];
	cur_anim_speed = anim_states[state]["speed"];
}

void Animation::next_frame(sf::Time dt) {
	frame+=dt.asSeconds()*cur_anim_speed;
	if(frame >= current_max_frames) frame-=current_max_frames * (int)(frame/current_max_frames);
}

void Animation::set_frame(int f) {
	frame = f;
}