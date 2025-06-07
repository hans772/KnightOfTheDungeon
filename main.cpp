

#include <SFML/Graphics.hpp>
#include "map_gen.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "globals.hpp"
#include "game_state.hpp"
#include <random>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "DungeonCrawler");
    window.setKeyRepeatEnabled(false);

    TileMap::load_tile_textures();

    sf::Clock clock;
    sf::Time dt;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> dist(3, 5);

    // create 2 game over screens, pop once for a loss, pop twice for a win.
    // i could use change state but this is simpler to implement.

    GameStateManager::get().push_state(std::make_unique<GameOver>(800, 800, "win.png"));
    GameStateManager::get().push_state(std::make_unique<GameOver>(800, 800, "game_over.png"));
    GameStateManager::get().push_state(std::make_unique<MainGame>(dist(gen) , dist(gen)));
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

        // updating and rendering
        current_state->update(dt, window);

        window.clear();

        current_state->render(window);

        window.display();

        GameStateManager::get().apply_queued_actions();
    }
}