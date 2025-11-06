#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include "minimax.h"

#define SIZE 3

static char player = 'O';     // AI
static char opponent = 'X';   // Human

static int max(int a, int b) { return (a > b) ? a : b; }
static int min(int a, int b) { return (a < b) ? a : b; }

static int isMovesLeft(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                return 1;
    return 0;
}

static int evaluate(char b[SIZE][SIZE]) {
    // Rows
    for (int row = 0; row < SIZE; row++) {
        if (b[row][0] == b[row][1] && b[row][1] == b[row][2]) {
            if (b[row][0] == player) return +10;
            if (b[row][0] == opponent) return -10;
        }
    }

    // Columns
    for (int col = 0; col < SIZE; col++) {
        if (b[0][col] == b[1][col] && b[1][col] == b[2][col]) {
            if (b[0][col] == player) return +10;
            if (b[0][col] == opponent) return -10;
        }
    }

    // Diagonals
    if (b[0][0] == b[1][1] && b[1][1] == b[2][2]) {
        if (b[0][0] == player) return +10;
        if (b[0][0] == opponent) return -10;
    }

    if (b[0][2] == b[1][1] && b[1][1] == b[2][0]) {
        if (b[0][2] == player) return +10;
        if (b[0][2] == opponent) return -10;
    }

    return 0; // No win
}

static int minimax(char board[SIZE][SIZE], int depth, int isMax) {
    int score = evaluate(board);
    if (score == 10) return score - depth;  // favor faster wins
    if (score == -10) return score + depth; // favor slower losses
    if (!isMovesLeft(board)) return 0;

    if (isMax) {
        int best = -1000;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = player;
                    best = max(best, minimax(board, depth + 1, 0));
                    board[i][j] = ' ';
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = opponent;
                    best = min(best, minimax(board, depth + 1, 1));
                    board[i][j] = ' ';
                }
            }
        }
        return best;
    }
}

Move findBestMove(char board[SIZE][SIZE]) {
    int bestVal = -1000;
    Move bestMoves[9];
    int bestCount = 0;

    // --- Step 1: If it's the first move and center is empty, take it ---
    int emptyCount = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                emptyCount++;

    // If it's the first AI move (board mostly empty) and center is free
    if (emptyCount >= 8 && board[1][1] == ' ') {
        Move center = {1, 1};
        return center;
    }

    // --- Step 2: Otherwise, use minimax and collect all best moves ---
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = player;
                int moveVal = minimax(board, 0, 0);
                board[i][j] = ' ';

                if (moveVal > bestVal) {
                    bestVal = moveVal;
                    bestCount = 0;
                    bestMoves[bestCount++] = (Move){i, j};
                } else if (moveVal == bestVal) {
                    bestMoves[bestCount++] = (Move){i, j};
                }
            }
        }
    }
    srand(time(NULL)); // ensure randomness
    int choice = rand() % bestCount;

    printf("\nbest move is: %d,%d", bestMoves[choice],bestMoves[choice]);
    return bestMoves[choice];
}
