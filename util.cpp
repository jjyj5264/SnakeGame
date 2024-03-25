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
#define SNAKE_BODY_STRING "▪"
#define APPLE_STRING "●"

void gameLogic();
void checkGameOver();
void drawSnake();

void initializeSeed() {
    std::srand(std::time(0));
}

int x = 0, y = 0; // Coordinate of Snake's Head.
int tailX[(BOARD_SIZE - 1) * (BOARD_SIZE - 1)];
int tailY[(BOARD_SIZE - 1) * (BOARD_SIZE - 1)];
int tailLength = 0;
int appleX = 0, appleY = 0;
int frame = 0;
bool isGameOver = false;
enum eInput { LEFT, RIGHT, UP, DOWN, NONE, ESC, ENTER };
eInput direction = RIGHT;
eInput input = NONE;
int tmp = 0;

void recieveInput() {
  // Directions
  if (console::key(console::K_LEFT) && direction != RIGHT) {
    direction = LEFT;
  }
  if (console::key(console::K_RIGHT) && direction != LEFT) {
    direction = RIGHT;
  }
  if (console::key(console::K_UP) && direction != DOWN) {
    direction = UP;
  }
  if (console::key(console::K_DOWN) && direction != UP) {
    direction = DOWN;
  }

  // Inputs
  if (console::key(console::K_ESC)){
    input = ESC;
    exit(0);
  }
  if (isGameOver && console::key(console::K_ENTER)) { input = ENTER; }
  if (console::key(console::K_NONE)) { input = NONE; }
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
  int avails = ((BOARD_SIZE - 2) * (BOARD_SIZE - 2)) - (tailLength + 1);
  int availCoords[2][avails];
  int index = 0;
  bool avail = false;

  // availCoords[0][0] = 3; // x, (3, 7)
  // availCoords[1][0] = 7; // y

  // x, y 동일 / tailX, tailY 동일 둘 중 하나라도 있으면 안 됨.

  int randInt = std::rand() % avails; // 0 to 167(at first time)
  // appleX = availCoords[0][0];
  // appleY = availCoords[1][0];
  
  appleX = availCoords[0][randInt];
  appleY = availCoords[1][randInt];
  tmp = avails;
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
  
  // Draw snake
  console::draw(x, y, SNAKE_STRING); // Head
  drawSnake(); // Tails

  // Draw apple
  console::draw(appleX, appleY, APPLE_STRING);
}

void drawSnake() {
  for (int i = 0; i < tailLength; i++) {
    console::draw(tailX[i], tailY[i], SNAKE_BODY_STRING);
  }

  console::draw(x, y, SNAKE_STRING);
}

void drawScore() {
  std::string score = "Score: " + std::to_string(tailLength * 10);
  console::draw((BOARD_SIZE / 2) - (score.length() / 2), BOARD_SIZE, score);
  console::draw((BOARD_SIZE / 2) - (score.length() / 2), BOARD_SIZE + 1, std::to_string(tmp));
}

void checkCollision() {
  // Walls
  if (x == 0 || x == BOARD_SIZE - 1 || y == 0 || y == BOARD_SIZE - 1) {
    isGameOver = true;
  }

  // Body cells
  for (int i = 0; i < tailLength; i++) {
    if (x == tailX[i] && y == tailY[i]) {
      isGameOver = true;
    }
  }
}

void drawGameWin() {
  console::draw((BOARD_SIZE / 4), BOARD_SIZE / 2, "YOU WIN!");
}

void checkGameWin() {
  if (tailLength == (BOARD_SIZE - 1) * (BOARD_SIZE - 1)) {
    drawGameWin();
  }
}

void drawGameOver() {
  console::draw((BOARD_SIZE / 4), BOARD_SIZE / 2, "YOU LOSE!");
  console::draw((BOARD_SIZE / 8), (BOARD_SIZE / 2) + 1, "Try again? (Enter)");
}

void checkGameOver() {
  checkCollision();
  // etc...

  if (isGameOver) {
    drawGameOver();

    if (input == ENTER) { // Reset
      isGameOver = false;
      x = 0, y = 0; appleX = 0, appleY = 0;
      tailLength = 0;
      frame = 0;
      direction = RIGHT; input = NONE;
      gameLogic(); // Restart the game.
    }
  }
}

void checkEat() {
  if (x == appleX && y == appleY) { // If it's true.
    initApple();
    tailLength++;
  }
}

void move() {
  if (!isGameOver && frame % MOVE_DELAY == 0) {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int pprevX, pprevY;
    tailX[0] = x; // Follow the head.
    tailY[0] = y;

    for (int i = 1; i < tailLength; i++) {
      pprevX = tailX[i];
      pprevY = tailY[i];
      tailX[i] = prevX;
      tailY[i] = prevY;
      prevX = pprevX;
      prevY = pprevY;
    }

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

  checkGameOver();
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
    drawBoard();
    recieveInput();
    move();
    checkEat();
    checkGameWin();
    drawScore();

    // 화면을 갱신하고 다음 프레임까지 대기한다.
    console::wait();
    
    if (frame == 60) { // Just scared about overflow.
      frame = 0;
    }

    frame++;
  }
}