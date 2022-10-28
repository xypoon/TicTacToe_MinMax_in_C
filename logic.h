#ifndef LOGIC_H_
#define LOGIC_H_
/* ^^ these are the include guards */

/* Prototypes for the functions */
int processEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game);

int processMenuEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game);

void selectGamemode(int col, GameState *game);

void botMove(GameState *game);

void playerMove(GameState *game, int row, int column);

void checkWin(GameState *game);

char gridChar(int i);

void draw_in_terminal(int b[9]);

void computerMove(int board[9]);

int minimax(int board[9], int player);

int bestMove(int val, int depth, GameState *game);

void reset_board(GameState *game);

#endif