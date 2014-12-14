#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include "violet/core/component/Component.h"
#include "violet/core/math/Polygon.h"
#include "violet/core/script/Script.h"

#include <memory>

namespace Violet
{
	class Deserializer;

	class InputComponent : public Component
	{
	public:

		static const char * getLabel();

	public:

		InputComponent(const Entity & entity, Deserializer & deserializer);
		InputComponent(InputComponent && other);
		InputComponent(const InputComponent &) = delete;

	public:

		Polygon m_mesh;
		std::unique_ptr<Script> m_script;
	};
}

#endif