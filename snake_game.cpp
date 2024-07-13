#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>

#define DELAY 100000 // Adjust snake speed by changing DELAY value
#define INITIAL_TAIL_LENGTH 5

enum class Direction { LEFT, RIGHT, UP, DOWN };

struct Point {
    int x;
    int y;
};

int maxX = 800; // Adjust window size as needed
int maxY = 600;
int tailLength = INITIAL_TAIL_LENGTH;
int score = 0;
bool gameOver = false;
Direction currentDir = Direction::RIGHT;
Point snakeParts[255] = {};
Point food;

void createFood() {
    food.x = rand() % (maxX / 10) * 10; // Food positions in multiples of 10
    food.y = rand() % (maxY / 10) * 10;
}

void shiftSnake() {
    for (int i = tailLength - 1; i > 0; --i) {
        snakeParts[i] = snakeParts[i - 1];
    }
}

bool checkCollision(int x, int y) {
    // Check collision with walls
    if (x >= maxX || x < 0 || y >= maxY || y < 0) {
        return true;
    }

    // Check collision with snake itself
    for (int i = 1; i < tailLength; ++i) {
        if (snakeParts[i].x == x && snakeParts[i].y == y) {
            return true;
        }
    }

    return false;
}

void updateSnakePosition() {
    int nextX = snakeParts[0].x;
    int nextY = snakeParts[0].y;

    // Update head position based on current direction
    if (currentDir == Direction::RIGHT) {
        nextX += 10;
    } else if (currentDir == Direction::LEFT) {
        nextX -= 10;
    } else if (currentDir == Direction::UP) {
        nextY -= 10;
    } else if (currentDir == Direction::DOWN) {
        nextY += 10;
    }

    // Check collision
    if (checkCollision(nextX, nextY)) {
        gameOver = true;
        return;
    }

    // Move snake
    shiftSnake();
    snakeParts[0].x = nextX;
    snakeParts[0].y = nextY;

    // Check if snake eats food
    if (snakeParts[0].x == food.x && snakeParts[0].y == food.y) {
        // Increase score and tail length
        score += 10;
        tailLength++;
        createFood();
    }
}

void draw(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);

    // Draw snake
    for (int i = 0; i < tailLength; ++i) {
        sf::RectangleShape snakePart(sf::Vector2f(10, 10)); // Each part is 10x10
        snakePart.setPosition(snakeParts[i].x, snakeParts[i].y);
        snakePart.setFillColor(sf::Color::Green);
        window.draw(snakePart);
    }

    // Draw food
    sf::RectangleShape foodItem(sf::Vector2f(10, 10));
    foodItem.setPosition(food.x, food.y);
    foodItem.setFillColor(sf::Color::Red);
    window.draw(foodItem);

    // Draw score
    sf::Font font;
    font.loadFromFile("arial.ttf"); // Load a font, adjust path as necessary
    sf::Text scoreText("Score: " + std::to_string(score), font, 20);
    scoreText.setPosition(10, 10);
    scoreText.setFillColor(sf::Color::White);
    window.draw(scoreText);

    // Draw game over message if game is over
    if (gameOver) {
        sf::Text gameOverText("Game Over!", font, 50);
        gameOverText.setPosition(maxX / 2 - 150, maxY / 2 - 25);
        gameOverText.setFillColor(sf::Color::White);
        window.draw(gameOverText);
    }

    window.display();
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(maxX, maxY), "SFML Snake Game");
    window.setFramerateLimit(15); // Limit frame rate to 15 FPS

    // Initialize snake position and food
    snakeParts[0] = {maxX / 2, maxY / 2}; // Start in the middle
    createFood();

    // Main game loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                // Handle key presses to change direction
                if (event.key.code == sf::Keyboard::Right && currentDir != Direction::LEFT) {
                    currentDir = Direction::RIGHT;
                } else if (event.key.code == sf::Keyboard::Left && currentDir != Direction::RIGHT) {
                    currentDir = Direction::LEFT;
                } else if (event.key.code == sf::Keyboard::Up && currentDir != Direction::DOWN) {
                    currentDir = Direction::UP;
                } else if (event.key.code == sf::Keyboard::Down && currentDir != Direction::UP) {
                    currentDir = Direction::DOWN;
                }
            }
        }

        // Update game state
        if (!gameOver) {
            updateSnakePosition();
        }

        // Render game
        draw(window);

        // Delay to control snake speed
        sf::sleep(sf::microseconds(DELAY));
    }

    return 0;
}
