#include "scene_settings.h"
#include "shared.h"
#include "utility.h"
#include "scene_menu.h"
#include "scene_menu_object.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>

extern ALLEGRO_TIMER* game_tick_timer;
extern bool mapp = true;
static ALLEGRO_BITMAP* gameVictory = NULL;
static ALLEGRO_SAMPLE_ID victoryBGM;
static int gameVictoryW;
static int gameVictoryH;
static Button music_down;
static Button music_up;
static Button effect_down;
static Button effect_up;
static Button map_1;
static Button map_2;
static Button yes;
static Button no;

static void victory_init() {

	gameVictory = load_bitmap("Assets/victory.png");
	gameVictoryW = al_get_bitmap_width(gameVictory);
	gameVictoryH = al_get_bitmap_height(gameVictory);
	stop_bgm(victoryBGM);
	victoryBGM = play_bgm(PACMAN_VICTORY_SOUND, music_volume);

}

static void draw_victory(void) {
	game_log("victory in");
	al_clear_to_color(al_map_rgb(0, 0, 0));
	const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameVictoryW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameVictoryH;
	al_draw_scaled_bitmap(
		gameVictory,
		0, 0,
		gameVictoryW, gameVictoryH,
		offset_w, offset_h,
		gameVictoryW * scale, gameVictoryH * scale,
		0
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 100,
		ALLEGRO_ALIGN_CENTER,
		"PRESS \"ENTER\" TO CONTINUE"
	);

}

static void on_key_down(int keycode) {

	switch (keycode) {
	case ALLEGRO_KEY_ENTER:
		game_change_scene(scene_menu_create());
		break;
	default:
		break;
	}
}

static void destroy_victory() {
	stop_bgm(victoryBGM);
	al_destroy_bitmap(gameVictory);
}

Scene scene_victory_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Victory";
	scene.initialize = &victory_init;
	scene.draw = &draw_victory;
	scene.on_key_down = &on_key_down;
	scene.destroy = &destroy_victory;
	game_log("Victory scene created");
	return scene;
}

static void settings_init() {

	music_down = button_create((SCREEN_W / 5) * 3, SCREEN_H / 5 - 10, 50, 50, "Assets/down_white.png", "Assets/down_red.png");
	music_up = button_create((SCREEN_W / 5) * 4 - 20, SCREEN_H / 5 - 10, 50, 50, "Assets/up_white.png", "Assets/up_red.png");
	effect_down = button_create((SCREEN_W / 5) * 3, (SCREEN_H / 5) * 2 - 10, 50, 50, "Assets/down_white.png", "Assets/down_red.png");
	effect_up = button_create((SCREEN_W / 5) * 4 - 20, (SCREEN_H / 5) * 2 - 10, 50, 50, "Assets/up_white.png", "Assets/up_red.png");
	map_1 = button_create((SCREEN_W / 5) * 2-50, (SCREEN_H / 5) * 3 + 100, 50, 50, "Assets/map11.png", "Assets/map12.png");
	map_2 = button_create((SCREEN_W / 5) * 3-50, (SCREEN_H / 5) * 3 + 100, 50, 50, "Assets/map21.png", "Assets/map22.png");

}

static void drawvolumnButton(Button button) {
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

static void draw(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	char music[50];
	char effect[50];
	sprintf(music, "MUSIC VOLUME                   %.1f", music_volume);
	sprintf(effect, "EFFECT VOLUME                %.1f", effect_volume);
	al_draw_text(menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/5-10, SCREEN_H/5,
		ALLEGRO_ALIGN_LEFT,
		music
	);
	al_draw_text(menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 5-10, (SCREEN_H / 5)*2,
		ALLEGRO_ALIGN_LEFT,
		effect
	);
	al_draw_text(menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 5 - 10, (SCREEN_H / 5) * 3,
		ALLEGRO_ALIGN_LEFT,
		"PLEASE CHOOSE A MAP:"
	);
	drawvolumnButton(music_down);
	drawvolumnButton(music_up);
	drawvolumnButton(effect_down);
	drawvolumnButton(effect_up);
	drawvolumnButton(map_1);
	drawvolumnButton(map_2);
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	music_down.hovered = buttonHover(music_down, mouse_x, mouse_y);
	music_up.hovered = buttonHover(music_up, mouse_x, mouse_y);
	effect_down.hovered = buttonHover(effect_down, mouse_x, mouse_y);
	effect_up.hovered = buttonHover(effect_up, mouse_x, mouse_y);
	map_1.hovered = buttonHover(map_1, mouse_x, mouse_y);
	map_2.hovered = buttonHover(map_2, mouse_x, mouse_y);
}

static void on_mouse_down() {
	if (music_volume >= 0.1 && effect_volume >= 0.1) {
		if (music_down.hovered)	music_volume -= 0.1;
		if (effect_down.hovered)	effect_volume -= 0.1;
	}
	if (music_up.hovered)	music_volume += 0.1;
	if (effect_up.hovered)	effect_volume += 0.1;
	if (map_1.hovered) {
		mapp = true;
		game_change_scene(scene_menu_create());
	}
	if (map_2.hovered) {
		mapp = false;
		game_change_scene(scene_menu_create());
	}
}

static void destroy_settings() {
	al_destroy_bitmap(music_down.default_img);
	al_destroy_bitmap(music_down.hovered_img);
	al_destroy_bitmap(music_up.default_img);
	al_destroy_bitmap(music_up.hovered_img);
	al_destroy_bitmap(effect_down.default_img);
	al_destroy_bitmap(effect_down.hovered_img);
	al_destroy_bitmap(effect_up.default_img);
	al_destroy_bitmap(effect_up.hovered_img);

}

Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &settings_init;
	scene.draw = &draw;
	scene.destroy = &destroy_settings;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	scene.on_key_down = &on_key_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}

static void exit_init() {

	yes = button_create(SCREEN_W / 2 - 50, SCREEN_H - 200 , 100, 50, "Assets/yes1.png", "Assets/yes2.png");
	no = button_create(SCREEN_W / 2 - 50, SCREEN_H - 100, 100, 50, "Assets/no1.png", "Assets/no2.png");
	
}

static void exit_draw(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));
	drawvolumnButton(yes);
	drawvolumnButton(no);
}

static void exit_on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	yes.hovered = buttonHover(yes, mouse_x, mouse_y);
	no.hovered = buttonHover(no, mouse_x, mouse_y);
}

static void exit_on_mouse_down() {
	if (yes.hovered) {
		game_change_scene(scene_menu_create());
	}
}

static void exit_destroy() {
	al_destroy_bitmap(yes.default_img);
	al_destroy_bitmap(yes.hovered_img);
	al_destroy_bitmap(no.default_img);
	al_destroy_bitmap(no.hovered_img);

}

Scene scene_exit_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &exit_init;
	scene.draw = &exit_draw;
	scene.destroy = &exit_destroy;
	scene.on_mouse_move = &exit_on_mouse_move;
	scene.on_mouse_down = &exit_on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Exit scene created");
	return scene;
}

