#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


// --- CONSTANTS ------------>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60


// --- ENTRY POINT ------------>

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(FPS);

	while (!WindowShouldClose())
    {
		BeginDrawing();
			ClearBackground(BLACK);
		EndDrawing();
    }
    
    CloseWindow();

	return 0;
}
