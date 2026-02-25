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

float Length(Vec2);
Vec2 Add(Vec2, Vec2);
Vec2 Sub(Vec2, Vec2);
Vec2 Scale(Vec2, float);
Vec2 Normalize(Vec2);

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

	InitBoids();

	while (!WindowShouldClose())
    {
		BeginDrawing();
			ClearBackground(BLACK);

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

float Length(Vec2 v)
{
	return sqrtf(v.x*v.x + v.y*v.y);
}

Vec2 Add(Vec2 a, Vec2 b)
{
	return (Vec2){ a.x + b.x, a.y + b.y };
}

Vec2 Sub(Vec2 a, Vec2 b)
{
	return (Vec2){ a.x - b.x, a.y - b.y };
}

Vec2 Scale(Vec2 v, float s)
{
	return (Vec2){ v.x * s, v.y * s };
}

Vec2 Normalize(Vec2 v)
{
	float len = Length(v);
    if (len == 0) return (Vec2){0, 0};
    return Scale(v, 1.0f / len);
}

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