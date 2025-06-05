

#include <SFML/Graphics.hpp>
#include "map_gen.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "globals.hpp"
#include "game_state.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML works!");
    window.setKeyRepeatEnabled(false);

    TileMap::load_tile_textures();

    sf::Clock clock;
    sf::Time dt;

    GameStateManager::get().push_state(std::make_unique<GameOver>(800, 800));
    GameStateManager::get().push_state(std::make_unique<MainGame>(5, 5));
    GameStateManager::get().apply_queued_actions();

    while (window.isOpen())
    {
        dt = clock.restart();

        GameState* current_state = GameStateManager::get().current_state();
        window.setView(current_state->game_view);

        // Event handling

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
            {
                current_state->event_handler.handle_single_k_events(key_pressed);
            }
        }

        current_state->event_handler.handle_repeat_k_events();
        current_state->event_handler.run_scheduled_events(dt);

        current_state->update(dt, window);

        window.clear();

        current_state->render(window);

        window.display();

        GameStateManager::get().apply_queued_actions();
    }
}