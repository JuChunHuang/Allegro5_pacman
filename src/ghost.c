#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include "ghost.h"
#include "map.h"
#include "pacman_obj.h"

/* global variables*/
const int cage_grid_x = 22, cage_grid_y = 11;
/* shared variables. */
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern const int block_width, block_height;
/* Internal variables */
static const int fix_draw_pixel_offset_x = -3;
static const int fix_draw_pixel_offset_y = -3;
static const int draw_region = 30;
static const int basic_speed = 2;

Ghost* ghost_create(int flag) {

	Ghost* ghost = (Ghost*)malloc(sizeof(Ghost));
	if (!ghost)
		return NULL;

	ghost->typeFlag = flag;
	ghost->objData.Size.x = block_width;
	ghost->objData.Size.y = block_height;

	ghost->objData.nextTryMove = NONE;
	ghost->speed = basic_speed;
	ghost->status = BLOCKED;

	ghost->flee_sprite = load_bitmap("Assets/ghost_flee.png");
	ghost->dead_sprite = load_bitmap("Assets/ghost_dead.png");

	switch (ghost->typeFlag) {
	case Blinky:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	case Pinky:
		ghost->objData.Coord.x = cage_grid_x + 1;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_pink.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	case Inky:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y + 1;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_blue.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	default:
		ghost->objData.Coord.x = cage_grid_x + 1;
		ghost->objData.Coord.y = cage_grid_y + 1;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_orange.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	}
	return ghost;
}

void ghost_destory(Ghost* ghost) {
	
	al_destroy_bitmap(ghost->flee_sprite);
	al_destroy_bitmap(ghost->dead_sprite);
	al_destroy_bitmap(ghost->move_sprite);
	free(ghost);

}

void ghost_draw(Ghost* ghost) {
	// getDrawArea return the drawing RecArea defined by objData and GAME_TICK_CD
	RecArea drawArea = getDrawArea(ghost->objData, GAME_TICK_CD);

	//Draw default image
	al_draw_scaled_bitmap(ghost->move_sprite, 0, 0,
		16, 16,
		drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
		draw_region, draw_region, 0
	);

	int bitmap_x_offset = 0;
	if (ghost->status == FLEE) {
		/*
			al_draw_scaled_bitmap(...)
		*/
	}
	else if (ghost->status == GO_IN) {
		/*
		switch (ghost->objData.facing)
		{
		case LEFT:
			...
		*/
	}
	else{
		
		switch (ghost->objData.facing)
		{
		case RIGHT:
			al_draw_scaled_bitmap(ghost->move_sprite, 0, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case LEFT:
			al_draw_scaled_bitmap(ghost->move_sprite, 32, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case UP:
			al_draw_scaled_bitmap(ghost->move_sprite, 64, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		case DOWN:
			al_draw_scaled_bitmap(ghost->move_sprite, 96, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
			break;
		}
	}

}

void ghost_NextMove(Ghost* ghost, Directions next) {
	ghost->objData.nextTryMove = next;
}

void printGhostStatus(GhostStatus S) {

	switch (S) {

	case BLOCKED: // stay inside the ghost room
		game_log("BLOCKED");
		break;
	case GO_OUT: // going out the ghost room
		game_log("GO_OUT");
		break;
	case FREEDOM: // free at the map
		game_log("FREEDOM");
		break;
	case GO_IN:
		game_log("GO_IN");
		break;
	case FLEE:
		game_log("FLEE");
		break;
	default:
		game_log("status error");
		break;
	}
}

bool ghost_movable(Ghost* ghost, Map* M, Directions targetDirec, bool room) {

	int x = ghost->objData.Coord.x;
	int y = ghost->objData.Coord.y;
	switch (targetDirec)
	{
	case UP:
		y += -1;
		break;
	case DOWN:
		y += 1;
		break;
	case LEFT:
		x += -1;
		break;
	case RIGHT:
		x += 1;
		break;
	default:
		return false;
		break;
	}
	if (is_wall_block(M, x, y))
		return false;
	else if(room && is_room_block(M, x, y))
		return false;
	
	return true;

}

void ghost_toggle_FLEE(Ghost* ghost, bool setFLEE) {
	// TODO: Here is reserved for power bean implementation.
	// The concept is "When pacman eats the power bean, only
	// ghosts who are in state FREEDOM will change to state FLEE.
	// For those who are not (BLOCK, GO_IN, etc.), they won't change state."
	// This implementation is based on the classic PACMAN game.
	// You are allowed to do your own implementation of power bean system.
	/*
		if(setFLEE){
			if(... == FREEDOM){
				... = FLEE;
				... speed = ...
			}
		}else{
			if(... == FLEE)
				..
		}
	*/
}

void ghost_collided(Ghost* ghost) {

	if (ghost->status == FLEE) {
		ghost->status = GO_IN;
		ghost->speed = 4;
	}
}

void ghost_move_script_GO_IN(Ghost* ghost, Map* M) {

	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, cage_grid_x, cage_grid_y);

}

void ghost_move_script_GO_OUT(Ghost* ghost, Map* M) {
	// Here we always assume the room of ghosts opens upward.
	// And used a greedy method to drag ghosts out of room.
	if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B')
		ghost_NextMove(ghost, UP);
	else
		ghost->status = FREEDOM;

}

void ghost_move_script_FLEE(Ghost* ghost, Map* M, const Pacman* const pacman) {

	Directions shortestDirection = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pacman->objData.Coord.x, pacman->objData.Coord.y);
	// Description:
	// The concept here is to simulate ghosts running away from pacman while pacman is having power bean ability.
	// To achieve this, think in this way. We first get the direction to shortest path to pacman, call it K (K is either UP, DOWN, RIGHT or LEFT).
	// Then we choose other available direction rather than direction K.
	// In this way, ghost will escape from pacman.

}
