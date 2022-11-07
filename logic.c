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
        // printf("here");
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

int win(const int board[9])         //Win Conditions
{
    unsigned winsStates[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}}; //ALl Possible winnable states on the board
    for (int i = 0; i < 8; ++i) // loop through the different outcomes to check if value on the board are 3 in a row
    {
        if (board[winsStates[i][0]] != 0 &&
            board[winsStates[i][0]] == board[winsStates[i][1]] &&
            board[winsStates[i][0]] == board[winsStates[i][2]])
            return board[winsStates[i][0]]; // Return 1 if Player X Wins, return -1 if Player O Wins
    }
    return 0;
}

void checkWin(GameState *game)
{
    // Check win condition
    switch (win(game->board)) // Depending of player change game win state
    {
    case 0:
        if (game->turn == 9 && win(game->board) != 1) // If board filled and game tied, set game state to tie
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

char gridChar(int i)// Convert the board[9] to X, O, or blank
{
    switch (i)
    {
    case 1:
        return 'X';
    case -1:
        return 'O';
    default:
        return ' ';
    }
}


void draw_in_terminal(int b[9]) // Plots tictactoe in terminal based on Gamestate Board
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
    int score = -1;
    int i;
    for (i = 0; i < 9; ++i)                     // Goes through All the moves/squares in the board
    {                       
        if (board[i] == 0)                      // Check if move is empty
        {
        board[i] = PLAYER_O;                    // Bot Test his move
            int tempScore = -miniMax(board, 1); 
            board[i] = 0;                       // Resets the board back to its previous state
            if (tempScore > score)              // Bot Makes its mov
            {
                score = tempScore;              // Returns a score based on minimax tree at a given node.
                move = i;                       // Move = best possible move calculated from the minimax algorithm 
            }
        }
    }
    board[move] = PLAYER_O;                     //Bot Takes its move
}

int miniMax(int board[9], int player)                   //Winnable Minimax Algorithm
{
    int winner = win(board);                            //Check if the game ended
    if (winner != 0) 
        return winner * player;

    int move = -1;
    int score = 0;                                       //Score of 0 startes of the algorithm thinking in a more neutral way
    int i;
    for (i = 0; i < 9; ++i)                              // For all moves
    {                                                    
        if (board[i] == 0)                               // Check if move is legal (Empty square)
        {                                                // If legal,
            board[i] = player;                           // Try the move
            int thisScore = miniMax(board, player * -1); // Test the next players move and return the score back here
            if (thisScore > score)
            {
                score = thisScore;                       //Constantly updates the score based on the next available win/lose/draw move
                move = i;
            }                                            // Pick the move that's worst for the opponent
            board[i] = 0;                                // Reset board after try
        }
    }

    return score;
}

int bestMove(int val, int depth, GameState *game)
{
    int move, score;
    int best = -1, changed = 0;

    if ((score = win(game->board)))     // check the current board which player wins
        return (score == val) ? 1 : -1; // HELP

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
