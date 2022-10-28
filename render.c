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

