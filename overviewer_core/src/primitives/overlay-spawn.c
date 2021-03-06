/*
 * This file is part of the Minecraft Overviewer.
 *
 * Minecraft Overviewer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * Minecraft Overviewer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with the Overviewer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include "overlay.h"

typedef struct {
    /* inherits from overlay */
    RenderPrimitiveOverlay parent;
} RenderPrimitiveSpawn;

static void get_color(void* data, RenderState* state,
                      uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
    RenderPrimitiveSpawn* self = (RenderPrimitiveSpawn*)data;
    int32_t x = state->x, y = state->y, z = state->z;
    int32_t y_light = y + 1;
    uint8_t blocklight, skylight;

    /* set a nice, pretty red color */
    *r = self->parent.color->r;
    *g = self->parent.color->g;
    *b = self->parent.color->b;

    /* default to no overlay, until told otherwise */
    *a = 0;

    if (block_has_property(state->block, NOSPAWN)) {
        /* nothing can spawn on this */
        return;
    }

    blocklight = get_data(state, BLOCKLIGHT, x, y_light, z);
    skylight = get_data(state, SKYLIGHT, x, y_light, z);

    if (OV_MAX(blocklight, skylight) <= 7) {
        /* hostile mobs spawn in daylight */
        *a = 240;
    } else if (OV_MAX(blocklight, skylight - 11) <= 7) {
        /* hostile mobs spawn at night */
        *a = 150;
    }
}

static bool
overlay_spawn_start(void* data, RenderState* state, PyObject* support) {
    RenderPrimitiveSpawn* self;

    /* first, chain up */
    bool ret = primitive_overlay.start(data, state, support);
    if (ret != false)
        return ret;

    /* now do custom initializations */
    self = (RenderPrimitiveSpawn*)data;
    self->parent.get_color = get_color;

    self->parent.default_color.r = 229;
    self->parent.default_color.g = 36;
    self->parent.default_color.b = 38;
    self->parent.default_color.a = 0;

    return false;
}

static void
overlay_spawn_finish(void* data, RenderState* state) {
    /* chain up */
    primitive_overlay.finish(data, state);
}

RenderPrimitiveInterface primitive_overlay_spawn = {
    "overlay-spawn",
    sizeof(RenderPrimitiveSpawn),
    overlay_spawn_start,
    overlay_spawn_finish,
    NULL,
    NULL,
    overlay_draw,
};
