#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <optional>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const int ROWS = 15;
const int COLS = 31;
const float TILE_SIZE = 40.0f;

enum class GameState {
    Menu,
    Playing,
    StageTransition,
    GameOver,
    GameWon
};

int gameMap[ROWS][COLS];

const int originalMap[ROWS][COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,2,0,2,2,0,0,0,2,0,2,0,0,2,0,0,0,2,2,0,0,0,2,2,0,0,0,1},
    {1,0,1,2,1,0,1,0,1,2,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,0,1,0,1},
    {1,0,2,2,0,0,0,2,2,2,0,0,0,2,2,0,0,2,2,0,0,2,2,2,0,0,2,2,0,0,1},
    {1,2,1,0,1,0,1,2,1,0,1,2,1,0,1,0,1,0,1,2,1,0,1,2,1,0,1,2,1,2,1},
    {1,0,0,0,0,2,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,2,0,0,0,2,0,0,1},
    {1,2,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,2,1},
    {1,0,0,2,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,0,2,2,2,0,0,1},
    {1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1},
    {1,0,2,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,1},
    {1,2,1,2,1,0,1,2,1,0,1,2,1,0,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1},
    {1,0,2,2,0,0,2,2,2,0,0,2,2,0,0,2,2,0,0,2,2,2,0,0,2,2,0,0,2,0,1},
    {1,0,1,0,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,2,1,0,1,0,1,0,1},
    {1,0,0,0,2,2,0,0,0,2,0,0,2,0,0,0,2,2,0,0,0,2,0,2,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

class Player {
private:
    int x_grid, y_grid;
    sf::RectangleShape body;
    int bombRange;
    sf::Color playerColor;
    sf::Clock bombCooldown;
    sf::Clock moveCooldown; 

public:
    Player(int startX, int startY, sf::Color color) {
        x_grid = startX;
        y_grid = startY;
        bombRange = 1;
        playerColor = color;
        body.setSize(sf::Vector2f(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f));
        body.setFillColor(playerColor);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.4f, TILE_SIZE * 0.4f));
        bombCooldown.restart();
        moveCooldown.restart(); 
    }

    void move(int dx, int dy) {
        
        if (moveCooldown.getElapsedTime().asSeconds() < 0.15f) {
            return;
        }

        int nextX = x_grid + dx;
        int nextY = y_grid + dy;

        if (gameMap[nextY][nextX] == 0 || gameMap[nextY][nextX] == 3) {
            x_grid = nextX;
            y_grid = nextY;
            if (gameMap[y_grid][x_grid] == 3) {
                bombRange++;
                gameMap[y_grid][x_grid] = 0;
            }
        }

        moveCooldown.restart(); 
    }

    bool canDropBomb() { return bombCooldown.getElapsedTime().asSeconds() > 0.5f; }
    void resetCooldown() { bombCooldown.restart(); }
    void updateVisuals() { body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f)); }
    void draw(sf::RenderWindow& window) { window.draw(body); }
    void setPosition(int x, int y) { x_grid = x; y_grid = y; }
    int getX() { return x_grid; }
    int getY() { return y_grid; }
    int getBombRange() { return bombRange; }
    void resetRange() { bombRange = 1; }
};

class Enemy {
private:
    int x_grid, y_grid;
    int dx, dy;
    float speed;
    sf::Clock moveTimer;
    sf::RectangleShape body;

public:
    Enemy(int startX, int startY, int startDx, int startDy, float spd) {
        x_grid = startX;
        y_grid = startY;
        dx = startDx;
        dy = startDy;
        speed = spd;
        body.setSize(sf::Vector2f(TILE_SIZE * 0.8f, TILE_SIZE * 0.8f));
        body.setFillColor(sf::Color::Magenta);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.4f, TILE_SIZE * 0.4f));
    }

    void patrol(int p1x, int p1y, int p2x, int p2y, bool isP2Active) {
        if (moveTimer.getElapsedTime().asSeconds() >= speed) {
            int targetX = p1x, targetY = p1y;

            if (isP2Active) {
                double dist1 = std::sqrt(std::pow(x_grid - p1x, 2) + std::pow(y_grid - p1y, 2));
                double dist2 = std::sqrt(std::pow(x_grid - p2x, 2) + std::pow(y_grid - p2y, 2));
                if (dist2 < dist1) { targetX = p2x; targetY = p2y; }
            }

            int targetDx = 0, targetDy = 0;
            if (x_grid < targetX) targetDx = 1; else if (x_grid > targetX) targetDx = -1;
            if (y_grid < targetY) targetDy = 1; else if (y_grid > targetY) targetDy = -1;

            if (targetDx != 0 && (gameMap[y_grid][x_grid + targetDx] == 0 || gameMap[y_grid][x_grid + targetDx] == 3)) {
                dx = targetDx; dy = 0;
            }
            else if (targetDy != 0 && (gameMap[y_grid + targetDy][x_grid] == 0 || gameMap[y_grid + targetDy][x_grid] == 3)) {
                dx = 0; dy = targetDy;
            }
            else {
                if (gameMap[y_grid + dy][x_grid + dx] != 0 && gameMap[y_grid + dy][x_grid + dx] != 3) {
                    dx = -dx; dy = -dy;
                }
            }

            int nextX = x_grid + dx, nextY = y_grid + dy;
            if (gameMap[nextY][nextX] == 0 || gameMap[nextY][nextX] == 3) {
                x_grid = nextX; y_grid = nextY;
            }
            moveTimer.restart();
        }
    }

    void updateVisuals() { body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f)); }
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
        x = gridX; y = gridY;
        body.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        body.setFillColor(sf::Color(255, 69, 0));
        body.setPosition(sf::Vector2f(x * TILE_SIZE, y * TILE_SIZE));
    }
};

class Bomb {
public:
    int x_grid, y_grid;
    int range;
    sf::Clock timer;
    sf::RectangleShape body;
    sf::Clock pulseTimer;

    Bomb(int x, int y, int r) {
        x_grid = x; y_grid = y; range = r;
        body.setSize(sf::Vector2f(TILE_SIZE * 0.7f, TILE_SIZE * 0.7f));
        body.setFillColor(sf::Color::Red);
        body.setOrigin(sf::Vector2f(TILE_SIZE * 0.35f, TILE_SIZE * 0.35f));
        timer.restart(); pulseTimer.restart();
    }

    void explode(int& score, std::vector<Explosion>& explosions) {
        explosions.push_back(Explosion(x_grid, y_grid));
        int dx[] = { 1, -1, 0, 0 }; int dy[] = { 0, 0, 1, -1 };

        for (int d = 0; d < 4; d++) {
            for (int r = 1; r <= range; r++) {
                int nx = x_grid + (dx[d] * r);
                int ny = y_grid + (dy[d] * r);
                if (gameMap[ny][nx] == 1) break;
                explosions.push_back(Explosion(nx, ny));
                if (gameMap[ny][nx] == 2) {
                    if ((rand() % 100) < 30) gameMap[ny][nx] = 3; else gameMap[ny][nx] = 0;
                    score += 10; break;
                }
                if (gameMap[ny][nx] == 3) { gameMap[ny][nx] = 0; break; }
            }
        }
    }

    void updateVisuals() {
        body.setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
        float time = pulseTimer.getElapsedTime().asSeconds();
        float scale = 1.0f + 0.1f * std::sin(time * 10.0f);
        body.setScale(sf::Vector2f(scale, scale));
    }
    void draw(sf::RenderWindow& window) { window.draw(body); }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));

    float screenWidth = 15 * TILE_SIZE;
    float screenHeight = (ROWS + 2) * TILE_SIZE;
    sf::VideoMode windowMode(sf::Vector2u(static_cast<unsigned int>(screenWidth), static_cast<unsigned int>(screenHeight)));
    sf::RenderWindow window(windowMode, "Bomberman");

    sf::View cameraView(sf::FloatRect({ 0.0f, 0.0f }, { screenWidth, screenHeight }));
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    GameState currentState = GameState::Menu;

    Player player1(1, 1, sf::Color::Blue);
    Player player2(29, 13, sf::Color::Green);
    bool isPlayer2Active = false;

    std::vector<Bomb> activeBombs;
    std::vector<Enemy> enemies;
    std::vector<Explosion> activeExplosions;

    int playerScore = 0;
    int currentStage = 1;
    sf::Clock transitionTimer;

    sf::RectangleShape startButton(sf::Vector2f(220.0f, 50.0f));
    startButton.setFillColor(sf::Color::Blue);
    startButton.setPosition(sf::Vector2f(screenWidth / 2.0f - 110.0f, 260.0f));

    sf::RectangleShape startButton2P(sf::Vector2f(220.0f, 50.0f));
    startButton2P.setFillColor(sf::Color::Green);
    startButton2P.setPosition(sf::Vector2f(screenWidth / 2.0f - 110.0f, 330.0f));

    auto initStage = [&]() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                gameMap[i][j] = originalMap[i][j];
            }
        }
        player1.setPosition(1, 1);
        player2.setPosition(29, 13);
        activeBombs.clear();
        activeExplosions.clear();
        enemies.clear();

        if (currentStage == 1) {
            enemies.push_back(Enemy(10, 5, 1, 0, 0.4f));
            enemies.push_back(Enemy(20, 9, 0, 1, 0.4f));
        }
        else if (currentStage == 2) {
            enemies.push_back(Enemy(8, 3, 1, 0, 0.35f));
            enemies.push_back(Enemy(22, 11, 0, 1, 0.35f));
            enemies.push_back(Enemy(15, 7, -1, 0, 0.35f));
            enemies.push_back(Enemy(28, 5, 0, -1, 0.35f));
        }
        else if (currentStage == 3) {
            enemies.push_back(Enemy(7, 3, 1, 0, 0.3f));
            enemies.push_back(Enemy(24, 11, 0, 1, 0.3f));
            enemies.push_back(Enemy(14, 5, -1, 0, 0.3f));
            enemies.push_back(Enemy(27, 9, 0, -1, 0.3f));
            enemies.push_back(Enemy(19, 7, 1, 0, 0.3f));
            enemies.push_back(Enemy(4, 11, 0, 1, 0.3f));
        }
        };

    auto fullResetGame = [&]() {
        currentStage = 1;
        playerScore = 0;
        player1.resetRange();
        player2.resetRange();
        initStage();
        };

    sf::Font font;
    std::optional<sf::Text> menuText, btn1Text, btn2Text, gameplayText, gameOverText, stageText, wonText;

    if (font.openFromFile("assets/fonts/arial.ttf")) {
        menuText.emplace(font, "BOMBERMAN MULTIPLAYER", 28);
        menuText->setFillColor(sf::Color::Yellow);
        menuText->setPosition(sf::Vector2f(screenWidth / 2.0f - 160.0f, 150.0f));

        btn1Text.emplace(font, "1 OYUNCU (ENTER)", 16);
        btn1Text->setFillColor(sf::Color::White);
        btn1Text->setPosition(sf::Vector2f(screenWidth / 2.0f - 80.0f, 272.0f));

        btn2Text.emplace(font, "2 OYUNCU (SHIFT)", 16);
        btn2Text->setFillColor(sf::Color::White);
        btn2Text->setPosition(sf::Vector2f(screenWidth / 2.0f - 80.0f, 342.0f));

        gameplayText.emplace(font, "Skor: 0", 24);
        gameplayText->setFillColor(sf::Color::White);
        gameplayText->setPosition(sf::Vector2f(10.0f, ROWS * TILE_SIZE + 10.0f));

        gameOverText.emplace(font, "OYUN BITTI!\n\nMenuye donmek icin ENTER", 24);
        gameOverText->setFillColor(sf::Color::Red);
        gameOverText->setPosition(sf::Vector2f(40.0f, 200.0f));

        stageText.emplace(font, "STAGE 1", 40);
        stageText->setFillColor(sf::Color::Yellow);
        stageText->setPosition(sf::Vector2f(screenWidth / 2.0f - 90.0f, 250.0f));

        wonText.emplace(font, "TEBRIKLER!\n\nOYUNU KAZANDINIZ!\n\nMenuye donmek icin ENTER", 24);
        wonText->setFillColor(sf::Color::Green);
        wonText->setPosition(sf::Vector2f(40.0f, 200.0f));
    }

    fullResetGame();

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (currentState == GameState::Menu) {
                    if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        isPlayer2Active = false; fullResetGame();
                        currentState = GameState::StageTransition; transitionTimer.restart();
                    }
                    else if (keyPressed->code == sf::Keyboard::Key::LShift || keyPressed->code == sf::Keyboard::Key::RShift) {
                        isPlayer2Active = true; fullResetGame();
                        currentState = GameState::StageTransition; transitionTimer.restart();
                    }
                }
                else if (currentState == GameState::Playing) {
                    if (keyPressed->code == sf::Keyboard::Key::W) player1.move(0, -1);
                    else if (keyPressed->code == sf::Keyboard::Key::S) player1.move(0, 1);
                    else if (keyPressed->code == sf::Keyboard::Key::A) player1.move(-1, 0);
                    else if (keyPressed->code == sf::Keyboard::Key::D) player1.move(1, 0);
                    else if (keyPressed->code == sf::Keyboard::Key::Space) {
                        if (player1.canDropBomb()) {
                            activeBombs.push_back(Bomb(player1.getX(), player1.getY(), player1.getBombRange()));
                            player1.resetCooldown();
                        }
                    }

                    if (isPlayer2Active) {
                        if (keyPressed->code == sf::Keyboard::Key::Up) player2.move(0, -1);
                        else if (keyPressed->code == sf::Keyboard::Key::Down) player2.move(0, 1);
                        else if (keyPressed->code == sf::Keyboard::Key::Left) player2.move(-1, 0);
                        else if (keyPressed->code == sf::Keyboard::Key::Right) player2.move(1, 0);
                        else if (keyPressed->code == sf::Keyboard::Key::RControl) {
                            if (player2.canDropBomb()) {
                                activeBombs.push_back(Bomb(player2.getX(), player2.getY(), player2.getBombRange()));
                                player2.resetCooldown();
                            }
                        }
                    }
                }
                else if (currentState == GameState::GameOver || currentState == GameState::GameWon) {
                    if (keyPressed->code == sf::Keyboard::Key::Enter) {
                        currentState = GameState::Menu;
                    }
                }
            }
        }

        if (currentState == GameState::StageTransition) {
            if (stageText.has_value()) stageText->setString("STAGE " + std::to_string(currentStage));
            if (transitionTimer.getElapsedTime().asSeconds() > 2.0f) currentState = GameState::Playing;

            cameraView.setCenter({ screenWidth / 2.0f, screenHeight / 2.0f });
            window.setView(cameraView);
        }

        if (currentState == GameState::Playing) {
            float player1PixelX = player1.getX() * TILE_SIZE + TILE_SIZE / 2.0f;

            float viewCenterX = player1PixelX;
            if (viewCenterX < screenWidth / 2.0f) viewCenterX = screenWidth / 2.0f;
            if (viewCenterX > (COLS * TILE_SIZE) - (screenWidth / 2.0f)) viewCenterX = (COLS * TILE_SIZE) - (screenWidth / 2.0f);

            cameraView.setCenter({ viewCenterX, screenHeight / 2.0f });
            window.setView(cameraView);

            for (size_t i = 0; i < activeBombs.size(); i++) {
                if (activeBombs[i].timer.getElapsedTime().asSeconds() >= 3.0f) {
                    activeBombs[i].explode(playerScore, activeExplosions);
                    activeBombs.erase(activeBombs.begin() + i); i--;
                }
            }

            for (size_t i = 0; i < activeExplosions.size(); i++) {
                if (activeExplosions[i].timer.getElapsedTime().asSeconds() > 0.4f) {
                    activeExplosions.erase(activeExplosions.begin() + i); i--;
                }
            }

            for (size_t i = 0; i < activeExplosions.size(); i++) {
                if (player1.getX() == activeExplosions[i].x && player1.getY() == activeExplosions[i].y) currentState = GameState::GameOver;
                if (isPlayer2Active && player2.getX() == activeExplosions[i].x && player2.getY() == activeExplosions[i].y) currentState = GameState::GameOver;
            }

            for (size_t i = 0; i < enemies.size(); i++) {
                enemies[i].patrol(player1.getX(), player1.getY(), player2.getX(), player2.getY(), isPlayer2Active);
                enemies[i].updateVisuals();

                if (player1.getX() == enemies[i].getX() && player1.getY() == enemies[i].getY()) currentState = GameState::GameOver;
                if (isPlayer2Active && player2.getX() == enemies[i].getX() && player2.getY() == enemies[i].getY()) currentState = GameState::GameOver;

                bool enemyDied = false;
                for (size_t j = 0; j < activeExplosions.size(); j++) {
                    if (enemies[i].getX() == activeExplosions[j].x && enemies[i].getY() == activeExplosions[j].y) {
                        enemyDied = true; playerScore += 50; break;
                    }
                }

                if (enemyDied) { enemies.erase(enemies.begin() + i); i--; }
            }

            if (enemies.empty() && activeExplosions.empty()) {
                currentStage++;
                if (currentStage > 3) currentState = GameState::GameWon;
                else {
                    initStage(); currentState = GameState::StageTransition; transitionTimer.restart();
                }
            }

            player1.updateVisuals();
            if (isPlayer2Active) player2.updateVisuals();
            for (size_t i = 0; i < activeBombs.size(); i++) activeBombs[i].updateVisuals();
            if (gameplayText.has_value()) gameplayText->setString("Skor: " + std::to_string(playerScore) + " | Stage: " + std::to_string(currentStage));
        }

        window.clear();

        if (currentState == GameState::Menu) {
            cameraView.setCenter({ screenWidth / 2.0f, screenHeight / 2.0f });
            window.setView(cameraView);
            window.draw(startButton); window.draw(startButton2P);
            if (menuText.has_value()) window.draw(menuText.value());
            if (btn1Text.has_value()) window.draw(btn1Text.value());
            if (btn2Text.has_value()) window.draw(btn2Text.value());
        }
        else if (currentState == GameState::StageTransition) {
            if (stageText.has_value()) window.draw(stageText.value());
        }
        else if (currentState == GameState::Playing) {
            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLS; j++) {
                    tile.setPosition(sf::Vector2f(j * TILE_SIZE, i * TILE_SIZE));
                    if (gameMap[i][j] == 1) tile.setFillColor(sf::Color(128, 128, 128));
                    else if (gameMap[i][j] == 2) tile.setFillColor(sf::Color(139, 69, 19));
                    else if (gameMap[i][j] == 3) tile.setFillColor(sf::Color::Yellow);
                    else tile.setFillColor(sf::Color(34, 139, 34));
                    window.draw(tile);
                }
            }

            for (size_t i = 0; i < activeExplosions.size(); i++) window.draw(activeExplosions[i].body);
            for (size_t i = 0; i < activeBombs.size(); i++) activeBombs[i].draw(window);
            for (size_t i = 0; i < enemies.size(); i++) enemies[i].draw(window);

            player1.draw(window);
            if (isPlayer2Active) player2.draw(window);

            window.setView(window.getDefaultView());
            if (gameplayText.has_value()) window.draw(gameplayText.value());
        }
        else if (currentState == GameState::GameOver) {
            window.setView(window.getDefaultView());
            if (gameOverText.has_value()) window.draw(gameOverText.value());
        }
        else if (currentState == GameState::GameWon) {
            window.setView(window.getDefaultView());
            if (wonText.has_value()) window.draw(wonText.value());
        }

        window.display();
    }

    return 0;
}