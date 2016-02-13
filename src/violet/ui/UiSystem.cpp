// ============================================================================

#include "violet/ui/UiSystem.h"

#include "violet/Engine.h"
#include "violet/component/ComponentManager.h"
#include "violet/serialization/Deserializer.h"
#include "violet/system/SystemFactory.h"
#include "violet/ui/AlignmentComponent.h"
#include "violet/window/WindowSystem.h"

using namespace Violet;

// ============================================================================

const char * UiSystem::getStaticLabel()
{
	return "guis";
}

// ----------------------------------------------------------------------------

void UiSystem::install(SystemFactory & factory)
{
	factory.assign(getStaticLabel(), &UiSystem::init);
}

// ----------------------------------------------------------------------------

void UiSystem::init(Deserializer & deserializer)
{
	deserializer.enterSegment(getStaticLabel());

	Engine::getInstance().addWriteTask(Engine::getInstance(),
		[](Engine & engine)
		{
			engine.addSystem(std::unique_ptr<System>(new UiSystem));
		});
}

// ============================================================================

UiSystem::UiSystem(UiSystem && other) :
	System(std::move(other))
{
}

// ----------------------------------------------------------------------------

UiSystem::~UiSystem()
{
}

// ---------------------------------------------------------------------------

void UiSystem::update(const float /*dt*/)
{
	const auto & componentManager = Engine::getInstance().getCurrentScene();
	for (auto entity : componentManager.getEntityView<AlignmentComponent>())
	{
		const auto & alignment = std::get<0>(entity);
		if (alignment.m_parent.isValid())
		{
			const auto * parentAlignment = componentManager.getComponent<AlignmentComponent>(alignment.m_parent);
			assert(false);
		}
		else
		{
			const auto & window = Engine::getInstance().getSystem<WindowSystem>();
			const Vec2f center = {
				alignment.m_x < 1.f ? alignment.m_x * -window->getWidth() / 2 : alignment.m_x,
				alignment.m_y < 1.f ? window->getHeight() / 2 : alignment.m_y,
			};
			const Vec2f halfDim = {
				alignment.m_width,
				alignment.m_height
			};
			AABB dimensions(
				alignment.m_x < 1.f ? alignment.m_x * -window->getWidth() / 2 : alignment.m_x,
				alignment.m_y < 1.f ? window->getHeight() / 2 : alignment.m_y,
				window->getWidth() / 2,
				window->getHeight() / 2
				);
		}
	}
}

// ============================================================================

UiSystem::UiSystem() :
	System(getStaticLabel())
{
}

// ============================================================================