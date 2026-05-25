#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <optional>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>

const int ROWS = 15;
const int COLS = 15;
const float TILE_SIZE = 40.0f;

enum class GameState {
    Menu,
    Playing,
    GameOver
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

const int originalMap[ROWS][COLS] = {
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
    int x_grid, y_grid;
    sf::RectangleShape body;
    int animationFrame;
    sf::Clock animationTimer;

public:
    Player(int startX, int startY) {
        x_grid = startX;
        y_grid = startY;
        animationFrame = 0;
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
            if (animationTimer.getElapsedTime().asSeconds() > 0.1f) {
                animationFrame = (animationFrame + 1) % 4;
                animationTimer.restart();
            }
        }
    }

    void updateVisuals() {
        body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
    }

    void draw(sf::RenderWindow& window) { window.draw(body); }
    void setPosition(int x, int y) { x_grid = x; y_grid = y; }
    int getX() { return x_grid; }
    int getY() { return y_grid; }
};

class Enemy {
private:
    int x_grid, y_grid;
    int dx, dy;
    sf::Clock moveTimer;
    sf::RectangleShape body;

public:
    Enemy(int startX, int startY, int startDx, int startDy) {
        x_grid = startX;
        y_grid = startY;
        dx = startDx;
        dy = startDy;
        body.setSize(sf::Vector2f(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f));
        body.setFillColor(sf::Color::Magenta);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.4f, TILE_SIZE * 0.4f));
    }

    void patrol() {
        if (moveTimer.getElapsedTime().asSeconds() >= 0.3f) {
            int nextX = x_grid + dx;
            int nextY = y_grid + dy;

            if (gameMap[nextY][nextX] == 0) {
                x_grid = nextX;
                y_grid = nextY;
            }
            else {
                dx = -dx;
                dy = -dy;
            }
            moveTimer.restart();
        }
    }

    void updateVisuals() {
        body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
    }

    void draw(sf::RenderWindow& window) { window.draw(body); }
    int getX() { return x_grid; }
    int getY() { return y_grid; }
};

class Explosion {
public:
    int x, y;
    sf::Clock timer;
    sf::RectangleShape body;

    Explosion(int gridX, int gridY) {
        x = gridX;
        y = gridY;
        body.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        body.setFillColor(sf::Color(255, 69, 0));
        body.setPosition(sf::Vector2f(x * TILE_SIZE, y * TILE_SIZE));
    }
};

class Bomb {
private:
    bool active;
    int x_grid, y_grid;
    sf::Clock timer;
    sf::RectangleShape body;
    sf::Clock pulseTimer;

public:
    Bomb() {
        active = false;
        x_grid = 0; y_grid = 0;
        body.setSize(sf::Vector2f(TILE_SIZE * 0.7f, TILE_SIZE * 0.7f));
        body.setFillColor(sf::Color::Red);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.35f, TILE_SIZE * 0.35f));
    }

    void place(int x, int y) {
        if (!active) {
            active = true;
            x_grid = x;
            y_grid = y;
            timer.restart();
            pulseTimer.restart();
        }
    }

    void checkExplosion(int& score, std::vector<Explosion>& explosions) {
        if (active && timer.getElapsedTime().asSeconds() >= 3.0f) {
            active = false;

            explosions.push_back(Explosion(x_grid, y_grid));

            int dx[] = { 1, -1, 0, 0 };
            int dy[] = { 0, 0, 1, -1 };

            for (int d = 0; d < 4; d++) {
                int nx = x_grid + dx[d];
                int ny = y_grid + dy[d];

                if (gameMap[ny][nx] == 1) continue;

                explosions.push_back(Explosion(nx, ny));

                if (gameMap[ny][nx] == 2) {
                    gameMap[ny][nx] = 0;
                    score += 10;
                }
            }
        }
    }

    void updateVisuals() {
        if (active) {
            body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
            float time = pulseTimer.getElapsedTime().asSeconds();
            float scale = 1.0f + 0.1f * std::sin(time * 10.0f);
            body.setScale(sf::Vector2f(scale, scale));
        }
    }

    void draw(sf::RenderWindow& window) { if (active) window.draw(body); }
    bool isActive() { return active; }
    void reset() { active = false; }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(COLS * TILE_SIZE), static_cast<unsigned int>((ROWS + 2) * TILE_SIZE) }), "Bomberman Klonu v0.5");
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    GameState currentState = GameState::Menu;

    Player player(1, 1);
    Bomb myBomb;
    std::vector<Enemy> enemies;
    std::vector<Explosion> activeExplosions;
    int playerScore = 0;

    sf::RectangleShape startButton(sf::Vector2f(200.0f, 60.0f));
    startButton.setFillColor(sf::Color::Blue);
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOutlineThickness(2.0f);
    startButton.setPosition(sf::Vector2f((COLS * TILE_SIZE) / 2.0f - 100.0f, 300.0f));

    auto resetGame = [&]() {
        player.setPosition(1, 1);
        myBomb.reset();
        activeExplosions.clear();
        playerScore = 0;
        enemies.clear();
        enemies.push_back(Enemy(5, 5, 1, 0));
        enemies.push_back(Enemy(13, 1, 0, 1));

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                gameMap[i][j] = originalMap[i][j];
            }
        }
        };

    resetGame();

    sf::Font font;
    std::optional<sf::Text> menuText, buttonText, gameplayText, gameOverText;

    if (font.openFromFile("assets/fonts/arial.ttf")) {
        menuText.emplace(font, "BOMBERMAN KLONU", 32);
        menuText->setFillColor(sf::Color::Yellow);
        menuText->setPosition(sf::Vector2f((COLS * TILE_SIZE) / 2.0f - 140.0f, 150.0f));

        buttonText.emplace(font, "BASLAT (ENTER)", 20);
        buttonText->setFillColor(sf::Color::White);
        buttonText->setPosition(sf::Vector2f((COLS * TILE_SIZE) / 2.0f - 75.0f, 315.0f));

        gameplayText.emplace(font, "Skor: 0", 24);
        gameplayText->setFillColor(sf::Color::White);
        gameplayText->setPosition(sf::Vector2f(10.0f, ROWS * TILE_SIZE + 10.0f));

        gameOverText.emplace(font, "OYUN BITTI!\n\nYeniden baslamak icin ENTER", 24);
        gameOverText->setFillColor(sf::Color::Red);
        gameOverText->setPosition(sf::Vector2f(40.0f, 200.0f));
    }

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                if (currentState == GameState::Menu) {
                    if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        currentState = GameState::Playing;
                    }
                }
                else if (currentState == GameState::Playing) {
                    if (keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::W) player.move(0, -1);
                    else if (keyPressed->code == sf::Keyboard::Key::Down || keyPressed->code == sf::Keyboard::Key::S) player.move(0, 1);
                    else if (keyPressed->code == sf::Keyboard::Key::Left || keyPressed->code == sf::Keyboard::Key::A) player.move(-1, 0);
                    else if (keyPressed->code == sf::Keyboard::Key::Right || keyPressed->code == sf::Keyboard::Key::D) player.move(1, 0);
                    else if (keyPressed->code == sf::Keyboard::Key::Space) myBomb.place(player.getX(), player.getY());
                }
                else if (currentState == GameState::GameOver) {
                    if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        resetGame();
                        currentState = GameState::Playing;
                    }
                }
            }
        }

        if (currentState == GameState::Playing) {
            myBomb.checkExplosion(playerScore, activeExplosions);

            for (size_t i = 0; i < activeExplosions.size(); i++) {
                if (activeExplosions[i].timer.getElapsedTime().asSeconds() > 0.4f) {
                    activeExplosions.erase(activeExplosions.begin() + i);
                    i--;
                }
            }

            for (size_t i = 0; i < activeExplosions.size(); i++) {
                if (player.getX() == activeExplosions[i].x && player.getY() == activeExplosions[i].y) {
                    currentState = GameState::GameOver;
                }
            }

            for (size_t i = 0; i < enemies.size(); i++) {
                enemies[i].patrol();
                enemies[i].updateVisuals();

                if (player.getX() == enemies[i].getX() && player.getY() == enemies[i].getY()) {
                    currentState = GameState::GameOver;
                }

                bool enemyDied = false;
                for (size_t j = 0; j < activeExplosions.size(); j++) {
                    if (enemies[i].getX() == activeExplosions[j].x && enemies[i].getY() == activeExplosions[j].y) {
                        enemyDied = true;
                        playerScore += 50;
                        break;
                    }
                }

                if (enemyDied) {
                    enemies.erase(enemies.begin() + i);
                    i--;
                }
            }

            player.updateVisuals();
            myBomb.updateVisuals();

            if (gameplayText.has_value()) {
                gameplayText->setString("Skor: " + std::to_string(playerScore));
            }
        }

        window.clear();

        if (currentState == GameState::Menu) {
            window.draw(startButton);
            if (menuText.has_value()) window.draw(menuText.value());
            if (buttonText.has_value()) window.draw(buttonText.value());
        }
        else if (currentState == GameState::Playing) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    tile.setPosition(sf::Vector2f(j * TILE_SIZE, i * TILE_SIZE));
                    if (gameMap[i][j] == 1) tile.setFillColor(sf::Color(128, 128, 128));
                    else if (gameMap[i][j] == 2) tile.setFillColor(sf::Color(139, 69, 19));
                    else tile.setFillColor(sf::Color(34, 139, 34));
                    window.draw(tile);
                }
            }

            for (size_t i = 0; i < activeExplosions.size(); i++) {
                window.draw(activeExplosions[i].body);
            }

            myBomb.draw(window);

            for (size_t i = 0; i < enemies.size(); i++) {
                enemies[i].draw(window);
            }

            player.draw(window);
            if (gameplayText.has_value()) window.draw(gameplayText.value());
        }
        else if (currentState == GameState::GameOver) {
            if (gameOverText.has_value()) window.draw(gameOverText.value());
        }

        window.display();
    }

    return 0;
}