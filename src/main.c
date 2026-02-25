#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


// --- CONSTANTS ------------>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60
#define NBOIDS 30


// --- STRUCTURES ------------>

typedef struct s_vec2
{
	float x, y;
} Vec2;

typedef struct s_boid
{
	Vec2 position, velocity;
	float radius;
} Boid;


// --- PROTOTYPES ------------>

void InitBoids(void);
void Separation(void);
void Alignment(void);
void Cohesion(void);
void DrawBoids(void);


// --- ENTRY POINT ------------>

Boid boids[NBOIDS];

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(FPS);

	while (!WindowShouldClose())
    {
		BeginDrawing();
			ClearBackground(BLACK);
			InitBoids();

			Separation();
			Alignment();
			Cohesion();

			DrawBoids();
		EndDrawing();
    }
    
    CloseWindow();

	return 0;
}


// --- IMPLEMENTATIONS ------------>

void InitBoids(void)
{
	return;
}

void Separation(void)
{
	return;
}

void Alignment(void)
{
	return;
}

void Cohesion(void)
{
	return;
}

void DrawBoids(void)
{
	return;
}