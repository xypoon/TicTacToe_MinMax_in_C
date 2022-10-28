//  Import Libraries
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "render.h"
#include "game.h"
#include "logic.h"

// Declare Variables
#define N 3

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CELL_WIDTH (SCREEN_WIDTH / N)
#define CELL_HEIGHT (SCREEN_HEIGHT / N)

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O -1

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

void render_grid(SDL_Renderer *renderer)
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

void renderMenu(SDL_Renderer *renderer)
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
}