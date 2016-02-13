#ifndef VIOLET_UiSystem_H
#define VIOLET_UiSystem_H

#include "violet/handle/Handle.h"
#include "violet/system/System.h"

namespace Violet
{
	class Deserializer;
	class SystemFactory;

	class VIOLET_API UiSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static void init(Deserializer & deserializer);

	public:

		UiSystem(UiSystem && other);

		virtual ~UiSystem() override;
		virtual void update(float dt) override;

	private:

		UiSystem();
	};
}

#endif