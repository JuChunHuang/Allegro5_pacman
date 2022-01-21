#ifndef GAME_H
#define GAME_H
#define _CRT_SECURE_NO_DEPRECATE
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>	
typedef struct Ghost Ghost;
typedef struct Map Map;
typedef struct Pacman Pacman;
// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED // Question
// Types of function pointers.
static ALLEGRO_EVENT_QUEUE* game_event_queue;
typedef void(*func_ptr)(void);
typedef void(*script_func_ptr)(Ghost* ghost, Map* M, Pacman* pacman);
typedef void(*func_ptr_keyboard)(int keycode);
typedef void(*func_ptr_mouse)(int btn, int x, int y, int dz);

// Structure containing all scene functions / event callbacks.
typedef struct {
	char* name;
	func_ptr initialize;
	func_ptr update;
	func_ptr draw;
	func_ptr destroy;
	func_ptr_keyboard on_key_down;
	func_ptr_keyboard on_key_up;
	func_ptr_mouse on_mouse_down;
	func_ptr_mouse on_mouse_move;
	func_ptr_mouse on_mouse_up;
	func_ptr_mouse on_mouse_scroll;
} Scene;

// Frame rate (frame per second)
extern const int FPS;
// Display (screen) width.
extern const int SCREEN_W;
// Display (screen) height.
extern const int SCREEN_H;
// At most 4 audios can be played at a time.
extern const int RESERVE_SAMPLES;


/* Input states */
// The active scene. Events will be triggered through function pointers.
extern Scene active_scene;
// Keyboard state, whether the key is down or not.
extern bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
extern bool* mouse_state;
extern int mouse_x, mouse_y;

/* Function prototypes */
void game_create(void);
void game_change_scene(Scene next_scene);
void game_abort(const char* format, ...);
void game_log(const char* format, ...);

/* Functions and variables that should be implemented in other files. */
// Initialize shared variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
extern void shared_init(void);
// Free shared variables and resources.
extern void shared_destroy(void);
#endif
