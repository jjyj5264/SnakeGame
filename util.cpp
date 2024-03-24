#include "console.h"
#include <ctime>
#include <string>
#include <cstring>

#define BOARD_SIZE 15
#define MOVE_DELAY 15
#define WALL_VERTICAL_STRING "┃"
#define WALL_HORIZONTAL_STRING "━"
#define WALL_RIGHT_TOP_STRING "┓"
#define WALL_LEFT_TOP_STRING "┏"
#define WALL_RIGHT_BOTTOM_STRING "┛"
#define WALL_LEFT_BOTTOM_STRING "┗"
#define SNAKE_STRING "■"
#define SNAKE_BODY_STRING "■"
#define APPLE_STRING "●"

void gameLogic();

void initializeSeed() {
    std::srand(std::time(0));
}

int x = 0, y = 0; // Coordinate of Snake's Head.
int tailX = 0, tailY = 0;
int appleX = 0, appleY = 0;
int score = 0;
int frame = 0;
bool isGameOver = false;
enum eInput { LEFT, RIGHT, UP, DOWN, NONE, ESC, ENTER };
eInput direction = RIGHT;
eInput input = NONE;

void recieveInput() {
  // Directions
  if (console::key(console::K_LEFT)) { direction = LEFT; }
  if (console::key(console::K_RIGHT)) { direction = RIGHT; }
  if (console::key(console::K_UP)) { direction = UP; }
  if (console::key(console::K_DOWN)) { direction = DOWN; }

  // Inputs
  if (console::key(console::K_ESC)){
    input = ESC;
    exit(0);
  }
  if (isGameOver && console::key(console::K_ENTER)) { input = ENTER; }
  if (console::key(console::K_NONE)) { input = NONE; }
}

void move() { // Moves only when !isGameOver.
  if (!isGameOver && frame % MOVE_DELAY == 0) {
    switch (direction) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }
  }
}

void restrictInScreen() {
  // x, y 위치를 화면의 최대 크기에서 벗어나지 않게 한다.
  if (x < 0) { x = 0; }
  if (x >= console::SCREEN_WIDTH) { x = console::SCREEN_WIDTH - 1; }
  if (y < 0) { y = 0; }
  if (y >= console::SCREEN_HEIGHT) { y = console::SCREEN_HEIGHT - 1; }
}

void initSnake() {
  x = BOARD_SIZE / 2;
  y = BOARD_SIZE / 2;
}

void initApple() {
  appleX = (std::rand() % 13) + 1; // 1 - 13
  appleY = (std::rand() % 13) + 1; // 1 - 13
}

void drawBoard() {
  // Draw walls
  console::draw(0, 0, WALL_LEFT_TOP_STRING);
  console::draw(BOARD_SIZE - 1, 0, WALL_RIGHT_TOP_STRING);
  console::draw(0, BOARD_SIZE - 1, WALL_LEFT_BOTTOM_STRING);
  console::draw(BOARD_SIZE - 1, BOARD_SIZE - 1, WALL_RIGHT_BOTTOM_STRING);

  for (int i = 1; i < BOARD_SIZE - 1; i++) {
    console::draw(i, 0, WALL_HORIZONTAL_STRING);
    console::draw(i, BOARD_SIZE - 1, WALL_HORIZONTAL_STRING);
  }

  for (int i = 1; i < BOARD_SIZE - 1; i++) {
    console::draw(0, i, WALL_VERTICAL_STRING);
    console::draw(BOARD_SIZE - 1, i, WALL_VERTICAL_STRING);
  }
  
  // Draw snake's head
  console::draw(x, y, SNAKE_STRING);

  // Draw snake's body
  // ...

  // Draw apple
  console::draw(appleX, appleY, APPLE_STRING);
}

void drawScore() {
  console::draw((BOARD_SIZE / 2) - 2, BOARD_SIZE, "Score: " + std::to_string(score));
}

void drawGameOver() {
  console::draw((BOARD_SIZE / 4), BOARD_SIZE / 2, "YOU LOSE!");
  console::draw((BOARD_SIZE / 8), (BOARD_SIZE / 2) + 1, "Try again? (Enter)");
}

void checkCollision() {
  // Walls
  if (x == 0 || x == BOARD_SIZE - 1 || y == 0 || y == BOARD_SIZE - 1) {
    isGameOver = true;
  }
}

void checkGameOver() {
  checkCollision();
  // etc...

  if (isGameOver) {
    drawGameOver();

    if (input == ENTER) {
      isGameOver = false;
      x = 0, y = 0;
      tailX = 0, tailY = 0;
      appleX = 0, appleY = 0;
      score = 0;
      frame = 0;
      isGameOver = false;
      direction = RIGHT;
      input = NONE;
      gameLogic(); // Restart the game.
    }
  }
}

// Game runs through this.
void gameLogic() {
  initializeSeed(); // For random seed
  console::init();
  initSnake(); // Snake
  initApple(); // Apple

  while (true) {
    // 화면을 지운다.
    console::clear();
    
    restrictInScreen();
    drawBoard(); // 1. Draw.
    recieveInput(); // 2. Recieve user input.
    checkGameOver(); // 3. Check if it's not Game Over.
    move(); // 4. If it's not, move.
    // checkEat();
    drawScore(); // 5. etc...

    // 화면을 갱신하고 다음 프레임까지 대기한다.
    console::wait();
    
    if (frame == 60) { // Just scared about overflow.
      frame = 0;
    }

    frame++;
  }
}
