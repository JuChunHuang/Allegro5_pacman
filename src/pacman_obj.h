#ifndef PACMAN_OBJ_H
#define PACMAN_OBJ_H

#include "utility.h"
#include "game.h"

typedef struct Ghost Ghost;
typedef struct Map Map;

typedef struct Pacman {

	bitmapdata imgdata;
	object objData;
	func_ptr move;
	int speed;
	bool powerUp;
	ALLEGRO_TIMER* death_anim_counter;
	ALLEGRO_BITMAP* move_sprite;
	ALLEGRO_BITMAP* die_sprite;
} Pacman;

Pacman* pacman_create();
void pacman_destroy(Pacman* pman);

void pacman_draw(Pacman* pacman);
void pacman_move(Pacman* pacman, Map* M);
void pacman_eatItem(Pacman* pacman, const char Item);

void pacman_NextMove(Pacman* pacman, Directions next);
void pacman_die();

#endif