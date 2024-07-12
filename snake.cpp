#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

#define DELAY 60000
#define TIMEOUT 12

enum class direction_type { LEFT, RIGHT, UP, DOWN };

struct point {
    int x;
    int y;
};

int x = 0, y = 0, maxY = 0, maxX = 0, nextX = 0, nextY = 0, tailLength = 5, score = 0;
bool gameOver = false;
direction_type currentDir = direction_type::RIGHT;
point snakeParts[255] = {};
point food;

void createFood() {
    food.x = (rand() % (maxX - 20)) + 10;
    food.y = (rand() % (maxY - 10)) + 5;
}

void drawPart(point drawPoint, const char* dot) {
    mvprintw(drawPoint.y, drawPoint.x, dot);
}

void cursesInit() {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    timeout(TIMEOUT);
    curs_set(FALSE);
    getmaxyx(stdscr, maxY, maxX);
}

void init() {
    srand(static_cast<unsigned int>(time(nullptr)));
    currentDir = direction_type::RIGHT;
    tailLength = 5;
    gameOver = false;
    score = 0;
    clear();

    int j = 0;
    for (int i = tailLength; i >= 0; i--) {
        point currPoint;
        currPoint.x = i;
        currPoint.y = maxY / 2;
        snakeParts[j] = currPoint;
        j++;
    }

    createFood();
    refresh();
}

void shiftSnake() {
    point tmp = snakeParts[tailLength - 1];
    for (int i = tailLength - 1; i > 0; i--) {
        snakeParts[i] = snakeParts[i - 1];
    }
    snakeParts[0] = tmp;
}

void drawScreen() {
    clear();
    if (gameOver)
        mvprintw(maxY / 2, maxX / 2, "Game Over!");

    for (int i = 0; i < tailLength; i++) {
        drawPart(snakeParts[i], "o");
    }

    drawPart(food, "O");
    mvprintw(1, 2, "Score: %i", score);
    refresh();
    usleep(DELAY);
}

int main(int argc, char* argv[]) {
    cursesInit();
    init();

    int ch;
    while (true) {
        getmaxyx(stdscr, maxY, maxX);
        if (gameOver) {
            sleep(2);
            init();
        }

        ch = getch();
        if ((ch == 'l' || ch == 'L' || ch == KEY_RIGHT) && (currentDir != direction_type::RIGHT && currentDir != direction_type::LEFT)) {
            currentDir = direction_type::RIGHT;
        } else if ((ch == 'h' || ch == 'H' || ch == KEY_LEFT) && (currentDir != direction_type::RIGHT && currentDir != direction_type::LEFT)) {
            currentDir = direction_type::LEFT;
        } else if ((ch == 'j' || ch == 'J' || ch == KEY_DOWN) && (currentDir != direction_type::UP && currentDir != direction_type::DOWN)) {
            currentDir = direction_type::DOWN;
        } else if ((ch == 'k' || ch == 'K' || ch == KEY_UP) && (currentDir != direction_type::UP && currentDir != direction_type::DOWN)) {
            currentDir = direction_type::UP;
        }

        nextX = snakeParts[0].x;
        nextY = snakeParts[0].y;

        if (currentDir == direction_type::RIGHT) nextX++;
        else if (currentDir == direction_type::LEFT) nextX--;
        else if (currentDir == direction_type::UP) nextY--;
        else if (currentDir == direction_type::DOWN) nextY++;

        if (nextX == food.x && nextY == food.y) {
            point tail;
            tail.x = nextX;
            tail.y = nextY;
            snakeParts[tailLength] = tail;

            if (tailLength < 255)
                tailLength++;
            else
                tailLength = 5;

            score += 5;
            createFood();
        } else {
            for (int i = 0; i < tailLength; i++) {
                if (nextX == snakeParts[i].x && nextY == snakeParts[i].y) {
                    gameOver = true;
                    break;
                }
            }

            snakeParts[tailLength - 1].x = nextX;
            snakeParts[tailLength - 1].y = nextY;
        }

        shiftSnake();

        if ((nextX >= maxX || nextX < 0) || (nextY >= maxY || nextY < 0)) {
            gameOver = true;
        }

        drawScreen();
    }

    endwin();
    nocbreak();
    return 0;
}
