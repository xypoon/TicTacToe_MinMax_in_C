// Import Libraries
#include <stdio.h>
#include <stdlib.h>

// Declare Functions
int printBoard();
void playerMove();

// Declare Variables
int move;
FILE *fp = NULL;
FILE *gnupipe = NULL;

// Program Start
void main(void)
{
    printBoard();
    // scanf("%d", &move);
    // for (int i = 0; i < 9; i++)
    // {
    //     playerMove();
    // }
}

int printBoard()
{

    char *GnuCommands[] = {"set title \"Tic Tac Toe\"",
                           "set xrange [0:3]",
                           "set yrange [0:3]",
                           "unset xtics",
                           "unset ytics",
                           "unset border",
                           "plot \
                           'lines.tmp' u 1:2 w l lc rgb \"#000000\" title \"\", \
                           'lines.tmp' u 3:4 w l lc rgb \"#000000\" title \"\", \
                           'lines.tmp' u 5:6 w l lc rgb \"#000000\" title \"\", \
                           'lines.tmp' u 7:8 w l lc rgb \"#000000\" title \"\" , handle =1"};

    gnupipe = _popen("gnuplot -persistent", "w");

    for (int i = 0; i < 7; i++)
    {
        fprintf(gnupipe, "%s\n", GnuCommands[i]);
    }

    printf("Board Printed\n");
    return 0;
}

void playerMove()
{
    printf("Please make a move");
    scanf("%d", &move);
    fprintf(gnupipe, "%s\n", "set object circle at first 0.5,0.5 radius char 1 \
    fillstyle empty border lc rgb '#000000' lw 1");
}

// int b[3][3]; /* board. 0: blank; -1: computer; 1: human */

// int check_winner()
// {
//     int i;
//     for (i = 0; i < 3; i++)
//     {
//         if (b[i][0] && b[i][1] == b[i][0] && b[i][2] == b[i][0])
//             return b[i][0];
//         if (b[0][i] && b[1][i] == b[0][i] && b[2][i] == b[0][i])
//             return b[0][i];
//     }
//     if (!b[1][1])
//         return 0;

//     if (b[1][1] == b[0][0] && b[2][2] == b[0][0])
//         return b[0][0];
//     if (b[1][1] == b[2][0] && b[0][2] == b[1][1])
//         return b[1][1];

//     return 0;
// }

// void showboard()
// {
//     const char *t = "X O";
//     int i, j;
//     for (i = 0; i < 3; i++, putchar('\n'))
//         for (j = 0; j < 3; j++)
//             printf("%c ", t[b[i][j] + 1]);
//     printf("-----\n");
// }

// #define for_ij              \
//     for (i = 0; i < 3; i++) \
//         for (j = 0; j < 3; j++)
// int best_i, best_j;
// int test_move(int val, int depth)
// {
//     int i, j, score;
//     int best = -1, changed = 0;

//     if ((score = check_winner()))
//         return (score == val) ? 1 : -1;

//     for_ij
//     {
//         if (b[i][j])
//             continue;

//         changed = b[i][j] = val;
//         score = -test_move(-val, depth + 1);
//         b[i][j] = 0;

//         if (score <= best)
//             continue;
//         if (!depth)
//         {
//             best_i = i;
//             best_j = j;
//         }
//         best = score;
//     }

//     return changed ? best : 0;
// }

// const char *game(int user)
// {
//     int i, j, k, move, win = 0;
//     for_ij b[i][j] = 0;

//     printf("Board postions are numbered so:\n1 2 3\n4 5 6\n7 8 9\n");
//     printf("You have O, I have X.\n\n");
//     for (k = 0; k < 9; k++, user = !user)
//     {
//         while (user)
//         {
//             printf("your move: ");
//             if (!scanf("%d", &move))
//             {
//                 scanf("%*s");
//                 continue;
//             }
//             if (--move < 0 || move >= 9)
//                 continue;
//             if (b[i = move / 3][j = move % 3])
//                 continue;

//             b[i][j] = 1;
//             break;
//         }
//         if (!user)
//         {
//             if (!k)
//             { /* randomize if computer opens, less boring */
//                 best_i = rand() % 3;
//                 best_j = rand() % 3;
//             }
//             else
//                 test_move(-1, 0);

//             b[best_i][best_j] = -1;
//             printf("My move: %d\n", best_i * 3 + best_j + 1);
//         }

//         showboard();
//         if ((win = check_winner()))
//             return win == 1 ? "You win.\n\n" : "I win.\n\n";
//     }
//     return "A draw.\n\n";
// }

// int main()
// {
//     int first = 0;
//     while (1)
//         printf("%s", game(first = !first));
//     return 0;
// }