#include <stdlib.h>
#include <string.h>

#include "violet/scene/scene.h"
#include "violet/structures/array.h"

/* component_pool */

typedef struct component_pool
{
	const u32 tag;
	const u32 component_size;
	array data;
	array ids;
} component_pool;


component_pool * component_pool_create(const uint32_t tag, const uint32_t component_size)
{
	component_pool temp = {
		.tag = tag,
		.component_size = component_size,
		.data = vector_create(component_size),
		.ids = vector_create(4)
	};
	component_pool * pool = malloc(sizeof(component_pool));
	memcpy(pool, &temp, sizeof(*pool));
	return pool;
}

void component_pool_destroy(component_pool * pool)
{
	vector_destroy(pool->data);
	vector_destroy(pool->ids);
}

/* scene */

scene * scene_create()
{

}

void scene_destroy(scene * scene)
{

}
