#ifndef EDITOR_INPUT_SYSTEM_H
#define EDITOR_INPUT_SYSTEM_H

#include "violet/core/system/System.h"
#include "violet/core/system/SystemFactory.h"

namespace Violet
{
	class Deserializer;
}

template <typename SystemType>
class EditorSystem : public SystemType
{
public:

	static void EditorSystem::install(Violet::SystemFactory & factory)
	{
		factory.assign(getStaticLabel(), &EditorSystem<SystemType>::init);
	}

	static std::unique_ptr<Violet::System> EditorSystem::init(Violet::Deserializer & deserializer)
	{
		auto system = SystemType::init(deserializer);
		return std::unique_ptr<Violet::System>(new EditorSystem(std::move(dynamic_cast<SystemType &>(*system))));
	}

public:

	EditorSystem(SystemType && system) :
		SystemType(std::move(system))
	{
	}

	virtual void update(float /*dt*/, Violet::Engine & /*engine*/) override
	{
	}
};

#endif