#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "minimax.h"

#define SIZE 3
#define BOARD_CELLS (SIZE * SIZE)
#define INF 10000

// Define quick access macro for 1D board
#define CELL(board, r, c) ((board)[(r) * SIZE + (c)])

static char player = 'O'; // AI
static char opponent = 'X'; // Human
static int currentDifficulty = 3;
static int pureEvaluationMode = 0;

// Utility helpers

// Returns 1 if any moves remain. If returns 0, minimax will stop recursion, meaning draw
static inline int movesLeft(const char *b) {
    for (int i = 0; i < BOARD_CELLS; i++) if (b[i] == ' ') return 1;
    return 0;
}

// For level 3. This to very clearly see wins/losses. 
static inline int checkWin(const char *b, char symbol) {
    for (int i = 0; i < SIZE; i++)
        if ((CELL(b,i,0)==symbol && CELL(b,i,1)==symbol && CELL(b,i,2)==symbol) ||
            (CELL(b,0,i)==symbol && CELL(b,1,i)==symbol && CELL(b,2,i)==symbol))
            return 1;
    return (CELL(b,0,0)==symbol && CELL(b,1,1)==symbol && CELL(b,2,2)==symbol) ||
           (CELL(b,0,2)==symbol && CELL(b,1,1)==symbol && CELL(b,2,0)==symbol);
}

//evaluation function (handles all difficulty levels)
static inline int evaluate(const char *b, int difficulty) {
    // Hard mode (perfect play): only check wins/losses
    if (difficulty == 3) {
        if (checkWin(b, player))   return +10;
        if (checkWin(b, opponent)) return -10;
        return 0;
    }

    // Positional weights: center > corners > edges (for lvl 1 and 2)
    static const int weights[SIZE][SIZE] = {{3,2,3},{2,5,2},{3,2,3}}; // weights for all positions
    int score = 0;

    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (CELL(b,r,c)==player) score += weights[r][c];
            else if (CELL(b,r,c)==opponent) score -= weights[r][c];

    // Simple 2-in-a-row bonus for medium difficulty
    if (difficulty >= 2) {
        for (int i = 0; i < SIZE; i++) {
            int aiR=0, oppR=0, aiC=0, oppC=0;
            for (int j = 0; j < SIZE; j++) {
                if (CELL(b,i,j)==player) aiR++; else if (CELL(b,i,j)==opponent) oppR++;
                if (CELL(b,j,i)==player) aiC++; else if (CELL(b,j,i)==opponent) oppC++;
            }
            if (aiR==2 && oppR==0) score += 10; if (oppR==2 && aiR==0) score -= 8;
            if (aiC==2 && oppC==0) score += 10; if (oppC==2 && aiC==0) score -= 8;
        }
    }
    return score;
}

// Minimax with alpha-beta pruning
static int minimax(char *board, int depth, int isMax, int alpha, int beta, int maxDepth) {
    // evaluate current board
    int score = evaluate(board, currentDifficulty);

    if (currentDifficulty == 3) {
        if (score == 10 || score == -10) return score - depth;
        if (!movesLeft(board)) return 0;
    } else {
        if (score >= 1000 || score <= -1000) return score - depth;
        if (!movesLeft(board) || depth >= maxDepth) return score;
    }

    int best = isMax ? -INF : INF;
    char symbol = isMax ? player : opponent;

    // Prioritize center and corners first
    int order[9][2] = {{1,1},{0,0},{0,2},{2,0},{2,2},{0,1},{1,0},{1,2},{2,1}};

    for (int k = 0; k < 9; k++) {
        int i = order[k][0], j = order[k][1];
        if (CELL(board,i,j) != ' ') continue;

        // “plays” a move on the board temporarily to find score of that move then keeps going deeper into tree
        CELL(board,i,j) = symbol;
        int val = minimax(board, depth + 1, !isMax, alpha, beta, maxDepth);
        CELL(board,i,j) = ' ';

        if (isMax) {
            best = (val > best) ? val : best;
            alpha = (best > alpha) ? best : alpha;
        } else {
            best = (val < best) ? val : best;
            beta  = (best < beta) ? best : beta;
        }
        if (beta <= alpha) break; // prune branch
    }
    return best;
}

// Find best move based on current board and difficulty
Move findBestMove(char board2D[SIZE][SIZE], int difficulty) {
    srand(time(NULL));
    currentDifficulty = difficulty;
    pureEvaluationMode = (difficulty == 3);
    clock_t start = clock();

    // Always take center immediately in Hard mode if available for lvl 3
    if (difficulty == 3 && board2D[1][1] == ' ') {
        return (Move){1, 1};
    }

    // Flatten 2D -> 1D board
    char board[BOARD_CELLS];
    memcpy(board, board2D, sizeof(board));

    // Collect empty cells
    Move emptyCells[9];
    int emptyCount = 0;
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board2D[r][c] == ' ')
                emptyCells[emptyCount++] = (Move){r, c};

    // Difficulty setup
    int maxDepth = 9, imperfectChance = 0;
    if (difficulty == 1) {
        if ((rand() % 100) < 80) { // mostly random
            Move random = emptyCells[rand() % emptyCount];
            printf("\nEasy mode played random move\n");
            return random;
        }
        maxDepth = 2;
    } else if (difficulty == 2) {
        maxDepth = 4 + rand() % 2;
        imperfectChance = 20;
    }

    // Evaluate all possible moves
    int bestVal = -INF, bestCount = 0;
    Move bestMoves[9];

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (CELL(board, i, j) != ' ') continue;

            CELL(board, i, j) = player;
            int moveVal = minimax(board, 0, 0, -INF, INF, maxDepth);
            CELL(board, i, j) = ' ';

            if (moveVal > bestVal) {
                bestVal = moveVal;
                bestCount = 0;
                bestMoves[bestCount++] = (Move){i, j};
            } else if (moveVal == bestVal) {
                bestMoves[bestCount++] = (Move){i, j};
            }
        }
    }

    // Medium difficulty: sometimes make imperfect move
    if (difficulty == 2 && (rand() % 100) < imperfectChance) {
        Move random = emptyCells[rand() % emptyCount];
        printf("\nmedium mode made imperfect move\n");
        return random;
    }

    Move bestMove = bestMoves[rand() % bestCount];

    clock_t end = clock();
    printf("\nAI (Level %d) chose (%d,%d) in %.5f sec\n",
           difficulty, bestMove.row, bestMove.col,
           (double)(end - start) / CLOCKS_PER_SEC);

    return bestMove;
}
