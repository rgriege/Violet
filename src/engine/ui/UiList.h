#ifndef VIOLET_UiList_H
#define VIOLET_UiList_H

#include "engine/input/system/InputSystem.h"
#include "engine/serialization/file/FileDeserializerFactory.h"
#include "engine/transform/component/TransformComponent.h"

#include <iostream>

namespace Violet
{
	class Engine;
	class Entity;

	class VIOLET_API UiList
	{
	public:

		UiList(const char * elementFileName);

		void update(const Entity & entity, const Engine & engine, uint32 elementCount);

		void clean(Script & script);

	private:

		std::string m_elementFileName;
	};
}

#endif