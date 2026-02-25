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

float Vec2Length(Vec2);
Vec2 Vec2Add(Vec2, Vec2);
Vec2 Vec2Sub(Vec2, Vec2);
Vec2 Vec2Scale(Vec2, float);
Vec2 Vec2Normalize(Vec2);

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

float Vec2Length(Vec2 v)
{
	return sqrtf(v.x*v.x + v.y*v.y);
}

Vec2 Vec2Add(Vec2 a, Vec2 b)
{
	return (Vec2){ a.x + b.x, a.y + b.y };
}

Vec2 Vec2Sub(Vec2 a, Vec2 b)
{
	return (Vec2){ a.x - b.x, a.y - b.y };
}

Vec2 Vec2Scale(Vec2 v, float s)
{
	return (Vec2){ v.x * s, v.y * s };
}

Vec2 Vec2Normalize(Vec2 v)
{
	float len = Vec2Length(v);
    if (len == 0) return (Vec2){0, 0};
    return Vec2Scale(v, 1.0f / len);
}

void InitBoids(void)
{
	srand(time(NULL));

    for (int i = 0; i < NBOIDS; i++)
    {
        boids[i].position = (Vec2){
            rand() % WIDTH,
            rand() % HEIGHT
        };

        boids[i].velocity = (Vec2){
            ((float)rand() / RAND_MAX) * 2 - 1,
            ((float)rand() / RAND_MAX) * 2 - 1
        };

        boids[i].radius = 4.0f;
    }
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
	for (int i = 0; i < NBOIDS; i++)
    {
        boids[i].position = Vec2Add(boids[i].position, boids[i].velocity);

        if (boids[i].position.x < 0) boids[i].position.x = WIDTH;
        if (boids[i].position.y < 0) boids[i].position.y = HEIGHT;
        if (boids[i].position.x > WIDTH) boids[i].position.x = 0;
        if (boids[i].position.y > HEIGHT) boids[i].position.y = 0;

        DrawCircleV(
            (Vector2){ boids[i].position.x, boids[i].position.y },
            boids[i].radius,
            WHITE
        );
    }
}