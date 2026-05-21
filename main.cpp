#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional> 

const int ROWS = 15;
const int COLS = 15;
const float TILE_SIZE = 40.0f;

struct Player {
    int x_grid, y_grid;
    sf::RectangleShape body;
};

int gameMap[ROWS][COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 0, 0, 1},
    {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1},
    {1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 2, 1},
    {1, 2, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 2, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 1},
    {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 2, 1},
    {1, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 1},
    {1, 2, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 2, 1},
    {1, 0, 2, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 2, 2, 2, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 1},
    {1, 0, 1, 2, 1, 2, 1, 2, 1, 0, 1, 2, 1, 0, 1},
    {1, 0, 0, 0, 2, 0, 2, 0, 0, 2, 0, 2, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int main() {
   
    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(COLS * TILE_SIZE), static_cast<unsigned int>(ROWS * TILE_SIZE) }), "Bomberman Klonu v0.1 - SFML 3");
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    Player player;
    player.x_grid = 1;
    player.y_grid = 1;
    player.body.setSize(sf::Vector2f(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f));
    player.body.setFillColor(sf::Color::Blue);
    
    player.body.setOrigin(sf::Vector2f(TILE_SIZE * 0.4f, TILE_SIZE * 0.4f));

    while (window.isOpen()) {
        
        while (const std::optional<sf::Event> event = window.pollEvent()) {

           
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                int next_x = player.x_grid;
                int next_y = player.y_grid;

                
                if (keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::W) {
                    next_y -= 1;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Down || keyPressed->code == sf::Keyboard::Key::S) {
                    next_y += 1;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Left || keyPressed->code == sf::Keyboard::Key::A) {
                    next_x -= 1;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Right || keyPressed->code == sf::Keyboard::Key::D) {
                    next_x += 1;
                }

                if (gameMap[next_y][next_x] == 0) {
                    player.x_grid = next_x;
                    player.y_grid = next_y;
                }
            }
        }

        player.body.setPosition(sf::Vector2f(player.x_grid * TILE_SIZE + TILE_SIZE / 2.0f, player.y_grid * TILE_SIZE + TILE_SIZE / 2.0f));

        window.clear();

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                tile.setPosition(sf::Vector2f(j * TILE_SIZE, i * TILE_SIZE));

                if (gameMap[i][j] == 1) {
                    tile.setFillColor(sf::Color::White);
                }
                else if (gameMap[i][j] == 2) {
                    tile.setFillColor(sf::Color(139, 69, 19));
                }
                else {
                    tile.setFillColor(sf::Color::Green);
                }
                window.draw(tile);
            }
        }

        window.draw(player.body);
        window.display();
    }

    return 0;
}