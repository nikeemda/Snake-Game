#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define BOARD_WIDTH  50
#define BOARD_HEIGHT 20
#define SNAKE_SIZE   100
#define FOOD_SYMBOL  '*'
#define SNAKE_SYMBOL 'O'
#define BORDER_SYMBOL '+'

#define HW_REGS_BASE ( 0xff200000 )
#define HW_REGS_SPAN ( 0x00200000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define BUTTON_PIO_BASE 0x50
#define HEX_PIO_BASE 0x20
#define BORDER_COLOR_PAIR 1
#define SNAKE_COLOR_PAIR 2
#define FOOD_COLOR_PAIR 3
#define GAME_OVER_COLOR_PAIR 4



volatile sig_atomic_t stop;

void catchSIGINT(int signum){
    stop = 1;
}

void drawGameOver() {
    attron(COLOR_PAIR(GAME_OVER_COLOR_PAIR));
    mvprintw(BOARD_HEIGHT / 2, (BOARD_WIDTH - 9) / 2, "Game Over!");
    attroff(COLOR_PAIR(GAME_OVER_COLOR_PAIR));
    refresh();
}

int askRestart() {
    mvprintw(BOARD_HEIGHT / 2 + 1, (BOARD_WIDTH - 25) / 2, "Do you want to play again? (Y/N)");
    refresh();

    int input;
    while (1) {
        input = getch();
        if (input == 'Y' || input == 'y' || input == 'N' || input == 'n') {
            break;
        }
        unsigned int buttonValue;
        readButton(&buttonValue);
        if (buttonValue & 0x1) {  // Assuming KEY_RIGHT is associated with the third bit
            return 0;  // No
        } else if (buttonValue & 0x8) {  // Assuming KEY_LEFT is associated with the second bit
            return 1;  // Yes
        }

    }
    return (input == 'Y' || input == 'y');
}

void initColors(){
        start_color();
        init_pair(BORDER_COLOR_PAIR, COLOR_BLUE, COLOR_BLACK);
        init_pair(SNAKE_COLOR_PAIR, COLOR_GREEN, COLOR_BLACK);
        init_pair(FOOD_COLOR_PAIR, COLOR_YELLOW, COLOR_BLACK);
        init_pair(GAME_OVER_COLOR_PAIR, COLOR_RED, COLOR_BLACK);
}




void readButton(unsigned int *buttonValue) {
    // Open /dev/mem
    int fd = open("/dev/mem", (O_RDWR | O_SYNC));
    if (fd == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return;
    }

    // Get virtual addr that maps to physical
    void *virtualBase = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
    if (virtualBase == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
        return;
    }

    // Get the address that maps to the buttons
    volatile unsigned int *h2p_lw_button_addr = (unsigned int *)(virtualBase + ((BUTTON_PIO_BASE) & (HW_REGS_MASK)));

    // Read the value from the pushbuttons
    *buttonValue = *h2p_lw_button_addr;

    if (munmap(virtualBase, HW_REGS_SPAN) != 0) {
        printf("ERROR: munmap() failed...\n");
        close(fd);
        return;
    }

    close(fd);
}

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point head;
    Point body[SNAKE_SIZE - 1];
    int length;
    int direction;
    int score;
} Snake;

Point food;

void initGame(Snake *snake) {
    // Initialize the snake
    snake->head.x = BOARD_WIDTH / 2;
    snake->head.y = BOARD_HEIGHT / 2;
    snake->length = 1;
    snake->direction = KEY_RIGHT;

    // Initialize the food
    food.x = rand() % (BOARD_WIDTH - 2) + 1;  // Avoid placing food on the border
    food.y = rand() % (BOARD_HEIGHT - 2) + 1;

    snake->score = 0;
}

void drawBoard(Snake *snake) {
    clear();

    //Count score
    mvprintw(0, BOARD_WIDTH + 2, "Your score is: %d", snake->score);

        attron(COLOR_PAIR(BORDER_COLOR_PAIR));
        for( int i = 0; i < BOARD_WIDTH; i++){
                mvprintw(0,i, "%c", BORDER_SYMBOL);
                mvprintw(BOARD_HEIGHT - 1, i, "%c", BORDER_SYMBOL);
        }

        for( int i = 0; i < BOARD_HEIGHT - 1; i++){
                mvprintw(i, 0, "%c", BORDER_SYMBOL);
                mvprintw(i, BOARD_WIDTH - 1, "%c", BORDER_SYMBOL);
        }

        attroff(COLOR_PAIR(BORDER_COLOR_PAIR));

        attron(COLOR_PAIR(SNAKE_COLOR_PAIR));
        mvprintw(snake->head.y, snake->head.x, "%c", SNAKE_SYMBOL);
        for( int i = 0; i < snake->length - 1; i++){
                mvprintw(snake->body[i].y, snake->body[i].x, "%c", SNAKE_SYMBOL);
        }
        attroff(COLOR_PAIR(SNAKE_COLOR_PAIR));

        attron(COLOR_PAIR(FOOD_COLOR_PAIR));
        mvprintw(food.y, food.x, "%c", FOOD_SYMBOL);
        attroff(COLOR_PAIR(FOOD_COLOR_PAIR));

        refresh();

    /*Draw borders
    for (int i = 0; i < BOARD_WIDTH; i++) {
        mvprintw(0, i, "%c", BORDER_SYMBOL);
        mvprintw(BOARD_HEIGHT - 1, i, "%c", BORDER_SYMBOL);
    }
    for (int i = 1; i < BOARD_HEIGHT - 1; i++) {
        mvprintw(i, 0, "%c", BORDER_SYMBOL);
        mvprintw(i, BOARD_WIDTH - 1, "%c", BORDER_SYMBOL);
    }

    // Draw snake
    mvprintw(snake->head.y, snake->head.x, "%c", SNAKE_SYMBOL);
    for (int i = 0; i < snake->length-1; i++) {
        mvprintw(snake->body[i].y, snake->body[i].x, "%c", SNAKE_SYMBOL);
    }

    // Draw food
    mvprintw(food.y, food.x, "%c", FOOD_SYMBOL);

    refresh();*/

}

void moveSnake(Snake *snake) {
    // Move the body
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    //new line - update body to the current pos of head
    snake->body[0] = snake->head;

    // Move the head
    switch (snake->direction) {
        case KEY_UP:
            snake->head.y--;
            break;
        case KEY_DOWN:
            snake->head.y++;
            break;
        case KEY_LEFT:
            snake->head.x--;
            break;
        case KEY_RIGHT:
            snake->head.x++;
            break;
    }
}

void checkCollision(Snake *snake, int *gameOver) {
    // Check for wall collision
    if (snake->head.x <= 0 || snake->head.x >= BOARD_WIDTH - 1 ||
        snake->head.y <= 0 || snake->head.y >= BOARD_HEIGHT - 1) {
        *gameOver = 1;
        return;
    }

    // Check for self-collision
    for (int i = 0; i < snake->length; i++) {
        if (snake->head.x == snake->body[i].x && snake->head.y == snake->body[i].y) {
            *gameOver = 1;
            return;
        }
    }

    // Check for food collision
    if (snake->head.x == food.x && snake->head.y == food.y) {
        // Snake eats the food
        snake->length++;
        snake->score += 10;


        // Add the new head at the front of the body
        for (int i = snake->length - 1; i > 0; i--) {
            snake->body[i] = snake->body[i - 1];
        }
        snake->body[0] = snake->head;

        // Generate new food position
        food.x = rand() % (BOARD_WIDTH - 2) + 1;
        food.y = rand() % (BOARD_HEIGHT - 2) + 1;
    }
}

int main() {
    initscr(); // Initialize ncurses
    curs_set(0); // Hide the cursor
    keypad(stdscr, TRUE); // Enable special keys
    nodelay(stdscr, TRUE); // Make getch non-blocking
    noecho(); // Don't echo keypresses
    initColors();
    int restart = 1;

    while (restart) {
        // Set up the initial game state
        Snake snake;
        stop = 0;
        int gameOver = 0;

        initGame(&snake);

        // Game loop
        while (!gameOver) {
            int ch = getch();

            // Handle user input
            switch (ch) {
                case KEY_UP:
                case KEY_DOWN:
                case KEY_LEFT:
                case KEY_RIGHT:
                    // Change direction only if not opposite
                    if ((ch == KEY_UP && snake.direction != KEY_DOWN) ||
                        (ch == KEY_DOWN && snake.direction != KEY_UP) ||
                        (ch == KEY_LEFT && snake.direction != KEY_RIGHT) ||
                        (ch == KEY_RIGHT && snake.direction != KEY_LEFT)) {
                        snake.direction = ch;
                    }
                    break;
                case 'q':
                    gameOver = 1;
                    break;
            }

            unsigned int buttonValue;
            readButton(&buttonValue);

            switch (buttonValue) {
                case 0x1:
                    // KEY[0] is pressed
                    snake.direction = KEY_RIGHT;
                    break;
                case 0x2:
                    // KEY[1] is pressed
                    snake.direction = KEY_DOWN;
                    break;
                case 0x4:
                    // KEY[2] is pressed
                    snake.direction = KEY_UP;
                    break;
                case 0x8:
                    // KEY[3] is pressed
                    snake.direction = KEY_LEFT;
                    break;
                default:
                    break;
            }

            moveSnake(&snake);
            checkCollision(&snake, &gameOver);
            drawBoard(&snake);

            if (gameOver) {
                drawGameOver();
                refresh();

                // Optionally, wait for a key press before displaying the restart prompt
                usleep(500000);

                // Ask the user if they want to restart
                restart = askRestart();
            }

            // Delay for a short time
            usleep(150000);  // Adjust the delay as needed for the desired speed
        }
    }

    // Clean up ncurses
    endwin();

    return 0;
}
