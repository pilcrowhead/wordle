#include "raylib.h"

int main() {

	int w = 600;
	int h = 600;

	InitWindow(w, h, "Test Window");

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("test text", w / 2, h / 2, 50, LIGHTGRAY);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
