

#include <SFML/Graphics.hpp>
#include "map_gen.hpp"
#include "tilemap.hpp"
#include "player.hpp"
#include "globals.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SFML works!");
    window.setKeyRepeatEnabled(false);

    MapGenerator mapgen(3, 3);
    mapgen.generate_map();

    TileMap::load_tile_textures();
    TileMap tM(64, 64, mapgen.map);
    
    Player knight("resources\\assets\\knight.png", 32, 32, { 20, 20 });
    knight.animation.add_state("idle", 0, 4, 4);
    knight.animation.add_state("walk_right", 1, 8, 8);
    knight.animation.add_state("walk_left", 3, 8, 8);
    knight.animation.add_state("walk_top", 2, 8, 8);
    knight.animation.add_state("walk_down", 4, 8, 8);

    knight.sprite.setScale({4, 4});

    sf::Clock clock;
    sf::Time dt;

    while (window.isOpen())
    {
        dt = clock.restart();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>())
            {
                switch (key_pressed->scancode) {
                case sf::Keyboard::Scancode::Right:
                    knight.animation.set_state("walk_right");
                    break;
                case sf::Keyboard::Scancode::Left:
                    knight.animation.set_state("walk_left");

                    break;
                }
            }
        }
        knight.update(dt);
        window.clear();
        window.draw(tM);
        window.draw(knight.sprite);
        window.display();
    }
}