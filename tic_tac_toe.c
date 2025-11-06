#include "raylib.h"
#include <stdio.h>

#define SIZE 3
#define CELL_SIZE 200
#define SCREEN_SIZE (CELL_SIZE * SIZE)

int checkWin(char board[3][3]) {
    for (int i = 0; i < 3; i++) {
        // Check rows
        if (board[i][0] != ' ' &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2])
            return board[i][0];

        // Check columns
        if (board[0][i] != ' ' &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i])
            return board[0][i];
    }

    // Check diagonals
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2])
        return board[0][0];

    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0])
        return board[0][2];

    return 0;  // no winner
}

int isDraw(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ')
                return 0;
    return 1;
}

int main(void) {
    InitWindow(SCREEN_SIZE, SCREEN_SIZE + 50, "Tic Tac Toe GUI - raylib");

    char board[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    char currentPlayer = 'X';
    int gameOver = 0;
    char winner = 0;

    while (!WindowShouldClose()) {
        if (!gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            int row = mouse.y / CELL_SIZE;
            int col = mouse.x / CELL_SIZE;

            if (row < SIZE && col < SIZE && board[row][col] == ' ') {
                board[row][col] = currentPlayer;
                winner = checkWin(board);
                if (winner || isDraw(board))
                    gameOver = 1;
                else
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw grid
        for (int i = 1; i < SIZE; i++) {
            DrawLine(0, i * CELL_SIZE, SCREEN_SIZE, i * CELL_SIZE, BLACK);
            DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_SIZE, BLACK);
        }

        // Draw marks
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                int x = j * CELL_SIZE + CELL_SIZE / 2;
                int y = i * CELL_SIZE + CELL_SIZE / 2;
                if (board[i][j] == 'X')
                    DrawText("X", x - 20, y - 20, 40, RED);
                else if (board[i][j] == 'O')
                    DrawText("O", x - 20, y - 20, 40, BLUE);
            }
        }

        if (gameOver) {
            if (winner)
                DrawText(TextFormat("Player %c Wins!", winner), 10, SCREEN_SIZE + 10, 30, DARKGREEN);
            else
                DrawText("It's a Draw!", 10, SCREEN_SIZE + 10, 30, DARKGRAY);

            DrawText("Press R to restart", SCREEN_SIZE - 200, SCREEN_SIZE + 10, 20, BLACK);

            if (IsKeyPressed(KEY_R)) {
                for (int i = 0; i < SIZE; i++)
                    for (int j = 0; j < SIZE; j++)
                        board[i][j] = ' ';
                currentPlayer = 'X';
                winner = 0;
                gameOver = 0;
            }
        } else {
            DrawText(TextFormat("Player %c's turn", currentPlayer), 10, SCREEN_SIZE + 10, 30, DARKBLUE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}