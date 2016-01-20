#ifndef VIOLET_TransformSystem_H
#define VIOLET_TransformSystem_H

#include "violet/handle/Handle.h"
#include "violet/math/Matrix4.h"
#include "violet/system/System.h"

#include <map>

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API TransformSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		TransformSystem(TransformSystem && other);

		virtual ~TransformSystem() override;
		virtual void update(float dt) override;

	private:

		TransformSystem();

	private:

		std::map<EntityId, Matrix4f> m_entityWorldTransformCache;
	};
}

#endif