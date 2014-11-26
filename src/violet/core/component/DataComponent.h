#ifndef __VIOLET_DATA_COMPONENT_H__
#define __VIOLET_DATA_COMPONENT_H__

#include <map>
#include <string>

#include "violet/core/Config.h"
#include "violet/core/component/Component.h"

namespace Violet
{
	class VIOLET_API DataComponent : public Component
	{
	public:
		DataComponent() : Component(ComponentType::Data) {}
		~DataComponent() {}

		std::map<std::string, std::string> data;
	};
}

#endif