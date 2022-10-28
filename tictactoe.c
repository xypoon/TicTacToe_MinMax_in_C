// Compile Code
// As you add new SDL2 libraries, add their dll name to the compile command
// gcc -I src/include -L src/lib -o main tictactoe.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

//  Import Libraries
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "./render.h"
#include "./game.h"
#include "./logic.h"

// Program Start
int main(int argc, char *argv[])
{
  SDL_Window *window;     // Declare a window
  SDL_Renderer *renderer; // Declare a renderer

  SDL_Init(SDL_INIT_VIDEO); // Initialise SDL2
  TTF_Init();

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
      .state = RUNNING_STATE,
      .turn = 1};

  // Event Loop
  int done = 0;
  int menu = 0;
  while (!done)
  {
    if (menu == 0)
    {
      renderMenu(renderer);

      while (menu == 0)
      {

        menu = processMenuEvents(renderer, window, &gameState);

        // Delay Refresh so as too not overun
        SDL_Delay(100);
      }
    }

    // Render Display Grid for Tic Tac Toe
    render_grid(renderer);
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
