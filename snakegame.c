{\rtf1\ansi\ansicpg1252\cocoartf2513
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 ArialMT;\f1\froman\fcharset0 Times-Roman;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;}
{\*\expandedcolortbl;;\cssrgb\c0\c0\c0;}
\margl1440\margr1440\vieww10800\viewh8400\viewkind0
\deftab720
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \expnd0\expndtw0\kerning0
\outl0\strokewidth0 \strokec2 #include <stdio.h>
\f1\fs24 \

\f0\fs29\fsmilli14667 #include <fcntl.h>
\f1\fs24 \

\f0\fs29\fsmilli14667 #include <sys/mman.h>
\f1\fs24 \

\f0\fs29\fsmilli14667 #include <signal.h>
\f1\fs24 \

\f0\fs29\fsmilli14667 #include <ncurses.h>
\f1\fs24 \

\f0\fs29\fsmilli14667 #include <stdlib.h>
\f1\fs24 \

\f0\fs29\fsmilli14667 #include <unistd.h>
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 #define BOARD_WIDTH\'a0 50
\f1\fs24 \

\f0\fs29\fsmilli14667 #define BOARD_HEIGHT 20
\f1\fs24 \

\f0\fs29\fsmilli14667 #define SNAKE_SIZE \'a0 100
\f1\fs24 \

\f0\fs29\fsmilli14667 #define FOOD_SYMBOL\'a0 '*'
\f1\fs24 \

\f0\fs29\fsmilli14667 #define SNAKE_SYMBOL 'O'
\f1\fs24 \

\f0\fs29\fsmilli14667 #define BORDER_SYMBOL '+'
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 #define HW_REGS_BASE ( 0xff200000 )
\f1\fs24 \

\f0\fs29\fsmilli14667 #define HW_REGS_SPAN ( 0x00200000 )
\f1\fs24 \

\f0\fs29\fsmilli14667 #define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
\f1\fs24 \

\f0\fs29\fsmilli14667 #define BUTTON_PIO_BASE 0x50
\f1\fs24 \

\f0\fs29\fsmilli14667 #define HEX_PIO_BASE 0x20
\f1\fs24 \

\f0\fs29\fsmilli14667 #define BORDER_COLOR_PAIR 1
\f1\fs24 \

\f0\fs29\fsmilli14667 #define SNAKE_COLOR_PAIR 2
\f1\fs24 \

\f0\fs29\fsmilli14667 #define FOOD_COLOR_PAIR 3
\f1\fs24 \

\f0\fs29\fsmilli14667 #define GAME_OVER_COLOR_PAIR 4
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\
\
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 volatile sig_atomic_t stop;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void catchSIGINT(int signum)\{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0stop = 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void drawGameOver() \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0attron(COLOR_PAIR(GAME_OVER_COLOR_PAIR));
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0mvprintw(BOARD_HEIGHT / 2, (BOARD_WIDTH - 9) / 2, "Game Over!");
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0attroff(COLOR_PAIR(GAME_OVER_COLOR_PAIR));
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0refresh();
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 int askRestart() \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0mvprintw(BOARD_HEIGHT / 2 + 1, (BOARD_WIDTH - 25) / 2, "Do you want to play again? (Y/N)");
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0refresh();
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0int input;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0while (1) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0input = getch();
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0if (input == 'Y' || input == 'y' || input == 'N' || input == 'n') \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0unsigned int buttonValue;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0readButton(&buttonValue);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0if (buttonValue & 0x1) \{\'a0 // Assuming KEY_RIGHT is associated with the third bit
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return 0;\'a0 // No
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\} else if (buttonValue & 0x8) \{\'a0 // Assuming KEY_LEFT is associated with the second bit
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return 1;\'a0 // Yes
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0return (input == 'Y' || input == 'y');
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void initColors()\{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0start_color();
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0init_pair(BORDER_COLOR_PAIR, COLOR_BLUE, COLOR_BLACK);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0init_pair(SNAKE_COLOR_PAIR, COLOR_GREEN, COLOR_BLACK);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0init_pair(FOOD_COLOR_PAIR, COLOR_YELLOW, COLOR_BLACK);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0init_pair(GAME_OVER_COLOR_PAIR, COLOR_RED, COLOR_BLACK);
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\
\
\
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void readButton(unsigned int *buttonValue) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0// Open /dev/mem
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0int fd = open("/dev/mem", (O_RDWR | O_SYNC));
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0if (fd == -1) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0printf("ERROR: could not open \\"/dev/mem\\"...\\n");
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Get virtual addr that maps to physical
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0void *virtualBase = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, HW_REGS_BASE);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0if (virtualBase == MAP_FAILED) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0printf("ERROR: mmap() failed...\\n");
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0close(fd);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Get the address that maps to the buttons
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0volatile unsigned int *h2p_lw_button_addr = (unsigned int *)(virtualBase + ((BUTTON_PIO_BASE) & (HW_REGS_MASK)));
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Read the value from the pushbuttons
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0*buttonValue = *h2p_lw_button_addr;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0if (munmap(virtualBase, HW_REGS_SPAN) != 0) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0printf("ERROR: munmap() failed...\\n");
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0close(fd);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0close(fd);
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 typedef struct \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0int x, y;
\f1\fs24 \

\f0\fs29\fsmilli14667 \} Point;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 typedef struct \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0Point head;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0Point body[SNAKE_SIZE - 1];
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0int length;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0int direction;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0int score;
\f1\fs24 \

\f0\fs29\fsmilli14667 \} Snake;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 Point food;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void initGame(Snake *snake) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0// Initialize the snake
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0snake->head.x = BOARD_WIDTH / 2;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0snake->head.y = BOARD_HEIGHT / 2;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0snake->length = 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0snake->direction = KEY_RIGHT;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Initialize the food
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0food.x = rand() % (BOARD_WIDTH - 2) + 1;\'a0 // Avoid placing food on the border
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0food.y = rand() % (BOARD_HEIGHT - 2) + 1;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0snake->score = 0;
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void drawBoard(Snake *snake) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0clear();
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0//Count score
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0mvprintw(0, BOARD_WIDTH + 2, "Your score is: %d", snake->score);
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0attron(COLOR_PAIR(BORDER_COLOR_PAIR));
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0for( int i = 0; i < BOARD_WIDTH; i++)\{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(0,i, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(BOARD_HEIGHT - 1, i, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0for( int i = 0; i < BOARD_HEIGHT - 1; i++)\{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(i, 0, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(i, BOARD_WIDTH - 1, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0attroff(COLOR_PAIR(BORDER_COLOR_PAIR));
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0attron(COLOR_PAIR(SNAKE_COLOR_PAIR));
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(snake->head.y, snake->head.x, "%c", SNAKE_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0for( int i = 0; i < snake->length - 1; i++)\{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(snake->body[i].y, snake->body[i].x, "%c", SNAKE_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0attroff(COLOR_PAIR(SNAKE_COLOR_PAIR));
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0attron(COLOR_PAIR(FOOD_COLOR_PAIR));
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(food.y, food.x, "%c", FOOD_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0attroff(COLOR_PAIR(FOOD_COLOR_PAIR));
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0refresh();
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0/*Draw borders
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0for (int i = 0; i < BOARD_WIDTH; i++) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(0, i, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(BOARD_HEIGHT - 1, i, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0for (int i = 1; i < BOARD_HEIGHT - 1; i++) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(i, 0, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(i, BOARD_WIDTH - 1, "%c", BORDER_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Draw snake
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0mvprintw(snake->head.y, snake->head.x, "%c", SNAKE_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0for (int i = 0; i < snake->length-1; i++) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0mvprintw(snake->body[i].y, snake->body[i].x, "%c", SNAKE_SYMBOL);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Draw food
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0mvprintw(food.y, food.x, "%c", FOOD_SYMBOL);
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0refresh();*/
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void moveSnake(Snake *snake) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0// Move the body
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0for (int i = snake->length - 1; i > 0; i--) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->body[i] = snake->body[i - 1];
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0//new line - update body to the current pos of head
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0snake->body[0] = snake->head;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Move the head
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0switch (snake->direction) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_UP:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->head.y--;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_DOWN:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->head.y++;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_LEFT:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->head.x--;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_RIGHT:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->head.x++;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 void checkCollision(Snake *snake, int *gameOver) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0// Check for wall collision
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0if (snake->head.x <= 0 || snake->head.x >= BOARD_WIDTH - 1 ||
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->head.y <= 0 || snake->head.y >= BOARD_HEIGHT - 1) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0*gameOver = 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Check for self-collision
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0for (int i = 0; i < snake->length; i++) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0if (snake->head.x == snake->body[i].x && snake->head.y == snake->body[i].y) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0*gameOver = 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0return;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Check for food collision
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0if (snake->head.x == food.x && snake->head.y == food.y) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Snake eats the food
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->length++;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->score += 10;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Add the new head at the front of the body
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0for (int i = snake->length - 1; i > 0; i--) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->body[i] = snake->body[i - 1];
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake->body[0] = snake->head;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Generate new food position
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0food.x = rand() % (BOARD_WIDTH - 2) + 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0food.y = rand() % (BOARD_HEIGHT - 2) + 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 int main() \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0initscr(); // Initialize ncurses
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0curs_set(0); // Hide the cursor
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0keypad(stdscr, TRUE); // Enable special keys
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0nodelay(stdscr, TRUE); // Make getch non-blocking
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0noecho(); // Don't echo keypresses
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0initColors();
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0int restart = 1;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0while (restart) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Set up the initial game state
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0Snake snake;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0stop = 0;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0int gameOver = 0;
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0initGame(&snake);
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Game loop
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0while (!gameOver) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0int ch = getch();
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Handle user input
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0switch (ch) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_UP:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_DOWN:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_LEFT:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case KEY_RIGHT:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Change direction only if not opposite
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0if ((ch == KEY_UP && snake.direction != KEY_DOWN) ||
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0(ch == KEY_DOWN && snake.direction != KEY_UP) ||
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0(ch == KEY_LEFT && snake.direction != KEY_RIGHT) ||
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0(ch == KEY_RIGHT && snake.direction != KEY_LEFT)) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake.direction = ch;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case 'q':
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0gameOver = 1;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0unsigned int buttonValue;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0readButton(&buttonValue);
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0switch (buttonValue) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case 0x1:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// KEY[0] is pressed
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake.direction = KEY_RIGHT;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case 0x2:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// KEY[1] is pressed
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake.direction = KEY_DOWN;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case 0x4:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// KEY[2] is pressed
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake.direction = KEY_UP;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0case 0x8:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// KEY[3] is pressed
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0snake.direction = KEY_LEFT;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0default:
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0break;
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0moveSnake(&snake);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0checkCollision(&snake, &gameOver);
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0drawBoard(&snake);
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0if (gameOver) \{
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0drawGameOver();
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0refresh();
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Optionally, wait for a key press before displaying the restart prompt
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0usleep(500000);
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Ask the user if they want to restart
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0restart = askRestart();
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0// Delay for a short time
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0usleep(150000);\'a0 // Adjust the delay as needed for the desired speed
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\'a0\'a0\'a0\'a0\}
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0\}
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0// Clean up ncurses
\f1\fs24 \

\f0\fs29\fsmilli14667 \'a0\'a0\'a0\'a0endwin();
\f1\fs24 \
\pard\pardeftab720\partightenfactor0
\cf2 \
\pard\pardeftab720\sl400\partightenfactor0

\f0\fs29\fsmilli14667 \cf2 \'a0\'a0\'a0\'a0return 0;
\f1\fs24 \

\f0\fs29\fsmilli14667 \}
\f1\fs24 \
}