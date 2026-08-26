#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
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

int world_map[MAP_WIDTH][MAP_HEIGHT] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,8,8,0,0,0,0,2,2,0,0,2,2,0,0,0,0,5,5,0,0,1},
  {1,0,0,8,8,0,0,0,0,2,2,0,0,2,2,0,0,0,0,5,5,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {1,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,0,0,1},
  {1,0,0,2,2,0,0,0,0,0,0,3,3,0,0,0,0,0,0,6,6,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,1},
  {1,0,0,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,1},
  {1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,6,6,0,0,0,0,2,2,0,0,2,2,0,0,0,0,8,8,0,0,1},
  {1,0,0,6,6,0,0,0,0,2,2,0,0,2,2,0,0,0,0,8,8,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void load_texture(int id, vector<Uint32> texture[12], const char* filepath){
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

Uint32 apply_fog_depth(Uint32 color, double distance, bool is_side_wall = false){

	if ((color & 0x00FFFFFF) == 0) return color;
	double max_distance = 22.0;
	double fog_intensity = 1.0 - (distance / max_distance);

	fog_intensity = max(0.0, fog_intensity);
	fog_intensity = min(1.0, fog_intensity);


	

	// const double fog_r = 30.0, fog_g = 15.0, fog_b = 8.0;
	const double fog_r = 5.0, fog_g = 25.0, fog_b = 10.0;
	
	Uint8 a = (color >> 24) & 0xFF;
	Uint8 r = ((color >> 16) & 0xFF);
	Uint8 g = ((color >> 8) & 0xFF);
	Uint8 b = (color & 0xFF);

	if (is_side_wall){
		r /= 2;
		g /= 2;
		b /= 2;
	}
	r = (Uint8)(r * fog_intensity + fog_r * (1.0 - fog_intensity));
	g = (Uint8)(g * fog_intensity + fog_g * (1.0 - fog_intensity));
	b = (Uint8)(b * fog_intensity + fog_b * (1.0 - fog_intensity));

	return (0xFFu << 24) | (r << 16) | (g << 8) | b;

}

struct Sprite{
	double x;
	double y;
	int texture;
};

#define SPRITE_COUNT 30

Sprite sprite[SPRITE_COUNT] = {
    // a glowing perimeter and inner quad of green lights (10)
    {2.5, 2.5, 10}, {2.5, 21.5, 10}, {21.5, 2.5, 10}, {21.5, 21.5, 10},
    {11.5, 2.5, 10}, {11.5, 21.5, 10}, {2.5, 11.5, 10}, {21.5, 11.5, 10},
    {7.5, 7.5, 10}, {16.5, 16.5, 10}, {7.5, 16.5, 10}, {16.5, 7.5, 10},

    // a tight ring of pillars (9) framing the central altar
    {10.0, 10.0, 9}, {11.5, 10.0, 9}, {12.5, 10.0, 9}, {14.0, 10.0, 9},
    {10.0, 14.0, 9}, {11.5, 14.0, 9}, {12.5, 14.0, 9}, {14.0, 14.0, 9},
    {10.0, 11.5, 9}, {10.0, 12.5, 9}, {14.0, 11.5, 9}, {14.0, 12.5, 9},

    // clusters of barrels (8) stashed near the chunky wall blocks
    {4.5, 4.5, 8}, {4.8, 4.2, 8}, {4.2, 4.8, 8},
    {19.5, 19.5, 8}, {19.8, 19.2, 8}, {19.2, 19.8, 8}
};
double Zbuffer[SCREEN_WIDTH];
int sprite_order[SPRITE_COUNT];

double sprite_distances[SPRITE_COUNT];

void sort_sprites(int* order, double* dist, int amount){
	vector<pair<double, int>> sprites(amount);
	for (int i = 0; i < amount; i++){
		sprites[i].first = dist[i];
		sprites[i].second = order[i];
	}

	sort(sprites.begin(), sprites.end());
	for (int i = 0; i < amount; i++){
		dist[i] = sprites[amount - i - 1].first;
		order[i] = sprites[amount - i - 1].second;
	}
}


int main(int argc, char* args[]){
	double posX = 1.5, posY = 1.5;
	double dirX = 0.0, dirY = 1.0;
	double planeX = 0.66, planeY = 0.0;
	double time = 0.0, old_time = 0.0;

	Uint32 buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	vector<Uint32> texture[12];
	

	for (int i = 0; i < 12; i++){
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
	bool is_floorcasting = true;
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
	load_texture(8, texture, "wall_textures/barrel.png");
	load_texture(9, texture, "wall_textures/pillar.png");
	load_texture(10, texture, "wall_textures/greenlight.png");
	load_texture(11, texture, "wall_textures/redbrick.png");
	
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
			}
			else if (event.type == SDL_KEYDOWN && event.key.repeat == 0){
			if (event.key.keysym.scancode == SDL_SCANCODE_C){
				if (event.key.keysym.mod & KMOD_ALT){
					is_floorcasting = !is_floorcasting;
					// cout << "change cast" << endl;
				}
			}
		}
		}


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
		
		// cout << "floorcasting " << is_floorcasting << endl;
		if (is_floorcasting){
		// 	cout << "we casting" << endl;
		for (int y = 0; y < SCREEN_HEIGHT; y++){
			float ray_dir_leftX = dirX - planeX;
			float ray_dir_leftY = dirY - planeY;

			float ray_dir_rightX = dirX + planeX;
			float ray_dir_rightY = dirY + planeY;

			int y_current = y - SCREEN_HEIGHT / 2;
			float posZ = 0.5 * SCREEN_HEIGHT;
			
			float row_distance = posZ / y_current;
			float floor_stepX = row_distance * (ray_dir_rightX - ray_dir_leftX) / SCREEN_WIDTH;
			float floor_stepY = row_distance * (ray_dir_rightY - ray_dir_leftY) / SCREEN_WIDTH;

			float floorX = posX + row_distance * ray_dir_leftX;
			float floorY = posY + row_distance * ray_dir_leftY;

			for (int x = 0; x < SCREEN_WIDTH; ++x){
				int cellX = (int)(floorX);
				int cellY = (int)(floorY);

				int tx = (int)(TEXTURE_WIDTH * (floorX - cellX)) & (TEXTURE_WIDTH - 1);
				int ty = (int)(TEXTURE_HEIGHT * (floorY - cellY)) & (TEXTURE_HEIGHT - 1);

				floorX += floor_stepX;
				floorY += floor_stepY;

				int floor_texture = 4;
				int ceiling_texture = 6;

				Uint32 color;

				color = texture[floor_texture][TEXTURE_WIDTH * ty + tx];
				// color = (color >> 1) & 8355711;
				color = apply_fog_depth(color, row_distance);
				buffer[y][x] = color;

				color = texture[ceiling_texture][TEXTURE_WIDTH * ty + tx];
				// color = (color >> 1) & 8355711;
				color = apply_fog_depth(color, row_distance);
				buffer[SCREEN_HEIGHT - y - 1][x] = color;
			}
		}	
		}


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
			
			if (!is_floorcasting){
			for (int y = 0; y < draw_start; y++){
				buffer[y][x] = 0xFF333333;
			}
			}

			for (int y = draw_start; y < draw_end; y++){
				int textureY = (int)texture_pos & (TEXTURE_HEIGHT - 1);
				texture_pos += step;
				Uint32 color = texture[texture_num][TEXTURE_HEIGHT * textureY + textureX];

				// if (side == 1) color = (color >> 1) & 8355711;

				color = apply_fog_depth(color, perpWallDist, side == 1);
				buffer[y][x] = color;
			}
			
			if (!is_floorcasting){
			for (int y = draw_end; y < SCREEN_HEIGHT; y++){
				buffer[y][x] = 0xFF555555;
			}
			}
			Zbuffer[x] = perpWallDist;
		}
		// sprite casting
		for (int i = 0; i < SPRITE_COUNT; i++){
			sprite_order[i] = i;
			sprite_distances[i] = ((posX - sprite[i].x) * (posX -sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y));

		}
		sort_sprites(sprite_order, sprite_distances, SPRITE_COUNT);

		for (int i = 0; i < SPRITE_COUNT; i++){
			double spriteX = sprite[sprite_order[i]].x - posX;
			double spriteY = sprite[sprite_order[i]].y - posY;

			
			double inverse_det = 1.0 / (planeX * dirY - planeY * dirX);
			double transformX = inverse_det * (dirY * spriteX - dirX * spriteY);
			double transformY = inverse_det * (planeX * spriteY - planeY * spriteX);

			int sprite_screenX = int((SCREEN_WIDTH / 2) * (1 + transformX / transformY));
			int sprite_height = abs(int(SCREEN_HEIGHT / transformY));

			int draw_startY = -sprite_height / 2 + SCREEN_HEIGHT / 2;
			draw_startY = (draw_startY < 0) ? 0 : draw_startY;

			int draw_endY = sprite_height / 2 + SCREEN_HEIGHT / 2;
			draw_endY = (draw_endY >= SCREEN_HEIGHT) ? (SCREEN_HEIGHT - 1) : draw_endY;

			int sprite_width = abs(int(SCREEN_HEIGHT / transformY));
			int draw_startX = -sprite_width / 2 + sprite_screenX;
			draw_startX = (draw_startX < 0) ? 0 : draw_startX;

			int draw_endX = sprite_width / 2 + sprite_screenX;
			draw_endX = (draw_endX >= SCREEN_WIDTH) ? (SCREEN_WIDTH - 1) : draw_endX;

			for (int stripe = draw_startX; stripe < draw_endX; stripe++){
				int textureX = int(256 * (stripe - (-sprite_width / 2 + sprite_screenX)) * TEXTURE_WIDTH / sprite_width) / 256;
				if (transformY > 0 && stripe > 0 && stripe < SCREEN_WIDTH && transformY < Zbuffer[stripe]){
					for (int y = draw_startY; y < draw_endY; y++){
						int d = (y) * 256 - SCREEN_HEIGHT * 128 + sprite_height * 128;
						int textureY = ((d * TEXTURE_HEIGHT) / sprite_height) / 256;
						Uint32 color = texture[sprite[sprite_order[i]].texture][TEXTURE_WIDTH * textureY + textureX];
						if ((color & 0x00FFFFFF) != 0) color = apply_fog_depth(color, transformY);
						if ((color & 0x00FFFFFF) != 0) buffer[y][stripe] = color;
					}
				}
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
