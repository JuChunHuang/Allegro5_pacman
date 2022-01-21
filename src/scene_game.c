#define _CRT_SECURE_NO_WARNINGS

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include <stdio.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"

#define GHOST_NUM 4
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
int eat_beans_num = 0;
bool game_over = false;
bool game_victory = false;
/* Internal variables*/
static ALLEGRO_TIMER* power_up_timer;
static ALLEGRO_BITMAP* exit_img = NULL;
static const int power_up_duration = 10;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
static Button quit;
bool debug_mode = false;
bool cheat_mode = false;

static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(void);
static void render_init_screen(void);
static void draw_hitboxes(void);

static void init(void) {
	game_over = false;
	game_main_Score = -10;
	basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}	
	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacman1\n");
	}
	
	ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
	if (!ghosts) {
		game_log("We haven't create any ghosts!\n");
	}
	else {
		for (int i = 0; i < GHOST_NUM; i++) {
			
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	GAME_TICK = 0;
	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick / sec
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	quit = button_create(SCREEN_W / 2 - 50, SCREEN_H - 100, 100, 100, "Assets/exit1.png", "Assets/exit2.png");
	return ;
}

static void drawMainButton(Button button) {
	ALLEGRO_BITMAP* _img = button.hovered_img ?
		button.hovered ?
		button.hovered_img :
		button.default_img :
		button.default_img;
	al_draw_scaled_bitmap(
		_img,
		0, 0,
		al_get_bitmap_width(_img), al_get_bitmap_height(_img),
		button.body.x, button.body.y,
		button.body.w, button.body.h, 0
	);
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}

static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;

	switch (basic_map->map[Grid_y][Grid_x])
	{
	case '.':
		pacman_eatItem(pman, '.');
		game_main_Score += 10;
		eat_beans_num++;
		basic_map->map[Grid_y][Grid_x] = '\0';
		break;
	case 'P':
		pacman_eatItem(pman, 'P');
		break;
	default:
		break;
	}	
}

static void status_update(void) {
	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN)
			continue;

		RecArea pman_area = getDrawArea(pman->objData, GAME_TICK_CD);
		RecArea ghost_area = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		if((!cheat_mode && RecAreaOverlap(pman_area, ghost_area))||(!cheat_mode && RecAreaOverlap(pman_area, ghost_area))){
			game_log("collide with ghost\n");
			game_over = true;
			al_rest(1.0);
			pacman_die();
			break;
		}
		if (eat_beans_num == basic_map->beansNum) {
			game_victory = true;
			break;
		}
	}
}

static void update(void) {

	if (game_over) {	
		al_start_timer(pman->death_anim_counter);
		al_rest(3);
		game_change_scene(scene_menu_create());
		return;
	}
	if (game_victory) {
		al_rest(1.0);
		game_change_scene(scene_victory_create());
		return;
	}
	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++)
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	char score[15];
	sprintf(score, "SCORE = %d", game_main_Score);
	al_draw_text(menuFont,
		al_map_rgb(255, 255, 255),
		0, 10,
		ALLEGRO_ALIGN_LEFT,
		score
	);
	if (GAME_TICK < 32) {
		al_draw_text(menuFont,
			al_map_rgb(255, 255, 255),
			600, 10,
			ALLEGRO_ALIGN_LEFT,
			"LET'S GO!"
		);
	}
	draw_map(basic_map);

	pacman_draw(pman);
	if (game_over)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);

	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}
	drawMainButton(quit);
}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	
	for (int i = 0; i < GHOST_NUM; i++)	ghost_destory(ghosts[i]);
	pacman_destroy(pman);
	delete_map(basic_map);
	al_destroy_bitmap(quit.default_img);
	al_destroy_bitmap(quit.hovered_img);
	al_destroy_bitmap(exit_img);
}

static void on_key_down(int key_code) {
	switch (key_code)
	{		
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman, UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman, LEFT);
			break;
		case ALLEGRO_KEY_S:
			pacman_NextMove(pman, DOWN);
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman, RIGHT);
			break;
		case ALLEGRO_KEY_C:
			cheat_mode = !cheat_mode;
			if (cheat_mode)
				printf("cheat mode on\n");
			else 
				printf("cheat mode off\n");
			break;
		case ALLEGRO_KEY_G:
			debug_mode = !debug_mode;
			break;
		
	default:
		break;
	}
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	quit.hovered = buttonHover(quit, mouse_x, mouse_y);
}

static void on_mouse_down(void) {
	if (quit.hovered) {
		game_change_scene(scene_exit_create());
	}
}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	exit_img = load_bitmap("Assets/exit1.png");
	al_draw_scaled_bitmap(
		exit_img,
		0, 0,
		al_get_bitmap_width(exit_img), al_get_bitmap_height(exit_img),
		SCREEN_W / 2 - 50, SCREEN_H - 100, 100, 100, 0
	);
	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);
	al_draw_text(menuFont,
		al_map_rgb(255, 255, 255),
		0, 0,
		ALLEGRO_ALIGN_LEFT,
		"SCORE = 0"
	);
	al_draw_text(menuFont,
		al_map_rgb(255, 255, 255),
		600, 10,
		ALLEGRO_ALIGN_LEFT,
		"LET'S GO!"
	);
	al_flip_display();
	al_rest(2.0);
}

Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	scene.on_mouse_move = &on_mouse_move;
	game_log("Start scene created");
	return scene;
}