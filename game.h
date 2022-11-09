#ifndef GAME_H_
#define GAME_H_
/* ^^ these are the include guards */

// Declare Variables
#define N 3

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O -1

#define RUNNING_STATE 0
#define PLAYER_X_WON_STATE 1
#define PLAYER_O_WON_STATE -1
#define TIE_STATE 3
//#define QUIT_STATE 4

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CELL_WIDTH (SCREEN_WIDTH / N)
#define CELL_HEIGHT (SCREEN_HEIGHT / N)

typedef struct
{
  int state;
  int playerTurn;
  int board[N * N];
  int turn;
  int gamemode;
} GameState;

#endif