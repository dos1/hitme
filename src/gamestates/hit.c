/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../common.h"
#include <allegro5/allegro_color.h>
#include <libsuperderpy.h>
#include <math.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	ALLEGRO_FONT *font, *comicsans;
	ALLEGRO_BITMAP* checkerboard;
	ALLEGRO_SAMPLE* key_sample;
	ALLEGRO_SAMPLE_INSTANCE* key;
	int blink_counter;
	int color;
	int randx, randy;
	int score;
	int counter;
	int sps;
	int max;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {}

void Gamestate_Tick(struct Game* game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;
	if (data->blink_counter == 60) {
		data->blink_counter = 0;
		data->sps = data->score / 60;
		data->score = 0;
	}
	if (data->sps > data->max) {
		data->max = data->sps;
	}
	data->counter++;
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_clear_to_color(al_color_hsv(fabs(sin(data->color / 360.0)) * 360, 0.75, 0.5 + sin(data->color / 20.0) / 20.0));

	if (data->blink_counter < 50) {
		al_draw_text(data->comicsans, al_map_rgb(255, 255, 255), data->randx + game->viewport.width / 2, data->randy + game->viewport.height / 2 - 72,
			ALLEGRO_ALIGN_CENTRE, "HIT ME");
	}

	al_draw_textf(data->font, al_map_rgb(255, 255, 255), -data->randx * 0.8 + (game->viewport.width / 2) - 20, -data->randy / 2 + game->viewport.height - 38,
		ALLEGRO_ALIGN_CENTRE, "%d POINTS/SEC", data->sps);
	al_draw_textf(data->font, al_map_rgb(255, 255, 255), -data->randx / 3 + (game->viewport.width / 2) + 35, data->randy / 3 + game->viewport.height - 24,
		ALLEGRO_ALIGN_CENTRE, "%d BEST", data->max);

	al_draw_bitmap(data->checkerboard, 0, 0, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) || (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
		data->color += 50;
		data->randx = rand() % 9 - 4;
		data->randy = rand() % 19 - 8;

		if (data->counter < 10) {
			data->score += (10 - data->counter) * 42 * 69;
		}
		data->counter = 0;

		al_stop_sample_instance(data->key);
		al_play_sample_instance(data->key);
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_CHAR) && (ev->keyboard.keycode == ALLEGRO_KEY_R)) {
		if (ev->keyboard.modifiers & ALLEGRO_KEYMOD_CTRL) {
			data->score = 0;
			data->sps = 0;
			data->max = 0;
		}
	}
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_MAG_LINEAR);

	data->font = al_create_builtin_font();
	data->comicsans = al_load_font(GetDataFilePath(game, "fonts/comicsans.ttf"), 80, ALLEGRO_TTF_MONOCHROME);

	al_set_new_bitmap_flags(flags);

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->key_sample = al_load_sample(GetDataFilePath(game, "click.flac"));
	data->key = al_create_sample_instance(data->key_sample);
	al_attach_sample_instance_to_mixer(data->key, game->audio.fx);
	al_set_sample_instance_playmode(data->key, ALLEGRO_PLAYMODE_ONCE);

	return data;
}

void Gamestate_PostLoad(struct Game* game, struct GamestateResources* data) {
	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(flags & ~ALLEGRO_MAG_LINEAR);

	data->checkerboard = al_create_bitmap(320, 180);

	al_set_target_bitmap(data->checkerboard);
	al_lock_bitmap(data->checkerboard, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	int x, y;
	for (x = 0; x < al_get_bitmap_width(data->checkerboard); x = x + 2) {
		for (y = 0; y < al_get_bitmap_height(data->checkerboard); y = y + 2) {
			al_put_pixel(x, y, al_map_rgba(0, 0, 0, 32));
			al_put_pixel(x + 1, y, al_map_rgba(0, 0, 0, 0));
			al_put_pixel(x, y + 1, al_map_rgba(0, 0, 0, 0));
			al_put_pixel(x + 1, y + 1, al_map_rgba(0, 0, 0, 0));
		}
	}
	al_unlock_bitmap(data->checkerboard);
	al_set_new_bitmap_flags(flags);
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_destroy_font(data->comicsans);
	al_destroy_bitmap(data->checkerboard);
	al_destroy_sample_instance(data->key);
	al_destroy_sample(data->key_sample);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	data->color = 0;
	data->randx = 0;
	data->randy = 0;
	data->counter = 0;
	data->score = 0;
	data->sps = 0;
	data->max = 0;
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {}
