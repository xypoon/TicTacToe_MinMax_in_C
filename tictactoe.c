// Compile Code
// As you add new SDL2 libraries, add their dll name to the compile command
// gcc -I src/include -L src/lib -o main tictactoe.c -lmingw32 -lSDL2main -lSDL2

//  Import Libraries
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Declare Variables
#define N 3

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O 2

#define RUNNING_STATE 0
#define PLAYER_X_WON_STATE 1
#define PLAYER_O_WON_STATE 2
#define TIE_STATE 3
#define QUIT_STATE 4

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CELL_WIDTH (SCREEN_WIDTH / N)
#define CELL_HEIGHT (SCREEN_HEIGHT / N)

typedef struct
{
  int state;
  int playerTurn;
  int board[N * N];
} GameState;

// Declare Functions
void playerMove(GameState *game);

void DrawCircle(SDL_Renderer *renderer, int centreX, int centreY)
{
  static int radius = 50;
  const int diameter = (radius * 2);

  int x = (radius - 1);
  int y = 0;
  int tx = 1;
  int ty = 1;
  int error = (tx - diameter);

  while (x >= y)
  {
    // Each of the following renders an octant of the circle
    SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
    SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
    SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
    SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
    SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
    SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
    SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
    SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

    if (error <= 0)
    {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0)
    {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

void render_o(SDL_Renderer *renderer, int row, int column)
{
  const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
  const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
  const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  DrawCircle(renderer, center_x, center_y);
}

void render_x(SDL_Renderer *renderer, int row, int column)
{
  const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
  const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
  const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawLine(renderer,
                     center_x - half_box_side,
                     center_y - half_box_side,
                     center_x + half_box_side,
                     center_y + half_box_side);
  SDL_RenderDrawLine(renderer,
                     center_x + half_box_side,
                     center_y - half_box_side,
                     center_x - half_box_side,
                     center_y + half_box_side);
}

void render_grid(SDL_Renderer *renderer, GameState *game)
{
  // clear screen color to black
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  for (int i = 1; i < N; i++)
  {
    // set draw color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer,
                       i * CELL_WIDTH, 0,
                       i * CELL_WIDTH, SCREEN_HEIGHT);
    SDL_RenderDrawLine(renderer,
                       0, i * CELL_HEIGHT,
                       SCREEN_WIDTH, i * CELL_HEIGHT);
  }
}

void render_board(SDL_Renderer *renderer, const int *board)
{
  for (int i = 0; i < N; ++i)
  {
    for (int j = 0; j < N; ++j)
    {
      switch (board[i * N + j])
      {
      case PLAYER_X:
        render_x(renderer, i, j);
        break;

      case PLAYER_O:
        render_o(renderer, i, j);
        break;

      default:
      {
      }
      }
    }
  }
}

int win(const int board[9])
{
  // determines if a player has won, returns 0 otherwise.
  unsigned wins[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
  int i;
  for (i = 0; i < 8; ++i)
  {
    if (board[wins[i][0]] != 0 &&
        board[wins[i][0]] == board[wins[i][1]] &&
        board[wins[i][0]] == board[wins[i][2]])
      return board[wins[i][2]];
  }
  return 0;
}

void reset_board(GameState *game)
{
  game->playerTurn = PLAYER_X;
  game->state = RUNNING_STATE;
  for (int i = 0; i < N * N; ++i)
  {
    game->board[i] = EMPTY;
  }
}

int processEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game)
{
  // SDL Event Listener
  SDL_Event event;
  int done = 0;

  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    // Quiting Game
    case SDL_WINDOWEVENT_CLOSE:
    {
      if (window)
      {
        printf("close Pressed\n");
        SDL_DestroyWindow(window);
        window = NULL;
        done = 1;
      }
    }
    break;
    case SDL_KEYDOWN:
    {
      switch (event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
        printf("Escape Pressed\n");
        done = 1;
        break;
      }
    }
    break;
    case SDL_QUIT:
      // quit out of the game
      done = 1;
      break;
    case SDL_MOUSEBUTTONDOWN:
    {
      // printf("%d,%d\n", event.button.y, event.button.x);
      playerMove(game);
      // for (int x = 0; x < 9; x++)
      //{
      //   printf("%d", game->board[x]);
      // }

      draw(game->board);

      switch (win(game->board))
      {
      case 0:
        printf("A draw. How droll.\n");
        break;
      case 1:
        draw(game->board);
        printf("You lose.\n");
        break;
      case -1:
        printf("You win. Inconceivable!\n");
        break;
      }
      break;
    }
    }
  }
  return done;
}

void playerMove(GameState *game)
{
  static int box = 0;
  game->board[box] = game->playerTurn;
  if (game->playerTurn == PLAYER_O)
  {
    game->playerTurn = PLAYER_X;
  }
  else
  {
    game->playerTurn = PLAYER_O;
  }
  box++;
}

// Program Start
int main(int argc, char *argv[])
{
  SDL_Window *window;     // Declare a window
  SDL_Renderer *renderer; // Declare a renderer

  SDL_Init(SDL_INIT_VIDEO); // Initialise SDL2

  // Create an application window with the following settings
  window = SDL_CreateWindow(
      "TicTacToe Game Window", // Window Title
      SDL_WINDOWPOS_UNDEFINED, // X-pos of window
      SDL_WINDOWPOS_UNDEFINED, // y-pos of window
      SCREEN_WIDTH,            // width, in pixels
      SCREEN_HEIGHT,           // width, in pixels
      0                        // flags
  );

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // create board
  GameState gameState = {
      .board = {EMPTY, EMPTY, EMPTY,
                EMPTY, EMPTY, EMPTY,
                EMPTY, EMPTY, EMPTY},
      .playerTurn = PLAYER_X,
      .state = RUNNING_STATE};

  // Event Loop
  int done = 0;
  while (!done)
  {
    // Render Display Grid for Tic Tac Toe
    render_grid(renderer, &gameState);
    render_board(renderer,
                 gameState.board);

    SDL_RenderPresent(renderer);
    // Check for Events
    done = processEvents(renderer, window, &gameState);

    // Delay Refresh so as too not overun
    SDL_Delay(100);
  }

  // Shutdown
  SDL_DestroyWindow(window);
  SDL_Quit();
}

char gridChar(int i)
{
  switch (i)
  {
  case -1:
    return 'X';
  case 0:
    return ' ';
  case 1:
    return 'O';
  }
}

void draw(int b[9])
{
  printf(" %c | %c | %c\n", gridChar(b[0]), gridChar(b[1]), gridChar(b[2]));
  printf("---+---+---\n");
  printf(" %c | %c | %c\n", gridChar(b[3]), gridChar(b[4]), gridChar(b[5]));
  printf("---+---+---\n");
  printf(" %c | %c | %c\n\n", gridChar(b[6]), gridChar(b[7]), gridChar(b[8]));
}