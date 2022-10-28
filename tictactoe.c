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
      // set background black
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);
      // set text colour white
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      for (int i = 0; i < N; i++)
      {
        SDL_RenderDrawLine(renderer,
                           0, i * CELL_HEIGHT,
                           SCREEN_WIDTH, i * CELL_HEIGHT);
      }
      // render text
      TTF_Init();
      TTF_Font *Sans = TTF_OpenFont("OpenSans-Regular.ttf", 24);
      if (Sans == NULL)
      {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
      }
      SDL_Color White = {255, 255, 255};
      // as TTF_RenderText_Solid could only be used on
      // SDL_Surface then you have to create the surface first
      SDL_Surface *menu1 =
          TTF_RenderText_Solid(Sans, "Player Vs Player", White);
      SDL_Surface *menu2 =
          TTF_RenderText_Solid(Sans, "Player Vs AI (Easy)", White);
      SDL_Surface *menu3 =
          TTF_RenderText_Solid(Sans, "Player Vs AI (Impossible)", White);
      // now you can convert it into a texture
      SDL_Texture *menu1Texture = SDL_CreateTextureFromSurface(renderer, menu1);
      SDL_Texture *menu2Texture = SDL_CreateTextureFromSurface(renderer, menu2);
      SDL_Texture *menu3Texture = SDL_CreateTextureFromSurface(renderer, menu3);

      SDL_Rect Menu1_rect;                  // create a rect
      Menu1_rect.x = SCREEN_WIDTH / 3 - 25; // controls the rect's x coordinate
      Menu1_rect.y = 20;                    // controls the rect's y coordinte
      Menu1_rect.w = 250;                   // controls the width of the rect
      Menu1_rect.h = 100;                   // controls the height of the rect

      SDL_Rect Menu2_rect;                  // create a rect
      Menu2_rect.x = SCREEN_WIDTH / 3 - 25; // controls the rect's x coordinate
      Menu2_rect.y = CELL_HEIGHT + 20;      // controls the rect's y coordinte
      Menu2_rect.w = 250;                   // controls the width of the rect
      Menu2_rect.h = 100;                   // controls the height of the rect

      SDL_Rect Menu3_rect;                  // create a rect
      Menu3_rect.x = SCREEN_WIDTH / 3 - 50; // controls the rect's x coordinate
      Menu3_rect.y = CELL_HEIGHT * 2 + 20;  // controls the rect's y coordinte
      Menu3_rect.w = 300;                   // controls the width of the rect
      Menu3_rect.h = 100;                   // controls the height of the rect

      SDL_RenderCopy(renderer, menu1Texture, NULL, &Menu1_rect);
      SDL_RenderCopy(renderer, menu2Texture, NULL, &Menu2_rect);
      SDL_RenderCopy(renderer, menu3Texture, NULL, &Menu3_rect);

      SDL_RenderPresent(renderer);

      // Don't forget to free your surface and texture
      SDL_FreeSurface(menu1);
      SDL_DestroyTexture(menu1Texture);
      SDL_FreeSurface(menu2);
      SDL_DestroyTexture(menu2Texture);
      SDL_FreeSurface(menu3);
      SDL_DestroyTexture(menu3Texture);

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

