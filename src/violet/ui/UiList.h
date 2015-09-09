#ifndef VIOLET_UiList_H
#define VIOLET_UiList_H

#include "violet/input/system/InputSystem.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/TransformComponent.h"

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

	DEFINE_METHOD(AssignIndexMethod, void(const Entity &, const Engine &, uint32 index));
}

#endif