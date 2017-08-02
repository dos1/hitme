/*! \file loading.c
 *  \brief Loading screen.
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
#include <libsuperderpy.h>

/*! \brief Resources used by Loading state. */
struct LoadingResources {
		ALLEGRO_BITMAP *loading_bitmap; /*!< Rendered loading bitmap. */
};

void Progress(struct Game *game, struct LoadingResources *data, float p) {
	al_set_target_bitmap(al_get_backbuffer(game->display));
	al_draw_bitmap(data->loading_bitmap,0,0,0);
	al_draw_filled_rectangle(0, game->viewport.height * 0.98, p*game->viewport.width,
	                         game->viewport.height, al_map_rgba(128,128,128,128));
}

void Draw(struct Game *game, struct LoadingResources *data, float p) {
	al_draw_bitmap(data->loading_bitmap,0,0,0);
	Progress(game, data, p);
}

void* Load(struct Game *game) {
	struct LoadingResources *data = malloc(sizeof(struct LoadingResources));
	al_clear_to_color(al_map_rgb(0,0,0));

	data->loading_bitmap = al_create_bitmap(game->viewport.width, game->viewport.height);

	al_set_target_bitmap(data->loading_bitmap);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_filled_rectangle(0, game->viewport.height * 0.98, game->viewport.width,
	                         game->viewport.height, al_map_rgba(32,32,32,32));
	al_set_target_bitmap(al_get_backbuffer(game->display));
	return data;
}
void Unload(struct Game *game, struct LoadingResources *data) {
	al_destroy_bitmap(data->loading_bitmap);
	free(data);
}

void Start(struct Game *game, struct LoadingResources *data) {}
void Stop(struct Game *game, struct LoadingResources *data) {}
