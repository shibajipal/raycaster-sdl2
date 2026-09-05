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

#define LIGHT_COUNT 12
Sprite light_sprites[LIGHT_COUNT] = {
	{2.5, 2.5, 10}, {2.5, 21.5, 10}, {21.5, 2.5, 10}, {21.5, 21.5, 10},
	{11.5, 2.5, 10}, {11.5, 21.5, 10}, {2.5, 11.5, 10}, {21.5, 11.5, 10},
	{7.5, 7.5, 10}, {16.5, 16.5, 10}, {7.5, 16.5, 10}, {16.5, 7.5, 10}
};

Uint32 apply_point_lights(Uint32 color, double worldX, double worldY, Sprite lights[], int light_count, double ambient = 0.38){
	double total_r = ambient, total_g = ambient, total_b = ambient;

	for (int i = 0; i < light_count; i++){
		double dx = worldX - lights[i].x;
		double dy = worldY - lights[i].y;
		double dist_sq = dx * dx + dy * dy;

		// skip lights further than ~8 units away
		if (dist_sq > 64.0) continue;
		if (dist_sq < 0.001) dist_sq = 0.001;

		double dist = sqrt(dist_sq);
		double attenuation = 1.0 / (1.0 + 0.2 * dist + 0.06 * dist_sq);

		// warm golden/amber light
		total_r += 1.6 * attenuation;
		total_g += 1.1 * attenuation;
		total_b += 0.35 * attenuation;
	}

	// allow overexposure near light sources for brilliant hotspots
	total_r = min(1.8, total_r);
	total_g = min(1.8, total_g);
	total_b = min(1.8, total_b);

	Uint8 r = (Uint8)min(255.0, ((color >> 16) & 0xFF) * total_r);
	Uint8 g = (Uint8)min(255.0, ((color >> 8) & 0xFF) * total_g);
	Uint8 b = (Uint8)min(255.0, (color & 0xFF) * total_b);

	return (0xFF << 24) | (r << 16) | (g << 8) | b;
}
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
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	bool is_running = true;
	bool is_fullscreen = false;
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
			if (event.key.keysym.scancode == SDL_SCANCODE_F11){
				is_fullscreen = !is_fullscreen;
				SDL_SetWindowFullscreen(window, is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_C){
				if (event.key.keysym.mod & KMOD_ALT){
					is_floorcasting = !is_floorcasting;
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
			if (world_map[int(posX)][int(posY + dirX * move_speed)] == 0) posY += dirX * move_speed;
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
				if (world_map[int(posX)][int(posY - dirX * move_speed)] == 0) posY -= dirX * move_speed;
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
			
			double hit_worldX = posX + perpWallDist * raydirX;
			double hit_worldY = posY + perpWallDist * raydirY;
			
			for (int y = draw_start; y < draw_end; y++){
				int textureY = (int)texture_pos & (TEXTURE_HEIGHT - 1);
				texture_pos += step;
				Uint32 color = texture[texture_num][TEXTURE_HEIGHT * textureY + textureX];

				// if (side == 1) color = (color >> 1) & 8355711;
				
				color = apply_point_lights(color, hit_worldX, hit_worldY, light_sprites, LIGHT_COUNT);
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
		

		// volumetric light shafts, kinda weird rn will make better later
		for (int li = 0; li < LIGHT_COUNT; li++){
			double lx = light_sprites[li].x - posX;
			double ly = light_sprites[li].y - posY;

			double inv_det = 1.0 / (planeX * dirY - planeY * dirX);
			double tX = inv_det * (dirY * lx - dirX * ly);
			double tY = inv_det * (planeX * ly - planeY * lx);

			if (tY < 0.5 || tY > 10.0) continue; // behind camera or too far

			int scrX = (int)((SCREEN_WIDTH / 2) * (1 + tX / tY));
			if (scrX < -80 || scrX >= SCREEN_WIDTH + 80) continue;

			int sprite_h = abs((int)(SCREEN_HEIGHT / tY));
			// shaft starts near the top of the light sprite, extends to bottom of screen
			int shaft_top = SCREEN_HEIGHT / 2 - sprite_h / 2;
			int shaft_bottom = SCREEN_HEIGHT;

			// narrow point at the light, widens toward the floor
			int max_half_w = sprite_h / 3;
			double dist_fade = 1.0 / (1.0 + tY * 0.25);

			for (int y = max(0, shaft_top); y < shaft_bottom; y++){
				double t = (double)(y - shaft_top) / max(1, shaft_bottom - shaft_top);
				int half_w = max(1, (int)(max_half_w * t));
				double vert_intensity = (1.0 - t * 0.5) * dist_fade * 0.12;

				int x_start = max(0, scrX - half_w);
				int x_end = min(SCREEN_WIDTH - 1, scrX + half_w);
				for (int x = x_start; x <= x_end; x++){
					if (Zbuffer[x] < tY) continue; // wall in front occludes shaft

					double edge = (double)abs(x - scrX) / (half_w + 1);
					double px_i = vert_intensity * (1.0 - edge * edge);

					// bright edge lines along the cone boundary for definition
					int dist_from_edge = abs(abs(x - scrX) - half_w);
					if (dist_from_edge <= 1)
						px_i = max(px_i, vert_intensity * 1.3);

					Uint32 c = buffer[y][x];
					int r = min(255, (int)(((c >> 16) & 0xFF) + 255 * px_i));
					int g = min(255, (int)(((c >> 8) & 0xFF) + 210 * px_i));
					int b = min(255, (int)((c & 0xFF) + 65 * px_i));
					buffer[y][x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
				}
			}
		int map_scale = 4;
		int map_pad = 10;  // padding from screen edge
		// semi-transparent background
		for (int my = 0; my < MAP_HEIGHT; my++){
			for (int mx = 0; mx < MAP_WIDTH; mx++){
				Uint32 map_col = (world_map[mx][my] > 0) ? 0xFFAAAAAA : 0xFF1A1A2E;
				for (int py = 0; py < map_scale; py++){
					for (int px = 0; px < map_scale; px++){
						int sx = map_pad + mx * map_scale + px;
						int sy = map_pad + my * map_scale + py;
						if (sx < SCREEN_WIDTH && sy < SCREEN_HEIGHT){
							// 70% map, 30% existing scene for transparency feel
							Uint32 bg = buffer[sy][sx];
							Uint8 mr = (Uint8)(((map_col >> 16) & 0xFF) * 0.7 + ((bg >> 16) & 0xFF) * 0.3);
							Uint8 mg = (Uint8)(((map_col >> 8) & 0xFF) * 0.7 + ((bg >> 8) & 0xFF) * 0.3);
							Uint8 mb = (Uint8)((map_col & 0xFF) * 0.7 + (bg & 0xFF) * 0.3);
							buffer[sy][sx] = (0xFF << 24) | (mr << 16) | (mg << 8) | mb;
						}
					}
				}
			}
		}
		// player dot (red, 3x3)
		int pmx = map_pad + (int)(posX * map_scale);
		int pmy = map_pad + (int)(posY * map_scale);
		for (int py = -1; py <= 1; py++){
			for (int px = -1; px <= 1; px++){
				int sx = pmx + px, sy = pmy + py;
				if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT)
					buffer[sy][sx] = 0xFFFF3333;
			}
		}
		// direction line (white, 6 pixels long)
		for (int i = 0; i < 6; i++){
			int sx = pmx + (int)(dirX * i);
			int sy = pmy + (int)(dirY * i);
			if (sx >= 0 && sx < SCREEN_WIDTH && sy >= 0 && sy < SCREEN_HEIGHT)
				buffer[sy][sx] = 0xFFFFFFFF;
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
