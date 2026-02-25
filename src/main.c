#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


// --- CONSTANTS ------------>

#define WIDTH 900
#define HEIGHT 600
#define FPS 60

#define NBOIDS 200
#define NPREDATORS ((NBOIDS) / 10)

#define SEP_RADIUS 25.0f
#define ALI_RADIUS 50.0f
#define COH_RADIUS 60.0f

#define SEP_WEIGHT 1.5f
#define ALI_WEIGHT 1.2f
#define COH_WEIGHT 1.0f

#define MAX_SPEED 2.5f
#define MAX_TURN 0.04f

#define MAX_PRED_SPEED 3.0f
#define FLEE_RADIUS 100.0f
#define FLEE_WEIGHT 4.0f
#define EAT_RADIUS 6.0f

#define PRED_CHASE_RADIUS 200.0f

#define FOV_DEGREES 270.0f
#define FOV_COS (cosf((FOV_DEGREES * DEG2RAD) * 0.5f))


// --- STRUCTURES ------------>

typedef struct s_vec2
{
	float x, y;
} Vec2;

typedef enum { PREY, PREDATOR } BoidType;

typedef struct s_boid
{
	Vec2 position, velocity;
	float radius;
	BoidType type;
} Boid;


// --- PROTOTYPES ------------>

float Vec2Length(Vec2);
Vec2 Vec2Add(Vec2, Vec2);
Vec2 Vec2Sub(Vec2, Vec2);
Vec2 Vec2Scale(Vec2, float);
Vec2 Vec2Normalize(Vec2);

void InitBoids(void);
Vec2 SeparationForce(int);
Vec2 AlignmentForce(int);
Vec2 CohesionForce(int);
Vec2 FleeForce(int);
Vec2 ChaseForce(int);
void EatBoids(void);
void UpdateBoids(void);
void DrawBoids(void);

void LimitTurn(Boid*, Vec2);
int IsVisible(int, int, float);
Vec2 ForwardBias(int);


// --- ENTRY POINT ------------>

Boid boids[NBOIDS];

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "Boids");
    SetTargetFPS(FPS);

	InitBoids();

	while (!WindowShouldClose())
    {
		EatBoids();
		BeginDrawing();
			ClearBackground(BLACK);
			UpdateBoids();
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

        boids[i].type   = (i < NPREDATORS) ? PREDATOR : PREY;
        boids[i].radius = (i < NPREDATORS) ? 5.0f : 4.0f;
    }
}

Vec2 SeparationForce(int i)
{
	Vec2 force = {0, 0};

    for (int j = 0; j < NBOIDS; j++)
    {
        if (i == j) continue;

        Vec2 diff = Vec2Sub(boids[i].position, boids[j].position);
        float d = Vec2Length(diff);

        if (d > 0 && d < SEP_RADIUS)
            force = Vec2Add(force, Vec2Scale(Vec2Normalize(diff), 1.0f / d));
    }
    return force;
}

Vec2 AlignmentForce(int i)
{
    Vec2 avg = {0, 0};
    int count = 0;

    for (int j = 0; j < NBOIDS; j++)
    {
        if (i == j) continue;
        if (!IsVisible(i, j, ALI_RADIUS)) continue;

        avg = Vec2Add(avg, boids[j].velocity);
        count++;
    }

    if (count == 0) return (Vec2){0,0};

    avg = Vec2Scale(avg, 1.0f / count);

    Vec2 steer = Vec2Sub(avg, boids[i].velocity);
    float mag = Vec2Length(steer);

    if (mag > 0) steer = Vec2Scale(Vec2Normalize(steer), mag / ALI_RADIUS);

    return steer;
}

Vec2 CohesionForce(int i)
{
    Vec2 center = {0, 0};
    int count = 0;

    for (int j = 0; j < NBOIDS; j++)
    {
        if (i == j) continue;
        if (!IsVisible(i, j, COH_RADIUS)) continue;

        center = Vec2Add(center, boids[j].position);
        count++;
    }

    if (count == 0) return (Vec2){0,0};

    center = Vec2Scale(center, 1.0f / count);

    Vec2 toCenter = Vec2Sub(center, boids[i].position);
    float dist = Vec2Length(toCenter);

    if (dist <= 0) return (Vec2){0,0};

    float strength = dist / COH_RADIUS;

    return Vec2Scale(Vec2Normalize(toCenter), strength);
}

Vec2 FleeForce(int i)
{
    Vec2 force = {0, 0};

    for (int j = 0; j < NBOIDS; j++)
    {
        if (boids[j].type != PREDATOR) continue;

        Vec2 diff = Vec2Sub(boids[i].position, boids[j].position);
        float d = Vec2Length(diff);

        if (d > 0 && d < FLEE_RADIUS)
            force = Vec2Add(force, Vec2Scale(Vec2Normalize(diff), (FLEE_RADIUS - d) / FLEE_RADIUS));
    }

    return force;
}

Vec2 ChaseForce(int i)
{
    Vec2 closest = {0, 0};
    float minDist = PRED_CHASE_RADIUS;
    int found = 0;

    for (int j = 0; j < NBOIDS; j++)
    {
        if (boids[j].type != PREY) continue;

        Vec2 diff = Vec2Sub(boids[j].position, boids[i].position);
        float d = Vec2Length(diff);

        if (d < minDist)
        {
            minDist = d;
            closest = diff;
            found = 1;
        }
    }

    if (!found) return (Vec2){0, 0};

    return Vec2Normalize(closest);
}

void EatBoids(void)
{
    for (int i = 0; i < NBOIDS; i++)
    {
        if (boids[i].type != PREDATOR) continue;

        for (int j = 0; j < NBOIDS; j++)
        {
            if (boids[j].type != PREY) continue;

            Vec2 diff = Vec2Sub(boids[i].position, boids[j].position);
            float d = Vec2Length(diff);

            if (d < EAT_RADIUS)
            {
                // Respawn prey at random location
                boids[j].position = (Vec2){
                    rand() % WIDTH,
                    rand() % HEIGHT
                };
                boids[j].velocity = (Vec2){
                    ((float)rand() / RAND_MAX) * 2 - 1,
                    ((float)rand() / RAND_MAX) * 2 - 1
                };
            }
        }
    }
}

void UpdateBoids(void)
{
    for (int i = 0; i < NBOIDS; i++)
    {
        Vec2 desired = boids[i].velocity;

        if (boids[i].type == PREDATOR)
        {
            desired = Vec2Add(desired, ChaseForce(i));
        }
        else
        {
            desired = Vec2Add(desired, Vec2Scale(SeparationForce(i), SEP_WEIGHT));
            desired = Vec2Add(desired, Vec2Scale(AlignmentForce(i), ALI_WEIGHT));
            desired = Vec2Add(desired, Vec2Scale(CohesionForce(i), COH_WEIGHT));
            desired = Vec2Add(desired, ForwardBias(i));
            desired = Vec2Add(desired, Vec2Scale(FleeForce(i), FLEE_WEIGHT));
        }

        LimitTurn(&boids[i], desired);

        float maxSpd = (boids[i].type == PREDATOR) ? MAX_PRED_SPEED : MAX_SPEED;
        float speed = Vec2Length(boids[i].velocity);
        if (speed > maxSpd)
            boids[i].velocity = Vec2Scale(Vec2Normalize(boids[i].velocity), maxSpd);

        boids[i].position = Vec2Add(boids[i].position, boids[i].velocity);

        if (boids[i].position.x < 0) boids[i].position.x += WIDTH;
        if (boids[i].position.y < 0) boids[i].position.y += HEIGHT;
        if (boids[i].position.x > WIDTH) boids[i].position.x -= WIDTH;
        if (boids[i].position.y > HEIGHT) boids[i].position.y -= HEIGHT;
    }
}

void DrawBoids(void)
{
    for (int i = 0; i < NBOIDS; i++)
    {
        float angle = atan2f(boids[i].velocity.y, boids[i].velocity.x);

        float size = boids[i].radius;
        float cx = boids[i].position.x;
        float cy = boids[i].position.y;

        // Tip (front)
        Vector2 tip = {
            cx + cosf(angle) * size * 1.5f,
            cy + sinf(angle) * size * 1.5f
        };

        // Back-left
        Vector2 left = {
            cx + cosf(angle + 2.5f) * size,
            cy + sinf(angle + 2.5f) * size
        };

        // Back-right
        Vector2 right = {
            cx + cosf(angle - 2.5f) * size,
            cy + sinf(angle - 2.5f) * size
        };

		Color c = (boids[i].type == PREDATOR) ? RED : GREEN;
        DrawTriangle(tip, right, left, c);
		DrawTriangleLines(tip, right, left, GRAY);
    }
}

void LimitTurn(Boid* b, Vec2 desired)
{
    float currentAngle = atan2f(b->velocity.y, b->velocity.x);
    float desiredAngle = atan2f(desired.y, desired.x);

    float diff = desiredAngle - currentAngle;

    while (diff >  (float)PI) diff -= 2.0f * (float)PI;
    while (diff < -(float)PI) diff += 2.0f * (float)PI;

    if (fabsf(diff) < MAX_TURN) currentAngle = desiredAngle;
    else currentAngle += (diff > 0 ? MAX_TURN : -MAX_TURN);

    float speed = Vec2Length(b->velocity);
    b->velocity = (Vec2){ cosf(currentAngle) * speed, sinf(currentAngle) * speed };
}

int IsVisible(int i, int j, float radius)
{
    Vec2 to = Vec2Sub(boids[j].position, boids[i].position);
    float dist = Vec2Length(to);

    if (dist <= 0 || dist > radius)
        return 0;

    Vec2 dir = Vec2Normalize(boids[i].velocity);
    Vec2 toNorm = Vec2Normalize(to);

    float dot = dir.x * toNorm.x + dir.y * toNorm.y;
    return dot >= FOV_COS;
}

Vec2 ForwardBias(int i)
{
    Vec2 v = boids[i].velocity;
    float l = Vec2Length(v);
    if (l == 0) return (Vec2){0,0};

    return Vec2Scale(Vec2Normalize(v), 0.02f);
}