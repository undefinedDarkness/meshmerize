/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you
--  wrote the original software. If you use this software in a product, an acknowledgment
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "common.h"

#define MAZE_SCALE 16
#define SCALE(x) (x * MAZE_SCALE)
#define SCALEX(x) (x * MAZE_SCALE + MAZE_SCALE / 2)

static struct sim_t SimulationState = {0};

int main()
{
	// Tell the window to use vysnc and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// SimulationState.maze = LoadImage("maze4.png");
	// SimulationState.player = (struct position_t){
	// 	.x = 6, .y = 15, .direction = UP};

	SimulationState.maze = LoadImage("maze6.png");
	SimulationState.player = (struct position_t){
	.x = 78, .y = 46, .direction = UP};

	// SimulationState.maze = LoadImage("maze.png");
	// SimulationState.player = (struct position_t){
	// 	.x = 18, .y = 16, .direction = LEFT};

	InitWindow(SimulationState.maze.width * MAZE_SCALE, SimulationState.maze.height * MAZE_SCALE, "Hello Raylib");
	SimulationState.followed_path = (char *)malloc(2056);
	SimulationState.algorithm_state = (struct algorithm_state_t){};

	Texture maze = LoadTextureFromImage(SimulationState.maze);
	initAlgorithm(&SimulationState, &SimulationState.algorithm_state);
	bool autoTick = false;
	// ImageColorGrayscale(&SimulationState.maze);

	// game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		// drawing
		BeginDrawing();

		// Setup the backbuffer for drawing (clear color and depth buffers)
		ClearBackground(RAYWHITE);

		DrawTextureEx(maze, (Vector2){0, 0}, 0, MAZE_SCALE, WHITE);

		// Draw grid lines
		// for (int i = 0; i < 24; i++) {
		// 	DrawLine(SCALE(i), 0, SCALE(i), SCALE(24), (Color){ 0xaa, 0xaa, 0xaa, 0xff });
		// 	DrawLine(0, SCALE(i), SCALE(24), SCALE(i), (Color){ 0xaa, 0xaa, 0xaa, 0xff });
		// }

		// DrawText(TextFormat("Tick: %i", SimulationState.tick), 10, 10, 20, YELLOW);
		struct sensors_t sensors = getSensors(&SimulationState);
		// DrawText(TextFormat("Sensors: %d %d %d", sensors.left, sensors.front, sensors.right), 10, 40, 20, YELLOW);
		// DrawText(TextFormat("Player: %d %d", SimulationState.player.x, SimulationState.player.y), 10, 70, 20, YELLOW);

		AUTO visited = SimulationState.algorithm_state.mat;

		for (int i = 0; i < SimulationState.maze.height; i++)
			for (int j = 0; j < SimulationState.maze.width; j++)
			{

				if (visited[i][j].passable)
				{
					DrawRectangle(SCALE(j), SCALE(i), MAZE_SCALE, MAZE_SCALE, (Color){0x00, 0xff, 0x00, 0x33});
				}

				if (visited[i][j].visited)
				{
					// DrawRectangle(SCALE(j), SCALE(i), MAZE_SCALE, MAZE_SCALE, (Color){0xbb, 0xbb, 0xbb, 0xff});

					if (visited[i][j].inspected)
					{
						if (visited[i][j].exits.top)
							DrawLineEx((Vector2){SCALE(j), SCALE(i)}, (Vector2){SCALE(j) + MAZE_SCALE, SCALE(i)}, 2, (Color){0xff, 0x00, 0x00, 0xff});
						if (visited[i][j].exits.left)
							DrawLineEx((Vector2){SCALE(j), SCALE(i)}, (Vector2){SCALE(j), SCALE(i) + MAZE_SCALE}, 2, (Color){0xff, 0x00, 0x00, 0xff});
						if (visited[i][j].exits.right)
							DrawLineEx((Vector2){SCALE(j) + MAZE_SCALE, SCALE(i)}, (Vector2){SCALE(j) + MAZE_SCALE, SCALE(i) + MAZE_SCALE}, 2, (Color){0xff, 0x00, 0x00, 0xff});
						if (visited[i][j].exits.bottom)
							DrawLineEx((Vector2){SCALE(j), SCALE(i) + MAZE_SCALE}, (Vector2){SCALE(j) + MAZE_SCALE, SCALE(i) + MAZE_SCALE}, 2, (Color){0xff, 0x00, 0x00, 0xff});
					}

					// Draw same with different color if used
					if (visited[i][j].used.top)
						DrawLineEx((Vector2){SCALE(j), SCALE(i)}, (Vector2){SCALE(j) + MAZE_SCALE, SCALE(i)}, 2, (Color){0x00, 0xff, 0x00, 0xff});
					if (visited[i][j].used.left)
						DrawLineEx((Vector2){SCALE(j), SCALE(i)}, (Vector2){SCALE(j), SCALE(i) + MAZE_SCALE}, 2, (Color){0x00, 0xff, 0x00, 0xff});
					if (visited[i][j].used.right)
						DrawLineEx((Vector2){SCALE(j) + MAZE_SCALE, SCALE(i)}, (Vector2){SCALE(j) + MAZE_SCALE, SCALE(i) + MAZE_SCALE}, 2, (Color){0x00, 0xff, 0x00, 0xff});
					if (visited[i][j].used.bottom)
						DrawLineEx((Vector2){SCALE(j), SCALE(i) + MAZE_SCALE}, (Vector2){SCALE(j) + MAZE_SCALE, SCALE(i) + MAZE_SCALE}, 2, (Color){0x00, 0xff, 0x00, 0xff});
				}
			}

		if (SimulationState.optimalPath.found)
		{
			for (int i = 0; i < SimulationState.optimalPath.length; i++)
			{
				struct coordinate_t p = SimulationState.optimalPath.path[i];
				DrawRectangle(SCALE(p.x), SCALE(p.y), MAZE_SCALE, MAZE_SCALE, (Color){0xf8, 0xde, 0x7e, 0xff});
			}
		}

		// DrawCircle(SimulationState.player.x * MAZE_SCALE + MAZE_SCALE / 2, SimulationState.player.y * MAZE_SCALE + MAZE_SCALE / 2, 8, VIOLET);
		DrawCircle(SCALEX(SimulationState.player.x), SCALEX(SimulationState.player.y), 8, VIOLET);

		switch (SimulationState.player.direction)
		{
		case LEFT:
			DrawLineEx(
				(Vector2){SCALEX(SimulationState.player.x) - MAZE_SCALE, SCALEX(SimulationState.player.y) - MAZE_SCALE * 1.5},
				(Vector2){SCALEX(SimulationState.player.x) - MAZE_SCALE, SCALEX(SimulationState.player.y) + MAZE_SCALE * 1.5},
				MAZE_SCALE, (Color){0x00, 0x00, 0xff, 0xaa});
			break;
		case RIGHT:
			DrawLineEx(
				(Vector2){SCALEX(SimulationState.player.x) + MAZE_SCALE, SCALEX(SimulationState.player.y) - MAZE_SCALE * 1.5},
				(Vector2){SCALEX(SimulationState.player.x) + MAZE_SCALE, SCALEX(SimulationState.player.y) + MAZE_SCALE * 1.5},
				MAZE_SCALE, (Color){0x00, 0x00, 0xff, 0xaa});
			break;
		case UP:
			DrawLineEx(
				(Vector2){SCALEX(SimulationState.player.x) - MAZE_SCALE * 1.5, SCALEX(SimulationState.player.y) - MAZE_SCALE},
				(Vector2){SCALEX(SimulationState.player.x) + MAZE_SCALE * 1.5, SCALEX(SimulationState.player.y) - MAZE_SCALE},
				MAZE_SCALE, (Color){0x00, 0x00, 0xff, 0xaa});
			break;
		case DOWN:
			DrawLineEx(
				(Vector2){SCALEX(SimulationState.player.x) - MAZE_SCALE * 1.5, SCALEX(SimulationState.player.y) + MAZE_SCALE},
				(Vector2){SCALEX(SimulationState.player.x) + MAZE_SCALE * 1.5, SCALEX(SimulationState.player.y) + MAZE_SCALE},
				MAZE_SCALE, (Color){0x00, 0x00, 0xff, 0xaa});
			break;
		}

		struct position_t prev_player = SimulationState.player;

		if (IsKeyPressed(KEY_SPACE))
		{
			// printf("--- TICK ---\n");
			nextTick(&SimulationState);
		}

		if (IsKeyPressed(KEY_UP))
		{
			straight(&SimulationState);
			// checkPosition(&SimulationState);
		}
		else if (IsKeyPressed(KEY_LEFT))
		{
			turnLeft(&SimulationState);
			// checkPosition(&SimulationState);
		}
		else if (IsKeyPressed(KEY_RIGHT))
		{
			turnRight(&SimulationState);
			// checkPosition(&SimulationState);
		}
		else if (IsKeyPressed(KEY_DOWN))
		{
			turnAround(&SimulationState);
			// checkPosition(&SimulationState);
		}
		else if (IsKeyPressed((KEY_P)))
		{
			SimulationState.followed_path[SimulationState.tick] = '\0';
			printf("PATH: %s\n", SimulationState.followed_path);
		}
		else if (IsKeyPressed(KEY_B))
		{
			printVisitedCells(&SimulationState.algorithm_state);
		}
		else if (IsKeyPressed(KEY_A))
		{
			autoTick = !autoTick;
		}
		else if (IsKeyPressed(KEY_C))
		{
			findOptimalPath(&SimulationState);
		}

		if (autoTick)
		{
			nextTick(&SimulationState);
		}

		if (invalidPlayerState(&SimulationState))
		{
			printf("INVALID POSITION\n");
			SimulationState.player = prev_player;
		}

		// Draw mouse coordinates on screen
		// relative to grid
		DrawText(TextFormat("Mouse Position: [%i, %i]", GetMouseX() / MAZE_SCALE, GetMouseY() / MAZE_SCALE), 5, 5, 20, DARKGRAY);
		DrawText(TextFormat("Passable: %s", visited[GetMouseY() / MAZE_SCALE][GetMouseX() / MAZE_SCALE].passable ? "true" : "false"), 5, 25, 20, DARKGRAY);
		// DrawText(TextFormat("Mouse Position: [%i, %i]", GetMouseX(), GetMouseY()), 10, 10, 20, DARKGRAY);

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	// UnloadTexture(wabbit);

	// destory the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
