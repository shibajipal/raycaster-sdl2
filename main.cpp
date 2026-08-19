#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include<iostream>
#include <string>
#include <bits/stdc++.h>
using namespace std;
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

#define MAP_HEIGHT 24
#define MAP_WIDTH 24
const int world_map[MAP_HEIGHT][MAP_WIDTH] = {
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
  {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
  {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
  {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

void load_texture(int id, vector<Uint32> texture[8], const char* filepath){
		SDL_Surface* surface = IMG_Load(filepath);
		if (!surface){
			cout << "Failed to load textures!" << filepath << endl;
			return;
		}

		SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
		SDL_FreeSurface(surface);

		Uint32* pixels = (Uint32*)formatted -> pixels;
		for (int i = 0; i < (TEXTURE_WIDTH * TEXTURE_HEIGHT); i++){
			texture[id][i] = pixels[i];
		}

		SDL_FreeSurface(formatted);
	}



int main(int argc, char* args[]){
	double posX = 1.5, posY = 1.5;
	double dirX = 0.0, dirY = 1.0;
	double planeX = 0.66, planeY = 0.0;
	double time = 0.0, old_time = 0.0;

	Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	vector<Uint32> texture[8];
	

	for (int i = 0; i < 8; i++){
		texture[i].resize(TEXTURE_WIDTH * TEXTURE_HEIGHT);
	}
	
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
	

	//handling the textures
	load_texture(0, texture, "wall_textures/eagle.png");
	load_texture(1, texture, "wall_textures/redbrick.png");
	load_texture(2, texture, "wall_textures/purplestone.png");
	load_texture(3, texture, "wall_textures/greystone.png");
	load_texture(4, texture, "wall_textures/bluestone.png");
	load_texture(5, texture, "wall_textures/mossy.png");
	load_texture(6, texture, "wall_textures/wood.png");
	load_texture(7, texture, "wall_textures/colorstone.png");
	
	SDL_Texture* screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

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
			if (world_map[int(posX + dirX * move_speed)][int(posY)] == 0) posX += dirX * move_speed;
			if (world_map[int(posX)][int(posY + dirY * move_speed)] == 0) posY += dirY * move_speed;
		}
		
		if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]){
				if (world_map[int(posX - dirX * move_speed)][int(posY)] == 0) posX -= dirX * move_speed;
				if (world_map[int(posX)][int(posY - dirY * move_speed)] == 0) posY -= dirY * move_speed;
				}
		if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]){
			if (state[SDL_SCANCODE_LSHIFT]){
			// cout << "strafe left" << endl;
			if (world_map[int(posX - dirY * move_speed)][int(posY)] == 0) posX -= dirY * move_speed;
			if (world_map[int(posX)][int(posY + dirY * move_speed)] == 0) posY += dirX * move_speed;
			}
			
			else{
			// cout << "normal left" << endl;
			double old_dirX = dirX;
			dirX = dirX * cos(rot_speed) - dirY * sin(rot_speed);
			dirY = old_dirX * sin(rot_speed) + dirY * cos(rot_speed);

			double old_planeX = planeX;
			planeX = planeX * cos(rot_speed) - planeY * sin(rot_speed);
			planeY = old_planeX * sin(rot_speed) + planeY * cos(rot_speed);
		}}

		if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]){
			if ((state[SDL_SCANCODE_LSHIFT])){
				// cout << "strafe right" << endl;
				if (world_map[int(posX + dirY * move_speed)][int(posY)] == 0) posX += dirY * move_speed;
				if (world_map[int(posX)][int(posY - dirY * move_speed)] == 0) posY -= dirX * move_speed;
		}
			else{
		// cout << "normal right" << endl;
			double old_dirX = dirX;
			dirX = dirX * cos(-rot_speed) - dirY * sin(-rot_speed);
			dirY = old_dirX * sin(-rot_speed) + dirY * cos(-rot_speed);

			double old_planeX = planeX;
			planeX = planeX * cos(-rot_speed) - planeY * sin(-rot_speed);
			planeY = old_planeX * sin(-rot_speed) + planeY * cos(-rot_speed);	
			}
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
				if (world_map[mapX][mapY] > 0) wall_hit = 1;	
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
			// texturing stuff!
			int r, g, b;
			double wallX;
			int texture_num = world_map[mapX][mapY] - 1;
			if (side == 0) wallX = posY + perpWallDist * raydirY;
			else wallX = posX + perpWallDist * raydirX;
			wallX -= floor(wallX);

			int textureX = int(wallX * double(TEXTURE_WIDTH));
			if (side == 0 && raydirX > 0) textureX = TEXTURE_WIDTH - textureX - 1;
			if (side == 1 && raydirY < 0) textureX = TEXTURE_WIDTH - textureX - 1;
			
			double step = 1.0 * TEXTURE_HEIGHT / line_height;
			double texture_pos = (draw_start - SCREEN_HEIGHT / 2.0 + line_height / 2.0) * step;
			for (int y = 0; y < draw_start; y++){
				buffer[y][x] = 0xFF333333;
			}

			for (int y = draw_start; y < draw_end; y++){
				int textureY = (int)texture_pos & (TEXTURE_HEIGHT - 1);
				texture_pos += step;
				Uint32 color = texture[texture_num][TEXTURE_HEIGHT * textureY + textureX];

				if (side == 1) color = (color >> 1) & 8355711;
				buffer[y][x] = color;
			}

			for (int y = draw_end; y < SCREEN_HEIGHT; y++){
				buffer[y][x] = 0xFF555555;
		}



		}

		SDL_UpdateTexture(screen_texture, NULL, buffer, SCREEN_WIDTH * sizeof(Uint32));
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
		

		
	}
	SDL_DestroyTexture(screen_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}	
