#ifndef VIOLET_UpdateSystem_H
#define VIOLET_UpdateSystem_H

#include "engine/system/System.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API UpdateSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		UpdateSystem(UpdateSystem && other);

		virtual ~UpdateSystem() override = default;
		virtual void update(float dt, const Engine & engine) override;

	private:

		UpdateSystem();
	};
}

#endif