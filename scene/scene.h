#ifndef VIOLET_SCENE_H
#define VIOLET_SCENE_H

#include "violet/structures/array.h"

typedef struct scene
{
	array component_pools;
} scene;

void scene_create(scene * s);
void scene_destroy(scene * s);

void scene_load(scene * s, const char * name);
void scene_save(scene * s, const char * name);

handle scene_create_entity(scene * s);

#endif
