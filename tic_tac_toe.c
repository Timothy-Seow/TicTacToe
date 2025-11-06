#include "raylib.h"
#include "minimax.h"
#include <stdio.h>

#define SIZE 3
#define CELL_SIZE 200
#define SCREEN_SIZE (CELL_SIZE * SIZE)

typedef enum { MENU, SINGLEPLAYER_CHOICE, PLAYING, GAMEOVER } GameState;
typedef enum { TWO_PLAYER, SINGLE_PLAYER } GameMode;

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

    GameState state = MENU;
    GameMode mode = TWO_PLAYER;

    char board[SIZE][SIZE];
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = ' ';

    char currentPlayer = 'X';
    int gameOver = 0;
    char winner = 0;
    // to track who starts, player = 1, ai =
    int playerStarts = 1; 

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        //state machine for start menu choosing
        if (state == MENU) {
            DrawText("TIC TAC TOE", SCREEN_SIZE/2 - 120, 100, 40, DARKBLUE);
            DrawText("Click to choose mode:", SCREEN_SIZE/2 - 150, 200, 30, GRAY);

            Rectangle btn1 = { SCREEN_SIZE/2 - 100, 300, 200, 60 };
            Rectangle btn2 = { SCREEN_SIZE/2 - 100, 400, 200, 60 };

            DrawRectangleRec(btn1, LIGHTGRAY);
            DrawRectangleRec(btn2, LIGHTGRAY);

            DrawText("Two Player", btn1.x + 20, btn1.y + 15, 25, BLACK);
            DrawText("Single Player", btn2.x + 10, btn2.y + 15, 25, BLACK);

            Vector2 mouse = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btn1)) {
                    mode = TWO_PLAYER;
                    state = PLAYING;
                } else if (CheckCollisionPointRec(mouse, btn2)) {
                    mode = SINGLE_PLAYER;
                    state = SINGLEPLAYER_CHOICE;
                }
            }
        }
        
        // this else if is for option of who starts first
        else if (state == SINGLEPLAYER_CHOICE) {
            DrawText("Single Player Mode", SCREEN_SIZE/2 - 150, 100, 30, DARKBLUE);
            DrawText("Who should start first?", SCREEN_SIZE/2 - 160, 180, 25, GRAY);

            Rectangle btn1 = { SCREEN_SIZE/2 - 100, 280, 200, 60 };
            Rectangle btn2 = { SCREEN_SIZE/2 - 100, 380, 200, 60 };

            DrawRectangleRec(btn1, LIGHTGRAY);
            DrawRectangleRec(btn2, LIGHTGRAY);

            DrawText("You Start", btn1.x + 40, btn1.y + 15, 25, BLACK);
            DrawText("AI Starts", btn2.x + 50, btn2.y + 15, 25, BLACK);

            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btn1)) {
                    playerStarts = 1;
                    currentPlayer = 'X'; // player
                    state = PLAYING;
                } else if (CheckCollisionPointRec(mouse, btn2)) {
                    playerStarts = 0;
                    currentPlayer = 'O'; // AI
                    state = PLAYING;
                }
            }
        }

        else if (state == PLAYING){
            //to make the ai start immeadiatly if player wants it to go first
            if (mode == SINGLE_PLAYER && !gameOver && currentPlayer == 'O') {
                Move best = findBestMove(board);
                board[best.row][best.col] = 'O';
                winner = checkWin(board);
                if (winner || isDraw(board))
                    gameOver = 1;
                else
                    currentPlayer = 'X';
            }           

            // Dave old code starts here
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

                        //to call minimax | might make it so that system chooses who to go first
                        if (mode == SINGLE_PLAYER && currentPlayer == 'O' && !gameOver) {
                            Move best = findBestMove(board);
                            board[best.row][best.col] = 'O';
                            winner = checkWin(board);
                            if (winner || isDraw(board))
                                gameOver = 1;
                            else
                                currentPlayer = 'X';
                        }
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

                DrawText("Press R to restart or M for menu", SCREEN_SIZE - 360, SCREEN_SIZE + 15, 20, BLACK);

                if (IsKeyPressed(KEY_R)) {
                    for (int i = 0; i < SIZE; i++)
                        for (int j = 0; j < SIZE; j++)
                            board[i][j] = ' ';
                    printf("\nCurrent player is: %d", playerStarts);
                    if (playerStarts == 0) {
                        currentPlayer = 'O'; // AI
                        state = PLAYING;
                    }else{currentPlayer = 'X';}
                    winner = 0;
                    gameOver = 0;
                }

                if (IsKeyPressed(KEY_M)){
                    state = MENU;
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
            
        }

        EndDrawing(); 
    }

    CloseWindow();
    return 0;
}