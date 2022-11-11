// Import Libraries
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "render.h"
#include "game.h"
#include "logic.h"

// Declare variables
int best_move;

// Functions

int processEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game)
{
    // SDL Event Listener
    SDL_Event event;

    // Variable to store status of done
    int done = 0;

    // Constantly loop botMove function, in botMove, if recorded playerTurn in GameState is not bot, it will not trigger
    botMove(game);

    while (SDL_PollEvent(&event)) // Check if there are any SDL Events to be handled
    {
        switch (event.type) // Switch to the case depending on the type of event
        {
        // Quiting Game
        case SDL_WINDOWEVENT_CLOSE: // Click the cross to close window
        {
            if (window) // Check if window still exist
            {
                printf("close Pressed\n");
                SDL_DestroyWindow(window); // close window
                window = NULL;
                done = 1; // return 1 to tell program to end
                break;
            }
        }
        break;
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE: // ESC Key on keyboard is presssed
                printf("Escape Pressed\n");
                done = 1; // return 1 to tell program to end
                break;
            }
        }
        break;
        case SDL_QUIT:
            // quit out of the game
            done = 1;
            break;
        case SDL_MOUSEBUTTONDOWN: // Listen to mouse click event
        {
            /* By dividing the position of the reference location with the width/height of the cell,
            integer returns the floor value which can be used to reference the cell to plot to */
            int row = event.button.y / CELL_HEIGHT; // Calculate clicked row
            int col = event.button.x / CELL_WIDTH;  // Calculate clicked col

            if (game->state == RUNNING_STATE) // Check if game is running
            {
                playerMove(game, row, col); // Player will move
            }
            else
            {
                reset_board(game); // If Gamestate State if not running, means game has ended and to reset board
            }
            break;
        }
        }
    }
    return done; // Return the value of done = 1 if program exits, else return 0 to continue loop
}

int processMenuEvents(SDL_Renderer *renderer, SDL_Window *window, GameState *game)
{
    // SDL Event Listener
    SDL_Event event;
    int done = 0;
    int menu = 0;

    while (SDL_PollEvent(&event)) // Check if there are any SDL Events to be handled
    {
        switch (event.type) // Switch to the case depending on the type of event
        {
        // Quiting Game
        case SDL_WINDOWEVENT_CLOSE: // Click the cross to close window
        {
            if (window) // Check if window still exist
            {
                printf("close Pressed\n");
                SDL_DestroyWindow(window); // close window
                window = NULL;
                done = 1; // return 1 to tell program to end
                break;
            }
        }
        break;
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE: // ESC Key on keyboard is presssed
                printf("Escape Pressed\n");
                done = 1; // return 1 to tell program to end
                break;
            }
        }
        break;
        case SDL_QUIT:
            // quit out of the game
            done = 1;
            break;
        case SDL_MOUSEBUTTONDOWN: // Listen to mouse click event
        {
            /* By dividing the position of the reference location with the cell-height,
            integer returns the floor value which can be used to reference the row clicked */
            selectGamemode(event.button.y / CELL_HEIGHT, game);
            menu = 1; // return 1 to tell program to exit menu
            break;
        }
        }
    }
    return menu; // Return the value of done = 1 if menu exits, else return 0 to continue loop
}

void switchPlayerMove(GameState *game)
{
    if (game->playerTurn == PLAYER_O)
    {
        game->playerTurn = PLAYER_X;
    }
    else
    {
        game->playerTurn = PLAYER_O;
    }
}

void selectGamemode(int col, GameState *game)
{
    switch (col)
    {
    case 0:
        game->gamemode = 0; // Player select Player vs Player
        printf("Game mode %d, Player vs Player Selected\n", game->gamemode);
        break;
    case 1:
        game->gamemode = 1; // Player select Player vs AI(Easy)
        printf("Game mode %d, Player vs AI (Easy) Selected\n", game->gamemode);
        break;
    case 2:
        game->gamemode = 2; // Player select Player vs AI(Impossible)
        printf("Game mode %d, Player vs AI (Impossible) Selected\n", game->gamemode);
        break;
    }
}

void botMove(GameState *game)
{
    if (game->state == RUNNING_STATE && game->playerTurn == -1 && game->gamemode == 1)
    {
        computerMove(game->board);     // Invoke the computer move
        draw_in_terminal(game->board); // Draw tictactoe move in terminal

        checkWin(game); // Check for game end after the bot moves

        // Switch Player Turn
        switchPlayerMove(game);
    }
    else if (game->state == RUNNING_STATE && game->playerTurn == -1 && game->gamemode == 2)
    {
        bestMove(-1, 0, game);             // Invoke to calculate best move
        game->board[best_move] = PLAYER_O; // Bot makes the best move
        draw_in_terminal(game->board);     // Draw tictactoe move in terminal
        checkWin(game);                    // Check for game end after the bot moves

        switchPlayerMove(game); // Switch Player Turn
    }
    else
    {
        // Bot has nothing to do as game is not running or not his turn
    }
}

void playerMove(GameState *game, int row, int column)
{
    int position = row * N + column; // Converts row and col to board[9] format
    if (game->board[position] == EMPTY)
    {
        game->board[position] = game->playerTurn; // Plots player move to board
        draw_in_terminal(game->board);            // Draw tictactoe move in terminal
        checkWin(game);                           // Check win condition

        switchPlayerMove(game); // Switch Player Turn
    }
}

int winCon(int gameBoard[9]) // Win Conditions
{
    int winsStates[8][3] = {{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}, {0, 1, 2}, {3, 4, 5}, {6, 7, 8}}; // ALl Possible winnable states on the board
    // check if any of the win states are met
    for (int i = 0; i < 8; i++) // loop through the different outcomes to check if value on the board are 3 in a row
    {
        if (gameBoard[winsStates[i][0]] != 0 && gameBoard[winsStates[i][0]] == gameBoard[winsStates[i][1]] && gameBoard[winsStates[i][0]] == gameBoard[winsStates[i][2]]) // Check if the value on the board are the same (Check if the player has won)
            return gameBoard[winsStates[i][0]];                                                                                                                           // Return 1 if Player X Wins, return -1 if Player O Wins
    }
    return 0; // Return 0 if no one has won
}

void checkWin(GameState *game)
{
    // Check win condition
    switch (winCon(game->board)) // Depending of player change game win state
    {
    case 0:
        if (game->turn == 9 && winCon(game->board) != 1) // If board filled and game tied, set game state to tie
        {
            game->state = TIE_STATE;
            printf("A draw. Better luck next time.\n");
        }
        else
        {
            game->turn++; // If board not filled, proceed to next turn
            printf("Not win, Now turn %d\n", game->turn);
        }
        break;
    case 1:
        if (game->playerTurn == 1) // Player X wins
        {
            game->state = PLAYER_X_WON_STATE;
            printf("turn: %d, player:%d\n", game->turn + 1, game->playerTurn);
            printf("Player X wins.\n");
        }
        else
        {
        }
        break;
    case -1: // Player O wins
        game->state = PLAYER_O_WON_STATE;
        printf("turn: %d, player:%d\n", game->turn + 1, game->playerTurn);
        printf("Player O wins.\n");
        break;
    }
}

char plotPlayer(int p) // Convert the board[9] to X, O, or blank
{
    switch (p)
    {
    case 1:
        return 'X';
    case -1:
        return 'O';
    default:
        return ' ';
    }
}

void draw_in_terminal(int x[9]) // Plots tictactoe in terminal based on Gamestate Board
{
    printf("==============\n");
    for (int i = 0; i < 3; i++)
    {
        printf("| %c | %c | %c |\n", plotPlayer(x[i * 3]), plotPlayer(x[i * 3 + 1]), plotPlayer(x[i * 3 + 2]));
        printf("==============\n");
    }
}

void computerMove(int gameBoard[9])
{
    int move = -1;
    int score = -1;
    int miniMaxScore;

    for (int i = 0; i < 9; i++) // Goes through All the moves/squares in the board
    {
        if (gameBoard[i] == 0) // Check if move is empty
        {
            gameBoard[i] = PLAYER_O; // Bot Test his move
            miniMaxScore = -miniMax(gameBoard, 1);
            gameBoard[i] = 0;         // Resets the board back to its previous state after testing
            if (miniMaxScore > score) // Bot Makes its move
            {
                score = miniMaxScore; // Returns a score based on minimax tree at a given node.
                move = i;             // Move = best possible move calculated from the minimax algorithm
            }
        }
    }
    gameBoard[move] = PLAYER_O; // Bot Takes its move
}

int miniMax(int gameBoard[9], int player) // Winnable Minimax Algorithm
{
    int winner = winCon(gameBoard); // Check if the game ended
    if (winner != 0)
        return winner * player;

    int testMove = -1;
    int startScore = 0;
    for (int i = 0; i < 9; i++) // For all moves
    {
        if (gameBoard[i] == 0)                               // Check if move is legal (Empty square)
        {                                                    // If move is legal
            gameBoard[i] = player;                           // Make the move
            int thisScore = miniMax(gameBoard, player * -1); // Recursively check the next move
            if (thisScore > startScore)
            {
                startScore = thisScore; // Constantly updates the score based on the next available win/lose/draw move

                testMove = i;
            }                 // Pick the move that's worst for the opponent
            gameBoard[i] = 0; // Reset board after try
        }
    }

    return startScore;
}

int bestMove(int val, int depth, GameState *game)
{
    int move, score;
    int best = -1, changed = 0;

    if ((score = winCon(game->board)))     // check the current board which player wins
        return (score == val) ? 1 : -1; // Return the previous player win value

    for (move = 0; move < 9; move++)
    {
        if (game->board[move]) // If the board position not equal 0, skip to position
            continue;

        changed = game->board[move] = val;        // make the test move
        score = -bestMove(-val, depth + 1, game); // run minmax to minimise for opposing player
        game->board[move] = 0;                    // undo moves

        if (score <= best)
            continue; // If score is not greater than current best skip code and go to next
        if (!depth)   // If depth not equal to 0
        {
            best_move = move;
        }
        best = score;
    }
    return changed ? best : 0;
}

void reset_board(GameState *game) // Resets Gamestate after game ends
{
    game->playerTurn = PLAYER_X;    // Reset to Player X
    game->state = RUNNING_STATE;    // Reset to Game Running State
    game->turn = 1;                 // Reset turn counter to 1
    for (int i = 0; i < N * N; ++i) // Reset board to Empty
    {
        game->board[i] = EMPTY;
    }
}
