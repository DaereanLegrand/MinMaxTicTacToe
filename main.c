#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <nuklear.h>

#define MAX_MEMORY 1024

typedef struct node node;

struct node {
    int val, alpha, beta;
    node *next;
};

void
printBoard(int **board, int n)
{
    printf(" ");
    for (int j = 0; j < n; j++) {
        printf(" %c ", j + 'a');
    }
    for (int i = 0; i < n; i++) {
        printf("\n%c", i + 'A');
        for (int j = 0; j < n; j++) {
            printf("|");
            switch (board[i][j]) {
                case 1:
                    printf("X");
                    break;
                case -1:
                    printf("O");
                    break;
                default:
                    printf(" ");
                    break;
            }
            printf("|");
        }
        printf("\n");
    }
}

int
gameStatus(int x, int y, int **board, int n)
{
    int player = board[y][x];
    int accH = 0;
    int accV = 0;
    int accD = 0;
    int accDD = 0;
    for (int i = 0; i < n; i++) {
        accH += board[y][i];
        accV += board[i][x];
        accD += board[i][i];
        accDD += board[i][n - i - 1];
    }

    int winScore = player * n;
    if (accDD == winScore || accD == winScore || accH == winScore || accV == winScore)
        return 1;
    return 0;
}

float
evalFunction(int **board, int n, int currentEval)
{
    float accH = 0;
    float accV = 0;
    for (int j = 0; j < n; j++) {
        float paccH = 0;
        float paccV = 0;
        float scoreH = 1;
        float scoreV = 1;
        for (int i = 0; i < n; i++) {
            if (board[j][i] == currentEval)
                paccH += scoreH;
            else if (board[j][i] == (currentEval * -1))
                paccH = scoreH = -1;

            if (board[i][j] == currentEval)
                paccV += scoreV;
            else if (board[i][j] == (currentEval * -1))
                paccV = scoreV = -1;
        }
        accH += paccH;
        accV += paccV;
    }

    accH = (accH == 0 ? 0 : accH / n);
    accV = (accV == 0 ? 0 : accV / n);

    float accD = 0;
    float scoreD = 1;

    float accDD = 0;
    float scoreDD = 1;
    for (int i = 0; i < n; i++) {
        if (board[i][i] == currentEval)
            accD += scoreD;
        else if (board[i][i] == (currentEval * -1))
            accD = scoreD = -1;

        if (board[i][n - i - 1] == currentEval)
            accDD += scoreDD;
        else if (board[i][n - i - 1] == (currentEval * -1))
            accDD = scoreDD = -1;

    }

    // printf("H: %f, V: %f, D: %f\n", accH, accV, accD);
    float winP = (accH == 0 ? 0 : accH / n) + (accV == 0 ? 0 : accV / n) + (accD == 0 ? 0 : accD / n) + (accDD == 0 ? 0 : accDD / n);
    return winP == 0 ? 0 : winP / 3;
}

float
minMax(int **board, int n, int *x, int *y, int currentEval, int depth)
{
    if (depth == 0) {
        return evalFunction(board, n, currentEval);
    }

    // MAX = -1, MIN = 1
    float alpha, beta, retval;
    if (currentEval == -1)
        retval = INT_MIN;
    else
        retval = INT_MAX;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 0) {
                board[i][j] = currentEval;
                float temp = minMax(board, n, x, y, currentEval * -1, depth - 1);
                board[i][j] = 0;

                if (currentEval == -1) {
                    if (temp > retval) {
                        retval = temp;
                        *x = j;
                        *y = i;
                    }
                } else {
                    if (temp < retval) {
                        retval = temp;
                        *x = j;
                        *y = i;
                    }
                }
            }
        }
    }

    return retval;
}


int
main()
{
    char userInp[5];
    int n, p, d;

    printf("Ingrese el tamaño del tablero: ");
    fgets(userInp, 3, stdin);
    n = atoi(userInp);
    if (n < 3) {
        printf("Error en el tamaño.\n");
        return -1;
    }

    printf("Ingrese la profundidad del algoritmo: ");
    fgets(userInp, 3, stdin);
    d = atoi(userInp);
    if (d < 1) {
        printf("Error en la profundidad.\n");
        return -1;
    }
    printf("Desea empezar usted? [S/n]: ");
    fgets(userInp, 3, stdin);
    p = atoi(userInp);
    p = (p == 'n' || p == 'N') ? 0 : 1;

    int **board = (int**)malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++) {
        board[i] = (int*)malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++) {
            board[i][j] = 0;       
        }
    }

    int gameState = 1;
    while (gameState == 1) {
        printBoard(board, n);
        if (p == 1) {
            printf("Ingrese su jugada de forma [a-z]-[A-Z]: ");
            char x, y;
            fgets(userInp, 5, stdin);
            sscanf(userInp, "%1c-%1c", &x, &y);
            while (!(x >= 'a' && x <= ('a' + n) && y >= 'A' && y <= ('A' + n))) {
                printf("Entrada incorrecta, ingrese su jugada nuevamente [a-z]-[A-Z]: ");
                fgets(userInp, 5, stdin);
                sscanf(userInp, "%1c-%1c", &x, &y);
            }
            x = x - 'a';
            y = y - 'A';
            board[y][x] = 1;

            printBoard(board, n);
            // printf("Probabilidad de ganar: %f\n", evalFunction(board, n, board[y][x]));
            if (gameStatus(x, y, board, n)) {
                printf("Gano el jugador!\n");
                return 1;
            }

        }

        int x = -1;
        int y = -1;

        minMax(board, n, &x, &y, 1, d);

        if (x >= 0 && y >= 0) {
            board[y][x] = -1;
        }

        if (gameStatus(x, y, board, n)) {
                printf("Gano el computador!\n");
                return 1;
            }

        
        p = 1;
    }


    return 0;
}
