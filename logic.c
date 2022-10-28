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

int best_move;

int processEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game)
{
    // SDL Event Listener
    SDL_Event event;
    int done = 0;

    botMove(game);

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
            // By dividing the position of the reference location with the width/height of the cell, integer returns the floor value which can be used to reference the cell to plot to
            int row = event.button.y / CELL_HEIGHT;
            int col = event.button.x / CELL_WIDTH;
            if (game->state == RUNNING_STATE)
            {
                playerMove(game, row, col);
            }
            else
            {
                reset_board(game);
            }
            break;
        }
        }
    }
    return done;
}

int processMenuEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game)
{
    // SDL Event Listener
    SDL_Event event;
    int done = 0;
    int menu = 0;

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
                menu = 1;
            }
        }
        break;
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                printf("Escape Pressed\n");
                menu = 1;
                break;
            }
        }
        break;
        case SDL_QUIT:
            // quit out of the game
            menu = 1;
            break;
        case SDL_MOUSEBUTTONDOWN:
        {
            select_gamemode(event.button.y / CELL_HEIGHT, game);
            menu = 1;
            break;
        }
        }
    }
    return menu;
}

void select_gamemode(int col, GameState *game)
{
    switch (col)
    {
    case 0:
        game->gamemode = 0;
        printf("Game mode %d, Player vs Player Selected\n", game->gamemode);
        break;
    case 1:
        game->gamemode = 1;
        printf("Game mode %d, Player vs AI (Easy) Selected\n", game->gamemode);
        break;
    case 2:
        game->gamemode = 2;
        printf("Game mode %d, Player vs AI (Impossible) Selected\n", game->gamemode);
        break;
    }
}

void botMove(GameState *game)
{
    if (game->state == RUNNING_STATE && game->playerTurn == -1 && game->gamemode == 1)
    {
        // printf("here");
        computerMove(game->board);
        draw_in_terminal(game->board);

        checkWin(game);

        // Switch Player Turn
        if (game->playerTurn == PLAYER_O)
        {
            game->playerTurn = PLAYER_X;
        }
        else
        {
            game->playerTurn = PLAYER_O;
        }
    }
    else if (game->state == RUNNING_STATE && game->playerTurn == -1 && game->gamemode == 2)
    {
        bestMove(-1, 0, game);
        game->board[best_move] = PLAYER_O;
        draw_in_terminal(game->board);

        checkWin(game);

        // Switch Player Turn
        if (game->playerTurn == PLAYER_O)
        {
            game->playerTurn = PLAYER_X;
        }
        else
        {
            game->playerTurn = PLAYER_O;
        }
    }
    else
    {
        // Bot has nothing to do
    }
}

void playerMove(GameState *game, int row, int column)
{
    if (game->board[row * N + column] == EMPTY)
    {
        game->board[row * N + column] = game->playerTurn;
        // Draw in Console
        draw_in_terminal(game->board);

        // Check win condition
        checkWin(game);

        // Switch Player Turn
        if (game->playerTurn == PLAYER_O)
        {
            game->playerTurn = PLAYER_X;
        }
        else
        {
            game->playerTurn = PLAYER_O;
        }
    }
}

int win(const int board[9])
{

    // determines if a player has won return 1, returns 0 otherwise.
    unsigned wins[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
    int i;
    for (i = 0; i < 8; ++i)
    {
        if (board[wins[i][0]] != 0 &&
            board[wins[i][0]] == board[wins[i][1]] &&
            board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][0]];
    }
    return 0;
}

void checkWin(GameState *game)
{
    // Check win condition
    switch (win(game->board))
    {
    case 0:
        if (game->turn == 9 && win(game->board) != 1)
        {
            game->state = TIE_STATE;
            printf("A draw. How droll.\n");
        }
        else
        {
            game->turn++;
            printf("Not win, Now turn %d\n", game->turn);
        }
        break;
    case 1:
        if (game->playerTurn == 1)
        {
            game->state = PLAYER_X_WON_STATE;
            printf("turn: %d,player:%d\n", game->turn + 1, game->playerTurn);
            printf("Player X wins.\n");
        }
        else
        {
        }
        break;
    case -1:
        game->state = PLAYER_O_WON_STATE;
        printf("turn: %d,player:%d\n", game->turn + 1, game->playerTurn);
        printf("Player O wins.\n");
        break;
    }
}

void click_on_cell(GameState *game, int row, int column)
{
    if (game->state == RUNNING_STATE)
    {
        playerMove(game, row, column);
    }
    else
    {
        reset_board(game);
    }
}

char gridChar(int i)
{
    switch (i)
    {
    case 1:
        return 'X';
    case 0:
        return ' ';
    case 2:
        return 'O';
    }
}

void draw_in_terminal(int b[9])
{
    printf(" %c | %c | %c\n", gridChar(b[0]), gridChar(b[1]), gridChar(b[2]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n", gridChar(b[3]), gridChar(b[4]), gridChar(b[5]));
    printf("---+---+---\n");
    printf(" %c | %c | %c\n\n", gridChar(b[6]), gridChar(b[7]), gridChar(b[8]));
}

void computerMove(int board[9])
{
    int move = -1;
    int score = -2;
    int i;
    for (i = 0; i < 9; ++i)
    {
        if (board[i] == 0)
        {
            board[i] = 1;
            int tempScore = -minimax(board, -1);
            board[i] = 0;
            if (tempScore > score)
            {
                score = tempScore;
                move = i;
            }
        }
    }
    // returns a score based on minimax tree at a given node.
    board[move] = PLAYER_O;
}

int minimax(int board[9], int player)
{
    // How is the position like for player (their turn) on board?
    int winner = win(board);
    if (winner != 0)
        return winner * player;

    int move = -1;
    int score = -2; // Losing moves are preferred to no move
    int i;
    for (i = 0; i < 9; ++i)
    { // For all moves,
        if (board[i] == 0)
        {                      // If legal,
            board[i] = player; // Try the move
            int thisScore = -minimax(board, player * -1);
            if (thisScore > score)
            {
                score = thisScore;
                move = i;
            }             // Pick the one that's worst for the opponent
            board[i] = 0; // Reset board after try
        }
    }
    if (move == -1)
        return 0;
    return score;
}

int bestMove(int val, int depth, GameState *game)
{
    int i, j, move, score;
    int best = -1, changed = 0;

    if ((score = win(game->board)))
        return (score == val) ? 1 : -1;

    for (move = 0; move < 9; move++)
    {
        if (game->board[move])
            continue;

        changed = game->board[move] = val;
        score = -bestMove(-val, depth + 1, game);
        game->board[move] = 0;

        if (score <= best)
            continue;
        if (!depth)
        {
            best_move = move;
        }
        best = score;
    }

    return changed ? best : 0;
}

void reset_board(GameState *game)
{
    game->playerTurn = PLAYER_X;
    game->state = RUNNING_STATE;
    game->turn = 1;
    for (int i = 0; i < N * N; ++i)
    {
        game->board[i] = EMPTY;
    }
}
