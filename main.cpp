#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

struct Ball
{
	Vector2 pos;
	Vector2 vel;
	float radius;
	float ac;
};


const int WIDTH = 850;
const int HEIGHT = 450;

void UpdateDrawFrame();

Ball ball;
bool ball_selected;

std::vector<Rectangle> recs;

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
	ball.radius = 10.f;
	ball.pos = {WIDTH/2-ball.radius/2, HEIGHT/2-ball.radius/2};
	ball.vel = {0, 0};

	recs = {{100, 100, 80, 95}, {400, 300, 200, 100}};

	#if defined(PLATFORM_WEB)
    	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
	#else
		SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
		//--------------------------------------------------------------------------------------

		// Main game loop
		while (!WindowShouldClose())    // Detect window close button or ESC key
		{
			UpdateDrawFrame();
		}
	#endif

	CloseWindow();

	return 0;
}

void ShootBall()
{
	Vector2 dir = Vector2Subtract(GetMousePosition(), ball.pos);
	float angle = atan2f(dir.y, dir.x) * RAD2DEG;
	float dir_x = cosf(angle*DEG2RAD);
	float dir_y = sinf(angle*DEG2RAD);

	float distance = Vector2Length(dir);
	Vector2 normalized = Vector2Normalize(dir);

	ball.vel = Vector2Negate(Vector2Scale(normalized, distance * 8.0f * GetFrameTime()));

	std::cout << "Ball Shot!\n";
}

void UpdateDrawFrame()
{
	float friction = 0.98f;
	ball.vel = Vector2Scale(ball.vel, friction);
	if (Vector2Length(ball.vel) < 0.05f) ball.vel = {0, 0};

	ball.ac = 0.2f;
	if (ball.vel.x > 0) ball.vel.x -= ball.ac;
	if (ball.vel.x < 0) ball.vel.x += ball.ac;
	
	if (ball.vel.y > 0) ball.vel.y -= ball.ac;
	if (ball.vel.y < 0) ball.vel.y += ball.ac;
	
	ball.pos = Vector2Add(ball.vel, ball.pos);
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), ball.pos, ball.radius))
	{
		ball_selected = true;
	}

	if (IsMouseButtonUp(MOUSE_BUTTON_LEFT) && ball_selected)
	{
		ball_selected = false;
		ShootBall();
	}

	if (ball.pos.x - ball.radius/2 > WIDTH) { ball.pos.x = WIDTH-ball.radius/2; ball.vel.x *= -1; }
	if (ball.pos.x + ball.radius/2 < 0) { ball.pos.x = ball.radius/2; ball.vel.x *= -1; }

	if (ball.pos.y - ball.radius/2 > HEIGHT) { ball.pos.y = HEIGHT-ball.radius/2; ball.vel.y *= -1; }
	if (ball.pos.y + ball.radius/2 < 0) { ball.pos.y = ball.radius/2; ball.vel.y *= -1; }

	BeginDrawing();
	
	ClearBackground(GREEN);

	DrawCircleV(ball.pos, ball.radius, WHITE);

	DrawCircle(ball.pos.x-ball.radius, ball.pos.y, 2.f, PURPLE);

	if (ball_selected)
	{
		DrawLineV(ball.pos, GetMousePosition(), BLACK);
	}

	for (Rectangle& rec : recs)
	{
		DrawRectangleRec(rec, DARKGREEN);

		if (CheckCollisionCircleRec(ball.pos, ball.radius, rec))
		{
			// Top
			if (ball.vel.y > 0 && ball.pos.y + ball.radius > rec.y && ball.pos.y < rec.y)
			{
				ball.pos.y = rec.y-ball.radius;
				ball.vel.y *= -1;
			}
			
			// Right
			if (ball.vel.x < 0 && ball.pos.x - ball.radius < rec.x + rec.width && ball.pos.x > rec.x + rec.width)
			{
				ball.pos.x = rec.x + rec.width + ball.radius;
				ball.vel.x *= -1;
			}
		}
	}

	EndDrawing();
}