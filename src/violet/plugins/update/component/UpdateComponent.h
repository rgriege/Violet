#ifndef UPDATE_COMPONENT_H
#define UPDATE_COMPONENT_H

#include "violet/core/component/Component.h"


#ifdef WIN32
#ifdef VIOLETUPDATE_EXPORT
#define VIOLET_UPDATE_API __declspec(dllexport)
#else
#define VIOLET_UPDATE_API __declspec(dllimport)
#endif
#else
#define VIOLET_API
#endif

namespace Violet
{
	class Deserializer;

	class VIOLET_UPDATE_API UpdateComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		UpdateComponent(const Entity & entity, Deserializer & deserializer);
		UpdateComponent(UpdateComponent && other);
		UpdateComponent & operator=(UpdateComponent && other);
	};
}

#endif