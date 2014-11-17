#ifndef COLLISION_RESOLVER_H
#define COLLISION_RESOLVER_H

namespace Violet
{
	class Intersection;

	class CollisionResolver
	{
	public:

		static void resolve(Intersection & intersection);
	};
}

#endif