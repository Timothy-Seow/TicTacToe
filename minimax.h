#ifndef MINIMAX_H
#define MINIMAX_H

#define SIZE 3

typedef struct {
    int row;
    int col;
} Move;

Move findBestMove(char board[SIZE][SIZE]);

#endif
