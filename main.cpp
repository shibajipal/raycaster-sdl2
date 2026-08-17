#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cmath>
#include<iostream>
#include <string>
using namespace std;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int MAP_WIDTH = 16;
const int MAP_HEIGHT = 16;
const int map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 2, 2, 2, 1}, 
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 0, 1},
    {1, 1, 0, 1, 1, 1, 1, 0, 0, 2, 2, 0, 2, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 3, 3, 3, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 3, 0, 0, 0, 0, 0, 3, 0, 4, 4, 4, 4, 4, 1},
    {1, 0, 3, 0, 5, 5, 5, 0, 3, 0, 4, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 5, 0, 5, 0, 0, 0, 4, 0, 0, 0, 0, 1},
    {1, 0, 3, 0, 5, 0, 5, 0, 3, 0, 4, 0, 0, 0, 0, 1},
    {1, 0, 3, 3, 3, 0, 3, 3, 3, 0, 4, 4, 4, 4, 4, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

int main(int argc, char* args[]){
	double posX = 2.5, posY = 2.5;
	double dirX = 0.0, dirY = 1.0;
	double planeX = 0.66, planeY = 0.0;
	double time = 0.0, old_time = 0.0;
	if (SDL_Init(SDL_INIT_VIDEO) < 0){
		cout << "SDL could not be initialized. Error: " << SDL_GetError() << endl;
	}

	SDL_Window* window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == nullptr){
		cout << "Window could not be created. Error: " << SDL_GetError() << endl;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	bool is_running = true;
	SDL_Event event;

	// starting off with the entire loop
	
	while (is_running){
		// timing and fps stuff
		old_time = time;
		time = SDL_GetTicks();
		double frame_time = (time - old_time) / 1000.0;
		int fps = (frame_time > 0) ? static_cast<int>(1.0 / frame_time) : 1e9;
		string title = "Raycaster | FPS: " + to_string(fps);
		SDL_SetWindowTitle(window, title.c_str());
		double move_speed = frame_time * 5.0;
		double rot_speed = frame_time * 3.0;


		while (SDL_PollEvent(&event) != 0){
			if (event.type == SDL_QUIT){
				is_running = false;
			}}


		// handling movements, using WASD or corresponding arrow keys
        const Uint8* state = SDL_GetKeyboardState(NULL);

		if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]){
			if (map[int(posX + dirX * move_speed)][int(posY)] == 0) posX += dirX * move_speed;
			if (map[int(posX)][int(posY + dirY * move_speed)] == 0) posY += dirY * move_speed;
		}
		
		if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]){
				if (map[int(posX - dirX * move_speed)][int(posY)] == 0) posX -= dirX * move_speed;
				if (map[int(posX)][int(posY - dirY * move_speed)] == 0) posY -= dirY * move_speed;
				}
		
		if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]){
			double old_dirX = dirX;
			dirX = dirX * cos(rot_speed) - dirY * sin(rot_speed);
			dirY = old_dirX * sin(rot_speed) + dirY * cos(rot_speed);

			double old_planeX = planeX;
			planeX = planeX * cos(rot_speed) - planeY * sin(rot_speed);
			planeY = old_planeX * sin(rot_speed) + planeY * cos(rot_speed);
		}

		if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]){
			double old_dirX = dirX;
			dirX = dirX * cos(-rot_speed) - dirY * sin(-rot_speed);
			dirY = old_dirX * sin(-rot_speed) + dirY * cos(-rot_speed);

			double old_planeX = planeX;
			planeX = planeX * cos(-rot_speed) - planeY * sin(-rot_speed);
			planeY = old_planeX * sin(-rot_speed) + planeY * cos(-rot_speed);
		}

		SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
		SDL_RenderClear(renderer);


		for (int x = 0; x < SCREEN_WIDTH; x++){
			// calculating exact direction of the ray
			double cameraX = 2 * x / double(SCREEN_WIDTH) - 1;
			double raydirX = dirX + planeX * cameraX;
			double raydirY = dirY + planeY * cameraX;

			// find the box
			int mapX = int(posX);
			int mapY = int(posY);

			// length to immediate next x or y side is called side distance
			double sidedistX;
			double sidedistY;

			// length between two consectuive x or y sides is called delta distance
			double deltadistX = (raydirX == 0) ? 1e30 : abs(1 / raydirX);
			double deltadistY = (raydirY == 0) ? 1e30 : abs(1 / raydirY);
			double perpWallDist;
			
			// basically which direction to move in, it's simply +1 or -1
			int stepX;
			int stepY;

			
			int wall_hit = 0;
			int side;

			stepX = (raydirX < 0) ? -1 : 1;
			stepY = (raydirY < 0) ? -1 : 1;

			sidedistX = (raydirX < 0) ? (posX - mapX) * deltadistX : (mapX + 1.0 - posX) * deltadistX;
			sidedistY = (raydirY < 0) ? (posY - mapY) * deltadistY : (mapY + 1.0 - posY) * deltadistY;

			// starting the DDA!
			while (wall_hit == 0){
				if (sidedistX < sidedistY){
					sidedistX += deltadistX;
					mapX += stepX;
					side = 0;
				}
				else{
					sidedistY += deltadistY;
					mapY += stepY;
					side = 1;
				}	
				// added an extra boundary check
				if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT){
					wall_hit = 1;
					break;
				}
				if (map[mapX][mapY] > 0) wall_hit = 1;	
			}
			// long ass calculation gets simplified to this
			if (side == 0) perpWallDist = (sidedistX - deltadistX);
			else perpWallDist = (sidedistY - deltadistY);


			int line_height = (int)(SCREEN_HEIGHT / perpWallDist);
			int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
			int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
			draw_start = (draw_start < 0) ? 0 : draw_start;
			draw_end = (draw_end >= SCREEN_HEIGHT) ? (SCREEN_HEIGHT - 1) : draw_end;
			

			mapX = (mapX < 0) ? 0 : mapX;
			mapX = (mapX >= MAP_WIDTH) ? MAP_WIDTH - 1 : mapX;
			mapY = (mapY < 0) ? 0 : mapY;
			mapY = (mapY >= MAP_HEIGHT) ? MAP_HEIGHT - 1 : mapY;
			// coloring stuff!
			int r, g, b;
			switch (map[mapX][mapY]){
				case 1: r = 255, g = 0, b = 0;
						break;
				case 2: r = 0, g = 255, b = 0;
						break;
				case 3: r = 0, g = 0, b = 255;
						break;
				case 4: r = 255, g = 255, b = 0;
						break;
				case 5: r = 255, g = 0, b = 255;
						break;
				case 6: r = 0, g = 255, b = 255;
						break;
				default: r = 255, g = 255, b = 0;
						 break;
			}
			if (side == 1){
				r /= 2;
				g /= 2;
				b /= 2;
		}
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
			SDL_RenderDrawLine(renderer, x, draw_start, x, draw_end);
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
		

		
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
		}	
