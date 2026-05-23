#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <optional>
#include <vector>
#include <iostream>

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

// --- SINIFLAR ---

class Player {
private:
    int x_grid;
    int y_grid;
    sf::Texture playerTexture;
    std::optional<sf::Sprite> sprite;

public:
    Player(int startX, int startY) {
        x_grid = startX;
        y_grid = startY;

        if (playerTexture.loadFromFile("assets/images/player.png")) {
            sprite.emplace(playerTexture);
            sprite->setOrigin(sf::Vector2f(playerTexture.getSize().x / 2.0f, playerTexture.getSize().y / 2.0f));
            sprite->setScale(sf::Vector2f(0.8f, 0.8f));
        }
        else {
            std::cerr << "Oyuncu resmi yuklenemedi! (assets/images/player.png eksik)" << std::endl;
        }
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
        if (sprite.has_value()) {
            sprite->setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
        }
    }

    void draw(sf::RenderWindow& window) {
        if (sprite.has_value()) {
            window.draw(sprite.value());
        }
    }

    void setPosition(int x, int y) { x_grid = x; y_grid = y; }
    int getX() { return x_grid; }
    int getY() { return y_grid; }
};

class Bomb {
private:
    bool active;
    int x_grid, y_grid;
    sf::Clock timer;

    sf::Texture bombTexture;
    std::optional<sf::Sprite> sprite;

public:
    Bomb() {
        active = false;
        // UYARIYI BURADA ÇÖZDÜK: Başlangıçta x ve y'ye 0 değerini verdik
        x_grid = 0;
        y_grid = 0;

        if (bombTexture.loadFromFile("assets/images/bomb.png")) {
            sprite.emplace(bombTexture);
            sprite->setOrigin(sf::Vector2f(bombTexture.getSize().x / 2.0f, bombTexture.getSize().y / 2.0f));
            sprite->setScale(sf::Vector2f(0.8f, 0.8f));
        }
    }

    void place(int x, int y) {
        if (!active) {
            active = true;
            x_grid = x;
            y_grid = y;
            timer.restart();
        }
    }

    void checkExplosion(int& score) {
        if (active && timer.getElapsedTime().asSeconds() >= 3.0f) {
            active = false;

            int dx[] = { 1, -1, 0, 0 };
            int dy[] = { 0, 0, 1, -1 };

            for (int d = 0; d < 4; d++) {
                int nx = x_grid + dx[d];
                int ny = y_grid + dy[d];

                if (gameMap[ny][nx] == 2) {
                    gameMap[ny][nx] = 0;
                    score += 10;
                }
            }
        }
    }

    void updateVisuals() {
        if (active && sprite.has_value()) {
            sprite->setPosition(sf::Vector2f(x_grid * TILE_SIZE + TILE_SIZE / 2.0f, y_grid * TILE_SIZE + TILE_SIZE / 2.0f));
        }
    }

    void draw(sf::RenderWindow& window) {
        if (active && sprite.has_value()) {
            window.draw(sprite.value());
        }
    }
};

// --- ANA FONKSİYON ---

int main() {
    sf::RenderWindow window(sf::VideoMode({ static_cast<unsigned int>(COLS * TILE_SIZE), static_cast<unsigned int>((ROWS + 2) * TILE_SIZE) }), "Bomberman - Gorseller ve Skor");

    sf::Texture texGrass, texWall, texBox;
    texGrass.loadFromFile("assets/images/grass.png");
    texWall.loadFromFile("assets/images/wall.png");
    texBox.loadFromFile("assets/images/box.png");

    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    // Burada o hataya sebep olan gizli harf tamamen temizlendi.
    Player player(1, 1);
    Bomb myBomb;

    int playerScore = 0;
    sf::Font font;
    std::optional<sf::Text> scoreText;

    if (font.openFromFile("assets/fonts/arial.ttf")) {
        scoreText.emplace(font, "Skor: 0", 24);
        scoreText->setFillColor(sf::Color::White);
        scoreText->setPosition(sf::Vector2f(10.0f, ROWS * TILE_SIZE + 10.0f));
    }
    else {
        std::cerr << "Yazi tipi bulunamadi! (assets/fonts/arial.ttf eksik)" << std::endl;
    }

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

        myBomb.checkExplosion(playerScore);

        player.updateVisuals();
        myBomb.updateVisuals();

        if (scoreText.has_value()) {
            scoreText->setString("Skor: " + std::to_string(playerScore));
        }

        window.clear();

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                tile.setPosition(sf::Vector2f(j * TILE_SIZE, i * TILE_SIZE));

                if (gameMap[i][j] == 1) {
                    tile.setTexture(&texWall);
                }
                else if (gameMap[i][j] == 2) {
                    tile.setTexture(&texBox);
                }
                else {
                    tile.setTexture(&texGrass);
                }
                window.draw(tile);
            }
        }

        myBomb.draw(window);
        player.draw(window);

        if (scoreText.has_value()) {
            window.draw(scoreText.value());
        }

        window.display();
    }

    return 0;
}