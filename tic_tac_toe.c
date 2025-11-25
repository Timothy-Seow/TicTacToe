#include "raylib.h"
#include "minimax.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui.h"

#define SIZE 3
#define CELL_SIZE 200
#define SCREEN_SIZE (CELL_SIZE * SIZE)

typedef enum { MENU, SINGLEPLAYER_CHOICE, STARTER_SELECT, DIFFICULTY_SELECT, PLAYING, GAMEOVER, PAUSE } GameState;
typedef enum { TWO_PLAYER, SINGLE_PLAYER_MM, SINGLE_PLAYER_NB } GameMode;

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
    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Tic Tac Toe GUI - raylib");
    InitAudioDevice();
    InitUI();

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
    int difficulty = 3;
    //Load textures 
    Texture2D gamebackground = LoadTexture("Graphics/background.png");
    Texture2D gameovertext = LoadTexture("Graphics/gameover.png");
    // Button definitions
    int buttonSpacing = 110;

    Rectangle btn1 = { SCREEN_SIZE/2 - 90, 250, 200, 60 };
    Rectangle btn2 = { SCREEN_SIZE/2 - 90, 250 + buttonSpacing, 200, 60 };
    Rectangle btn3 = { SCREEN_SIZE/2 - 90, 250 + buttonSpacing * 2, 200, 60 };
    
    // Load Music & Sounds 
    Sound clicksound = LoadSound("Audio/clicknew.mp3");

    while (!WindowShouldClose()) {
        BeginDrawing();
        DrawTexture(gamebackground, 0, 0, WHITE);

        //state machine for start menu choosing
        if (state == MENU) {
            DrawText("TIC TAC TOE", SCREEN_SIZE/2 - 140, 100, 40, BLACK);
            DrawText("Click to choose mode:", SCREEN_SIZE/2 - 150, 200, 30, GREEN);

            bool hover1 = CheckCollisionPointRec(GetMousePosition(), btn1);
            DrawButton(btn1, hover1);

            bool hover2 = CheckCollisionPointRec(GetMousePosition(), btn2);
            DrawButton(btn2, hover2);

            DrawText("Two Player", btn1.x + 20, btn1.y + 15, 28, BLACK);
            DrawText("Single Player", btn2.x + 10, btn2.y + 15, 28, BLACK);

            Vector2 mouse = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btn1)) {
                    mode = TWO_PLAYER;
                    state = PLAYING;
                } else if (CheckCollisionPointRec(mouse, btn2)) {
                    // mode = SINGLE_PLAYER;
                    state = SINGLEPLAYER_CHOICE;
                }
            }
        }

        else if (state == SINGLEPLAYER_CHOICE){
            DrawText("Select an AI model to play against", SCREEN_SIZE/2 - 250 , 100, 30, GREEN);

            bool hover1 = CheckCollisionPointRec(GetMousePosition(), btn1);
            DrawButton(btn1, hover1);
            bool hover2 = CheckCollisionPointRec(GetMousePosition(), btn2);
            DrawButton(btn2, hover2);

            DrawText("Minimax", btn1.x + 50, btn1.y + 15, 28, BLACK);
            DrawText("Naive Bayes", btn2.x + 20 , btn2.y + 15, 28, BLACK);

            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btn1)) {
                    state = STARTER_SELECT;
                    mode = SINGLE_PLAYER_MM;
                } else if (CheckCollisionPointRec(mouse, btn2)) {
                    state = STARTER_SELECT;
                    mode = SINGLE_PLAYER_NB;
                }
            }
        }
        
        // this else if is for option of who starts first
        else if (state == STARTER_SELECT) {
            DrawText("Single Player Mode", SCREEN_SIZE/2 - 150, 100, 30, BLUE);
            DrawText("Who should start first?", SCREEN_SIZE/2 - 150, 180, 28, GREEN);

            bool hover1 = CheckCollisionPointRec(GetMousePosition(), btn1);
            DrawButton(btn1, hover1);

            bool hover2 = CheckCollisionPointRec(GetMousePosition(), btn2);
            DrawButton(btn2, hover2);

            DrawText("You Start", btn1.x + 25, btn1.y + 15, 28, BLACK);
            DrawText("AI Starts", btn2.x + 25, btn2.y + 15, 28, BLACK);

            Vector2 mouse = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, btn1)) {
                    playerStarts = 1;
                    currentPlayer = 'X'; // player
                    state = DIFFICULTY_SELECT;
                } else if (CheckCollisionPointRec(mouse, btn2)) {
                    playerStarts = 0;
                    currentPlayer = 'O'; // AI
                    state = DIFFICULTY_SELECT;
                }
            }
        }

        else if (state == DIFFICULTY_SELECT) {
            if (mode == SINGLE_PLAYER_NB){
                difficulty = 0;
                state = PLAYING;
            }
            else{
                DrawText("Select AI Difficulty", SCREEN_SIZE/2 - 150, 100, 30, GREEN);

            bool hover1 = CheckCollisionPointRec(GetMousePosition(), btn1);
            DrawButton(btn1, hover1);
            bool hover2 = CheckCollisionPointRec(GetMousePosition(), btn2);
            DrawButton(btn2, hover2);
            bool hover3 = CheckCollisionPointRec(GetMousePosition(), btn3);
            DrawButton(btn3, hover3);

                DrawText("Easy", btn1.x + 70, btn1.y + 15, 28, BLACK);
                DrawText("Medium", btn2.x + 60, btn2.y + 15, 28, BLACK);
                DrawText("Hard", btn3.x + 70, btn3.y + 15, 28, BLACK);

                Vector2 mouse = GetMousePosition();
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, btn1)) difficulty = 1;
                    else if (CheckCollisionPointRec(mouse, btn2)) difficulty = 2;
                    else if (CheckCollisionPointRec(mouse, btn3)) difficulty = 3;

                    if (difficulty) state = PLAYING;              
                }
            }
            
        }

        else if (state == PLAYING){
            if (!gameOver && currentPlayer == 'O'){
                //to make the ai start immeadiatly if player wants it to go first
                if (mode == SINGLE_PLAYER_MM){
                    Move best = findBestMove(board, difficulty);
                    board[best.row][best.col] = 'O';
                    winner = checkWin(board);
                    if (winner || isDraw(board))
                        gameOver = 1;
                    else
                        currentPlayer = 'X';
                }
                else if (mode == SINGLE_PLAYER_NB){
                    char cmd[512];
                    char buffer[64];
                    char board_str[64] = "";
                    char prediction[10] = {0};

                    // Convert board to string
                    for (int i = 0; i < SIZE; i++) {
                        for (int j = 0; j < SIZE; j++) {
                            char temp[2];
                            temp[0] = (board[i][j] == ' ') ? 'B' : board[i][j];
                            temp[1] = '\0';
                            strcat(board_str, temp);
                            if (j < SIZE - 1) strcat(board_str, ",");
                        }
                        if (i < SIZE - 1) strcat(board_str, ";");
                    }

                    snprintf(cmd, sizeof(cmd),
                             "python -c \"import mlalgo,sys; print(mlalgo.main(sys.argv[1]))\" \"%s\"",
                             board_str);

                    FILE *fp = popen(cmd, "r");
                    int move = -1;
                    if (fp) {
                        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                            // remove newline
                            char *nl = strchr(buffer, '\n');
                            if (nl) *nl = '\0';
                            move = atoi(buffer);
                        }
                        pclose(fp);
                    }

                    // implement the changing of board based on prediction from python here
                    board[move / SIZE][move % SIZE] = 'O';
                    winner = checkWin(board);
                    if (winner || isDraw(board))
                        gameOver = 1;
                    else
                        currentPlayer = 'X';
                }
            }

            // Check if player made a winning move, if not game continues
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
                        DrawText("X", x - 20, y - 20, 40, WHITE);
                    else if (board[i][j] == 'O')
                        DrawText("O", x - 20, y - 20, 40, BLACK);
                }
            }

            if (gameOver) {
                DrawTexture(gameovertext, -80, 0, WHITE);

                if (winner)
                    DrawText(TextFormat("Player %c Wins!", winner), SCREEN_SIZE/2 - 110, 400, 30, GREEN);
                else
                    DrawText("It's a Draw!", SCREEN_SIZE/2 - 110, 400, 30, WHITE);

                DrawText("Press R to restart or M for menu", SCREEN_SIZE/2 - 170, 450, 20, BLACK);

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
                DrawText(TextFormat("Player %c's turn", currentPlayer), 10, SCREEN_SIZE + 10, 30, BLUE);
            }

            // Pause functionality
            if(IsKeyPressed(KEY_P)){
                state = PAUSE;
            }
        }
        // Pause State 
        else if (state == PAUSE){
            // Background dim
            DrawRectangle(0, 0, SCREEN_SIZE, SCREEN_SIZE, (Color){0,0,0,150});

            Vector2 mouse = GetMousePosition();

            bool hoverContinue = CheckCollisionPointRec(mouse, btn1);
            bool hoverRestart  = CheckCollisionPointRec(mouse, btn2);
            bool hoverMenu     = CheckCollisionPointRec(mouse, btn3);

            DrawButton(btn1, hoverContinue);
            DrawButton(btn2, hoverRestart);
            DrawButton(btn3, hoverMenu);

            DrawText("Paused",SCREEN_SIZE/2 - MeasureText("Paused", 50)/2, 150, 50,WHITE);

            DrawCenteredTextInButton(btn1, "Continue", 30, BLACK);
            DrawCenteredTextInButton(btn2, "Restart", 30, BLACK);
            DrawCenteredTextInButton(btn3, "Menu", 30, BLACK);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                Vector2 m = GetMousePosition();

                if (CheckCollisionPointRec(m, btn1)) {
                    state = PLAYING;
                }
                else if (CheckCollisionPointRec(m, btn2)) {
                    memset(board, ' ', sizeof(board));
                    gameOver = 0;
                    winner = 0;
                    currentPlayer = 'X';

                    state = PLAYING;
                }
                else if (CheckCollisionPointRec(m, btn3)) {
                    state = MENU;
                }
            }

        }
        // Click sound for button presses
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            PlaySound(clicksound);
        }

        EndDrawing(); 
    }

    CloseWindow();
    return 0;
}