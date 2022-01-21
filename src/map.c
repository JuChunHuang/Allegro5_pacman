#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "map.h"
#define QUEUE_SIZE 3000

extern bool mapp;
/*global variables*/
const int block_width = 21, block_height = 21;			// the pixel size of a "block"
const int map_offset_x = 25, map_offset_y = 50;			// pixel offset of where to start draw map
const int four_probe[4][2] = { { 1, 0 }, { 0, 1 }, { -1,0 }, { 0, -1 } };
/* Declare static function prototypes. */
static void draw_block_index(Map* M, int row, int col, int r, int g, int b);
static void draw_bean(Map* M, const int row, const int col);
static void draw_power_bean(Map* M, const int row, const int col);

Map* create_map() {

	Map* M = (Map*)malloc(sizeof(Map));
	FILE* pFile = NULL;
	if (!M) {
		game_abort("Error when creating Map");
		return NULL;
	}
	if (mapp==true) {
		game_log("Assets/map_nthu.txt\n");
		pFile = fopen("Assets/map_nthu.txt", "r");
		if (!pFile) {
			game_abort("error to open map file\n");
			return NULL;
		}
		if (fscanf(pFile, "%d%d", &M->row_num, &M->col_num) != 2) {
			game_abort("Map format unmatched\n");
			return NULL;
		}
		getc(pFile); // get the '\n'
	}
	else if(mapp==false){

		game_log("Assets/map2.txt\n");
		pFile = fopen("Assets/map2.txt", "r");
		if (!pFile) {
			game_abort("error to open map file\n");
			return NULL;
		}
		if(fscanf(pFile, "%d%d", &M->row_num, &M->col_num) != 2) {
			game_abort("Map format unmatched\n");
			return NULL;
		}
		getc(pFile); // get the '\n'
	}

	M->map = (char**)malloc(sizeof(char*) * M->row_num);
	if (!M->map) {
		game_abort(stderr, "map char array malloc error\n");
		return NULL;
	}
	for (int i = 0; i < M->row_num; i++) {
		M->map[i] = (char*)malloc(sizeof(char) * (M->col_num));
		if (!M->map[i]) {
			game_abort(stderr, "map char array malloc error\n");
			return NULL;
		}
	}

	M->wallnum = M->beansCount = 0;
	for (int i = 0; i < M->row_num; i++) {
		for (int j = 0; j < M->col_num; j++) {
			fscanf(pFile, "%c", &M->map[i][j]);
				
			switch (M->map[i][j]) {
			case '#':
				M->wallnum++;
				break;
			case '.':
				M->beansCount++;
				break;
			default:
				break;
			}
		}
		getc(pFile);
	}
	M->beansNum = M->beansCount;
	return M;
}

void delete_map(Map* M) {
	if (!M)
		return;
	if (M->map)
	{
		for (int i = 0; i < M->row_num; i++) free(M->map[i]);
		free(M->map);
	}
	free(M);
}


void draw_map(Map const* M) {
	if (M == NULL) {
		game_abort("error map!\n");
		return;
	}
	if (mapp == true) {
		for (int row = 0; row < M->row_num; row++) {
			for (int col = 0; col < M->col_num; col++) {
				if (row >= 10 && row < 17 && col >= 2 && col < 34) {
					switch (M->map[row][col])
					{
					case '#':
						draw_block_index(M, row, col, 167, 77, 246);
						break;
					case '.':
						draw_bean(M, row, col);
						break;
					default:
						break;
					}
				}
				else {
					switch (M->map[row][col])
					{
					case '#':
						draw_block_index(M, row, col, 25, 154, 25);
						break;
					case '.':
						draw_bean(M, row, col);
						break;
					default:
						break;
					}
				}
			}
		}
	}
	else {
		for (int row = 0; row < M->row_num; row++) {
			for (int col = 0; col < M->col_num; col++) {
				switch (M->map[row][col])
				{
				case '#':
					draw_block_index(M, row, col, 134, 215, 218);
					break;
					/*
					case 'P':
						draw_power_bean(...);
						break;
					*/
				case '.':
					draw_bean(M, row, col);
					break;
				default:
					break;
				}
			}
		}
	}
}

static void draw_block_index(Map* M, const int row, const int col, const int r, const int g, const int b) {
	bool U = is_wall_block(M, col, row - 1);
	bool UR = is_wall_block(M, col + 1, row - 1);
	bool UL = is_wall_block(M, col - 1, row - 1);
	bool B = is_wall_block(M, col, row + 1);
	bool BR = is_wall_block(M, col + 1, row + 1);
	bool BL = is_wall_block(M, col - 1, row + 1);
	bool R = is_wall_block(M, col + 1, row);
	bool L = is_wall_block(M, col - 1, row);
	if (!(U && UR && UL && B && BR && BL && R && L)) {
		int s_x, s_y, e_x, e_y, dw;
		int block_x = map_offset_x + block_width * col;
		int block_y = map_offset_y + block_height * row;
		dw = block_width / 3;
		s_x = block_x + dw;
		s_y = block_y + dw;
		e_x = s_x + dw;
		e_y = s_y + dw;

		al_draw_filled_rectangle(s_x, s_y,
			e_x, e_y, al_map_rgb(r, g, b));
		if (row < M->row_num - 1 && B && !(BL && BR && R && L)) {
			s_x = block_x + dw;
			s_y = block_y + dw;
			e_x = s_x + dw;
			e_y = block_y + block_height;
			al_draw_filled_rectangle(s_x, s_y,
				e_x, e_y, al_map_rgb(r, g, b));
		}
		if (row > 0 && U && !(UL && UR && R && L)) {
			s_x = block_x + dw;
			s_y = block_y + (dw << 1);
			e_x = s_x + dw;
			e_y = block_y;
			al_draw_filled_rectangle(s_x, s_y,
				e_x, e_y, al_map_rgb(r, g, b));
		}
		if (col < M->col_num - 1 && R && !(UR && BR && U && B)) {
			s_x = block_x + dw;
			s_y = block_y + dw;
			e_x = block_x + block_width;
			e_y = s_y + dw;
			al_draw_filled_rectangle(s_x, s_y,
				e_x, e_y, al_map_rgb(r, g, b));

		}
		if (col > 0 && L && !(UL && BL && U && B)) {
			s_x = block_x;
			s_y = block_y + dw;
			e_x = s_x + (dw << 1);
			e_y = s_y + dw;
			al_draw_filled_rectangle(s_x, s_y,
				e_x, e_y, al_map_rgb(r, g, b));
		}
	}
}

static void draw_bean(Map* M, const int row, const int col) {
	al_draw_filled_circle(map_offset_x + col * block_width + block_width / 2.0, map_offset_y + row * block_height + block_height / 2.0, block_width / 6.0, al_map_rgb(234, 38, 38));
}

static void draw_power_bean(Map* M, const int row, const int col) {
	al_draw_filled_circle(map_offset_x + col * block_width + block_width / 2.0, map_offset_y + row * block_height + block_height / 2.0, block_width / 3.0, al_map_rgb(234, 178, 38));
}

bool is_wall_block(Map* M, int index_x, int index_y) {
	if (index_x < 0 || index_x >= M->col_num || index_y < 0 || index_y >= M->row_num)
		return true;
	return M->map[index_y][index_x] == '#';
}

bool is_room_block(Map* M, int index_x, int index_y) {
	if (index_x < 0 || index_x >= M->col_num || index_y < 0 || index_y >= M->row_num)
		return true;
	return M->map[index_y][index_x] == 'B';
}

Directions shortest_path_direc(Map* M, int startGridx, int startGridy, int endGridx, int endGridy) {

	static int8_t queue_x[QUEUE_SIZE];
	static int8_t queue_y[QUEUE_SIZE];
	static	uint16_t front;
	static	uint16_t end;

	static Directions steped[MAX_WALL_NUM_H][MAX_WALL_NUM_W];
	memset(steped, 0, sizeof(steped)); // set as NONE;

	front = end = 0;
	queue_x[end] = startGridx;
	queue_y[end] = startGridy;
	steped[startGridy][startGridx] = 1; /*	for dummy just means that startGrid have been visited.	*/

	end++;

	for (size_t i = 0; i < 4; i++) {
		int8_t x = queue_x[front] + four_probe[i][0];
		int8_t y = queue_y[front] + four_probe[i][1];
		if (is_wall_block(M, x, y) || steped[y][x])
			continue;
		queue_x[end] = x;
		queue_y[end] = y;
		switch (i) {
		case 0:
			steped[y][x] = RIGHT;
			break;
		case 1:
			steped[y][x] = DOWN;
			break;
		case 2:
			steped[y][x] = LEFT;
			break;
		case 3:
			steped[y][x] = UP;
			break;
		default:
			break;
		}
		end++;
	}
	front++;

	while (front != end && steped[endGridy][endGridx] == NONE) {

		for (size_t i = 0; i < 4; i++) {
			int8_t x = queue_x[front] + four_probe[i][0];
			int8_t y = queue_y[front] + four_probe[i][1];
			if (is_wall_block(M, x, y) || steped[y][x])
				continue;
			queue_x[end] = x;
			queue_y[end] = y;
			steped[y][x] = steped[queue_y[front]][queue_x[front]];
			end++;
		}
		front++;
	}
	return steped[endGridy][endGridx];
}