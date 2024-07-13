#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

// Constants
const int WIDTH = 800;
const int HEIGHT = 600;
const int CELL_SIZE = 20;

class Snake {
public:
    Snake() {
        reset();
    }

    void reset() {
        snake.clear();
        snake.push_back(sf::RectangleShape(sf::Vector2f(CELL_SIZE, CELL_SIZE)));
        snake[0].setFillColor(sf::Color::Green);
        snake[0].setPosition(WIDTH / 2, HEIGHT / 2);
        direction = sf::Vector2f(CELL_SIZE, 0);
    }

    void move() {
        for (size_t i = snake.size() - 1; i > 0; --i) {
            snake[i].setPosition(snake[i - 1].getPosition());
        }
        snake[0].move(direction);
    }

    void grow() {
        sf::RectangleShape newPart(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        newPart.setFillColor(sf::Color::Green);
        newPart.setPosition(snake[snake.size() - 1].getPosition());
        snake.push_back(newPart);
    }

    void setDirection(sf::Vector2f dir) {
        direction = dir;
    }

    const std::vector<sf::RectangleShape>& getSnake() const {
        return snake;
    }

    bool checkCollision() {
        // Check collision with the walls
        if (snake[0].getPosition().x < 0 || snake[0].getPosition().x >= WIDTH ||
            snake[0].getPosition().y < 0 || snake[0].getPosition().y >= HEIGHT) {
            return true;
        }

        // Check collision with itself (starting from index 4 to avoid immediate collisions after eating food)
        for (size_t i = 4; i < snake.size(); ++i) {
            if (snake[0].getGlobalBounds().intersects(snake[i].getGlobalBounds())) {
                return true;
            }
        }

        return false;
    }



private:
    std::vector<sf::RectangleShape> snake;
    sf::Vector2f direction;
};

class Food {
public:
    Food() {
        food.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        food.setFillColor(sf::Color::Red);
        respawn();
    }

    void respawn() {
        int x = (std::rand() % (WIDTH / CELL_SIZE)) * CELL_SIZE;
        int y = (std::rand() % (HEIGHT / CELL_SIZE)) * CELL_SIZE;
        food.setPosition(x, y);
    }

    const sf::RectangleShape& getFood() const {
        return food;
    }

private:
    sf::RectangleShape food;
};

int main() {
    std::srand(std::time(0));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game");

    Snake snake;
    Food food;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(32); 
    gameOverText.setFillColor(sf::Color::White); // Set the color to white
    gameOverText.setString("Game Over!\nPress Enter to Restart");

    // Calculate center position for horizontal alignment
    sf::FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(sf::Vector2f(WIDTH / 2.0f, HEIGHT / 2.0f));

    int score = 0;
    bool isGameOver = false;

    sf::Clock clock;
    float deltaTime = 0.0f;
    const float MOVE_INTERVAL = 0.1f;

    while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (isGameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            snake.reset();
            food.respawn();
            score = 0;
            isGameOver = false;
        }
    }

        if (!isGameOver) {
            // Handle direction changes
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) &&
                (snake.getSnake().size() == 1 || snake.getSnake()[0].getPosition().y - CELL_SIZE != snake.getSnake()[1].getPosition().y))
                snake.setDirection(sf::Vector2f(0, -CELL_SIZE));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) &&
                (snake.getSnake().size() == 1 || snake.getSnake()[0].getPosition().y + CELL_SIZE != snake.getSnake()[1].getPosition().y))
                snake.setDirection(sf::Vector2f(0, CELL_SIZE));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) &&
                (snake.getSnake().size() == 1 || snake.getSnake()[0].getPosition().x - CELL_SIZE != snake.getSnake()[1].getPosition().x))
                snake.setDirection(sf::Vector2f(-CELL_SIZE, 0));
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) &&
                (snake.getSnake().size() == 1 || snake.getSnake()[0].getPosition().x + CELL_SIZE != snake.getSnake()[1].getPosition().x))
                snake.setDirection(sf::Vector2f(CELL_SIZE, 0));

            deltaTime += clock.restart().asSeconds();

            if (deltaTime >= MOVE_INTERVAL) {
                snake.move();

                // Check if snake eats food
                if (snake.getSnake()[0].getGlobalBounds().intersects(food.getFood().getGlobalBounds())) {
                    snake.grow();
                    food.respawn();
                    score += 10;
                }

                // Check collision after moving and possibly growing
                if (snake.checkCollision()) {
                    isGameOver = true;
                }

                deltaTime = 0;
            }
        }

        window.clear();

        // Draw snake and food
        for (const auto& part : snake.getSnake()) {
            window.draw(part);
        }
        window.draw(food.getFood());

        // Draw score text
        std::stringstream ss;
        ss << "Score: " << score;
        scoreText.setString(ss.str());
        window.draw(scoreText);

        // Draw game over text if game is over
        if (isGameOver) {
            window.draw(gameOverText);
        }

        window.display();
    }


    return 0;
}