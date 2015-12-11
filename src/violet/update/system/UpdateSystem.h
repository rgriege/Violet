#ifndef VIOLET_UpdateSystem_H
#define VIOLET_UpdateSystem_H

#include "violet/handle/Handle.h"
#include "violet/system/System.h"
#include "violet/script/Script.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API UpdateSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		UpdateSystem(UpdateSystem && other);

		virtual ~UpdateSystem() override;
		virtual void update(float dt) override;

	private:

		UpdateSystem();
	};

	DEFINE_METHOD(UpdateMethod, void(EntityId, float));
}

#endif