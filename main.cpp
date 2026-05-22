#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <optional>
#include <vector> 

const int ROWS = 15;
const int COLS = 15;
const float TILE_SIZE = 40.0f;


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




class Player {
private:
    int x_grid;
    int y_grid;
    sf::RectangleShape body;

public:
    
    Player(int startX, int startY) {
        x_grid = startX;
        y_grid = startY;
        body.setSize(sf::Vector2f(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f));
        body.setFillColor(sf::Color::Blue);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.4f, TILE_SIZE * 0.4f));
    }

    void move(int dx, int dy) {
        int nextX = x_grid + dx;
        int nextY = y_grid + dy;
       
        if (gameMap[nextY][nextX] == 0) {
            x_grid = nextX;
            y_grid = nextY;
        }
    }

    void updateVisuals() {
        body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
    }

    void draw(sf::RenderWindow& window) {
        window.draw(body);
    }

    
    void setPosition(int x, int y) { x_grid = x; y_grid = y; }
    int getX() { return x_grid; }
    int getY() { return y_grid; }
};



class Explosion {
private:
    int x_grid, y_grid;
    sf::Clock timer;
    sf::RectangleShape body;

public:
    Explosion(int x, int y) {
        x_grid = x;
        y_grid = y;
        body.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        body.setFillColor(sf::Color(255, 140, 0)); 
        body.setPosition(sf::Vector2f(x * TILE_SIZE, y * TILE_SIZE));
    }

    
    bool isExpired() {
        return timer.getElapsedTime().asSeconds() > 0.5f;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(body);
    }

    int getX() { return x_grid; }
    int getY() { return y_grid; }
};



class Bomb {
private:
    bool active;
    int x_grid, y_grid;
    int range; 
    sf::Clock timer;
    sf::RectangleShape body;

public:
    Bomb() {
        active = false;
        range = 1; 
        body.setSize(sf::Vector2f(TILE_SIZE * 0.6f, TILE_SIZE * 0.6f));
        body.setFillColor(sf::Color::Red);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.3f, TILE_SIZE * 0.3f));
    }

    void place(int x, int y) {
        if (active == false) {
            active = true;
            x_grid = x;
            y_grid = y;
            timer.restart();
        }
    }

    
    void checkExplosion(std::vector<Explosion>& explosionsList) {
        if (active == true && timer.getElapsedTime().asSeconds() >= 3.0f) {
            active = false; 

           
            explosionsList.push_back(Explosion(x_grid, y_grid));

            
            int dx[] = { 1, -1, 0, 0 };
            int dy[] = { 0, 0, 1, -1 };

            for (int d = 0; d < 4; d++) {
                for (int i = 1; i <= range; i++) {
                    int nx = x_grid + (dx[d] * i);
                    int ny = y_grid + (dy[d] * i);

                    if (gameMap[ny][nx] == 1) {
                        break;
                    }

                   
                    explosionsList.push_back(Explosion(nx, ny));

                    if (gameMap[ny][nx] == 2) {
                        gameMap[ny][nx] = 0; 
                        break; 
                    }
                }
            }
        }
    }

    void updateVisuals() {
        if (active) {
            body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
        }
    }

    void draw(sf::RenderWindow& window) {
        if (active) window.draw(body);
    }
};


int main() {
    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(COLS * TILE_SIZE), static_cast<unsigned int>(ROWS * TILE_SIZE) }), "Bomberman Klonu v0.3 - Sınıflar ve Patlama");
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    Player player(1, 1);
    Bomb myBomb;

    
    std::vector<Explosion> activeExplosions;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::W) {
                    player.move(0, -1);
                }
                else if (keyPressed->code == sf::Keyboard::Key::Down || keyPressed->code == sf::Keyboard::Key::S) {
                    player.move(0, 1);
                }
                else if (keyPressed->code == sf::Keyboard::Key::Left || keyPressed->code == sf::Keyboard::Key::A) {
                    player.move(-1, 0);
                }
                else if (keyPressed->code == sf::Keyboard::Key::Right || keyPressed->code == sf::Keyboard::Key::D) {
                    player.move(1, 0);
                }
                else if (keyPressed->code == sf::Keyboard::Key::Space) {
                    myBomb.place(player.getX(), player.getY());
                }
            }
        }

        
        myBomb.checkExplosion(activeExplosions);

        
        for (int i = 0; i < activeExplosions.size(); i++) {
            if (activeExplosions[i].isExpired()) {
                activeExplosions.erase(activeExplosions.begin() + i);
                i--;
            }
        }

        
        for (int i = 0; i < activeExplosions.size(); i++) {
            if (player.getX() == activeExplosions[i].getX() && player.getY() == activeExplosions[i].getY()) {
                
                player.setPosition(1, 1);
            }
        }

       
        player.updateVisuals();
        myBomb.updateVisuals();

        
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

        
        for (int i = 0; i < activeExplosions.size(); i++) {
            activeExplosions[i].draw(window);
        }

       
        myBomb.draw(window);
        player.draw(window);

        window.display();
    }

    return 0;
}