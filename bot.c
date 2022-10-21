// Compile Code
// As you add new SDL2 libraries, add their dll name to the compile command
// gcc -I src/include -L src/lib -o main tictactoe.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

//  Import Libraries
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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
    int turn;
} GameState;

// Declare Functions
void botMove(GameState *game, int row, int column);
void draw_in_terminal(int b[9]);
void select_gamemode(int col);
int minimax(int board[9], int player);
void computerMove(int board[9]);

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

    // determines if a player has won return 1, returns 0 otherwise.
    unsigned wins[8][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
    int i;
    for (i = 0; i < 8; ++i)
    {
        if (board[wins[i][0]] != 0 &&
            board[wins[i][0]] == board[wins[i][1]] &&
            board[wins[i][0]] == board[wins[i][2]])
            return 1;
    }
    return 0;
}

void reset_board(GameState *game)
{
    game->playerTurn = PLAYER_X;
    game->state = RUNNING_STATE;
    game->turn = 0;
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
            // By dividing the position of the reference location with the width/height of the cell, integer returns the floor value which can be used to reference the cell to plot to
            int row = event.button.y / CELL_HEIGHT;
            int col = event.button.x / CELL_WIDTH;
            if (game->state == RUNNING_STATE)
            {
                botMove(game, row, col);
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

int processMenuEvents(SDL_Renderer *renderer, SDL_Window *window)
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
            select_gamemode(event.button.y / CELL_HEIGHT);
            menu = 1;
            break;
        }
        }
    }
    return menu;
}

void select_gamemode(int col)
{
    switch (col)
    {
    case 0:
        printf("Player vs Player Selected\n");
        break;
    case 1:
        printf("Player vs AI (Easy) Selected\n");
        break;
    case 2:
        printf("Player vs AI (Impossible) Selected\n");
        break;
    }
}

void botMove(GameState *game, int row, int column)
{
    if (game->board[row * N + column] == EMPTY)
    {
        if (game->playerTurn == 2)
        {
            // printf("here");
            computerMove(game->board);
            draw_in_terminal(game->board);
        }
        else
        {
            // printf("here!!");
            game->board[row * N + column] = 1;
            draw_in_terminal(game->board);
        }
        // Draw in Console

        // Check win condition
        switch (win(game->board))
        {
        case 0:
            if (game->turn == 8 && win(game->board) != 1)
            {
                printf("A draw. How droll.\n");
            }
            break;
        case 1:
            if (game->playerTurn == 1)
            {
                game->state = PLAYER_X_WON_STATE;
                printf("Player X wins.\n");
            }
            else
            {
                game->state = PLAYER_O_WON_STATE;
                printf("Player O wins.\n");
            }

            break;
            // case -1:
            //   printf("You win. Inconceivable!\n");
            //   break;
        }

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
    game->turn++;
}

void click_on_cell(GameState *game, int row, int column)
{
    if (game->state == RUNNING_STATE)
    {
        botMove(game, row, column);
    }
    else
    {
        reset_board(game);
    }
}

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
        .turn = 0};

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

                menu = processMenuEvents(renderer, window);

                // Delay Refresh so as too not overun
                SDL_Delay(100);
            }
        }

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
    board[move] = 2;
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