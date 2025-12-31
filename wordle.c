#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"

#define WORD_LEN 5
#define MAX_GUESSES 6
#define TILE_SIZE 64
#define TILE_PADDING 5

typedef enum {
	TILE_UNKNOWN = 0,
	TILE_ABSENT,
	TILE_PRESENT,
	TILE_CORRECT
} TileState;

// TODO: Prevent duplicate wrong guesses
// TODO: Check if word is a valid guess

int main() {

	int width = 600;
	int height = 600;

	InitWindow(width, height, "Wordle Clone");
	SetTargetFPS(24);

	//const char *words[] = {"ATLAS", "BRAIN", "BREAK", "FIRST", "STALE", "WORDS"};

	char target[WORD_LEN + 1] = "BRAIN";

	char guesses[MAX_GUESSES][WORD_LEN + 1] = {0};
	TileState feedback[MAX_GUESSES][WORD_LEN] = {0};

	int currentRow = 0;
	int currentCol = 0;

	int gameWon = 0;
	int gameOver = 0;

	int duplicateGuesses = 0;

	while(!WindowShouldClose()) {

		if (!gameOver) {
			for (int key = KEY_A; key <= KEY_Z; key++) {
				if (IsKeyPressed(key) && currentCol < WORD_LEN) {
					guesses[currentRow][currentCol] = KEY_A + (key - KEY_A);
					currentCol++;
				}
			}

			if (IsKeyPressed(KEY_BACKSPACE) && currentCol > 0) {
				currentCol--;
				guesses[currentRow][currentCol] = '\0';
			}

			if (IsKeyPressed(KEY_ENTER) && currentCol == WORD_LEN) {

				for (int i = currentRow-1; i >= 0; i--) {
					if (memcmp(guesses[currentRow], guesses[i], sizeof(guesses[i])) == 0) {
						//printf("'%s' is the same as '%s'.\n", guesses[currentRow], guesses[i]);
						duplicateGuesses += 1;
					}
				}

				for (int i = 0; i < WORD_LEN; i++) {
					feedback[currentRow][i] = TILE_ABSENT;
				}

				int targetChar[26] = {0};
				for (int i = 0; i < WORD_LEN; i++) {
					char c = target[i];
					targetChar[c - KEY_A]++;
				}

				for (int i = 0; i < WORD_LEN; i++) {
					char guess = guesses[currentRow][i];
					if (guess == target[i]) {
						feedback[currentRow][i] = TILE_CORRECT;
						targetChar[guess - KEY_A]--;
					}
				}

				for (int i = 0; i < WORD_LEN; i++) {
					if (feedback[currentRow][i] == TILE_CORRECT) continue;
					char guess = guesses[currentRow][i];
					if (guess >= KEY_A && guess <= KEY_Z && targetChar[guess - KEY_A] > 0) {
						feedback[currentRow][i] = TILE_PRESENT;
						targetChar[guess - KEY_A]--;
					}
				}

				int rowCorrect = 1;
				for (int i = 0; i < WORD_LEN; i++) {
					if (feedback[currentRow][i] != TILE_CORRECT) {
						rowCorrect = 0;
						break;
					}
				}

				if (rowCorrect) {
					gameWon = 1;
					gameOver = 1;

				} else {
					currentRow++;
					currentCol = 0;

					if (currentRow >= MAX_GUESSES) {
						gameOver = 1;
					}
				}
			}
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);

		int gridWidth = WORD_LEN * TILE_SIZE + (WORD_LEN - 1) * TILE_PADDING;
		int gridHeight = MAX_GUESSES * TILE_SIZE + (MAX_GUESSES - 1) * TILE_PADDING;
		int gridX = (width - gridWidth) / 2;
		int gridY = 80;

		for (int row = 0; row < MAX_GUESSES; row++) {
			for (int col = 0; col < WORD_LEN; col++) {
				int x = gridX + col * (TILE_SIZE + TILE_PADDING);
				int y = gridY + row * (TILE_SIZE + TILE_PADDING);

				TileState state = feedback[row][col];

				Color tileColor = RAYWHITE;

				Color green = { 108, 169, 101, 255 };
				Color gold = { 201, 180, 88, 255 };

				if (state == TILE_ABSENT) tileColor = DARKGRAY;
				if (state == TILE_PRESENT) tileColor = gold;
				if (state == TILE_CORRECT) tileColor = green;


				if (row == currentRow && !gameOver && state == TILE_UNKNOWN) {
					tileColor = RAYWHITE;
				}

				DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, tileColor);
				DrawRectangleLines(x, y, TILE_SIZE, TILE_SIZE, DARKGRAY);

				char letter = guesses[row][col];
				if (letter != 0) {
					char text[2] = {letter, 0};
					int fontSize = 32;
					int textWidth = MeasureText(text, fontSize);

					int tx = x + (TILE_SIZE - textWidth) / 2;
					int ty = y + (TILE_SIZE - fontSize) / 2;
					DrawText(text, tx, ty, fontSize, BLACK);

				}

			}
		}

		if (gameOver) {
			if (gameWon && currentRow < 3) {
				DrawText("SPLENDID. Game won!", 120, gridY + gridHeight + 40, 24, DARKGREEN);
			} else if (gameWon) {
				DrawText("Game won!", 120, gridY + gridHeight + 40, 24, DARKGREEN);
			} else {
				DrawText(TextFormat("Game lost. Word: %s", target), 120, gridY + gridHeight + 40, 24, DARKGRAY);
			}

		} else {
			DrawText("Guess a 5-letter word. Press Enter to submit.", 60, gridY + gridHeight + 40, 20, BLACK);
			if (duplicateGuesses) {
				DrawText(TextFormat("%d duplicate guesses present.", duplicateGuesses), 120, gridY + gridHeight + 80, 24, RED);
			}
		}
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
