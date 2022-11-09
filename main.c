// Compile Code
// As you add new SDL2 libraries, add their dll name to the compile command
// gcc -I src/include -L src/lib -g logic.c render.c -o main tictactoe.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

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
  TTF_Init();               // Initialise SDL2_TTF for Menu Text Generation

  // Create an application window with the following settings
  window = SDL_CreateWindow(
      "TicTacToe Game Window", // Window Title
      SDL_WINDOWPOS_UNDEFINED, // X-pos of window
      SDL_WINDOWPOS_UNDEFINED, // y-pos of window
      SCREEN_WIDTH,            // width, in pixels
      SCREEN_HEIGHT,           // width, in pixels
      0                        // flags
  );

  // Create renderer to render items created to
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  // Initialise GameState
    GameState gameState = {
        // Initilise Empty Board
        .board = {EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY},
        .playerTurn = PLAYER_X, // Set Player to go first
        .state = RUNNING_STATE, // Set State to running
        .turn = 1};             // Set default game turn 1

  // Event Loop
  int done = 0; // Initialise done = 0, if 1 means completed and will stop program
  int menu = 0; // Initialise menu = 0, if 1 means completed and will exit menu
  while (!done) // While loop to listen to events until user exits the game
  {
    if (menu == 0) // Check for state of menu, if menu = 0, render the menu
    {
      renderMenu(renderer); // Render the components of the menu

      while (menu == 0) // While loop to listen to events until user interacts with menu
      {

        menu = processMenuEvents(renderer, window, &gameState); // Listens to updates to menu value

        // Delay Refresh so as too not overun
        SDL_Delay(100);
      }
    }

    // Render Display Grid for Tic Tac Toe
    render_grid(renderer);

    // Render the game on the grid when there are updates to the board
    render_board(renderer,
                 gameState.board);

    //  Present the renderer
    SDL_RenderPresent(renderer);

    done = processEvents(renderer, window, &gameState); // Listen for events to update done when user exits/quit the program

    // Delay Refresh so as too not overun
    SDL_Delay(100);
  }

  // Shutdown the program
  SDL_DestroyWindow(window);
  SDL_Quit();
}
