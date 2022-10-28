#ifndef LOGIC_H_
#define LOGIC_H_
/* ^^ these are the include guards */

int processEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game);

int processMenuEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game);

void select_gamemode(int col, GameState *game);

void botMove(GameState *game);

void playerMove(GameState *game, int row, int column);

int win(const int board[9]);

void checkWin(GameState *game);

void click_on_cell(GameState *game, int row, int column);

char gridChar(int i);

void draw_in_terminal(int b[9]);

void computerMove(int board[9]);

int minimax(int board[9], int player);

int bestMove(int val, int depth, GameState *game);

void reset_board(GameState *game);

#endif