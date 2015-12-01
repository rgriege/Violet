#ifndef VIOLET_UpdateSystem_H
#define VIOLET_UpdateSystem_H

#include "violet/system/System.h"
#include "violet/script/Script.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class Engine;
	class Entity;
	class SystemFactory;

	class VIOLET_API UpdateSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		UpdateSystem(UpdateSystem && other);

		virtual ~UpdateSystem() override = default;
		virtual void update(float dt, const Engine & engine) override;

	private:

		UpdateSystem();
	};

	DEFINE_METHOD(UpdateMethod, void(const Entity &, const Engine &, float));
}

#endif