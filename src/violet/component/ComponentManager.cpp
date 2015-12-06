// ============================================================================

#include "violet/component/ComponentManager.h"

#include "violet/Engine.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

void ComponentManager::installComponent(const Tag tag, const PoolFactory::Producer & producer, const ComponentFactory::Producer & cProducer, const ComponentsFactory::Producer & csProducer, const Thread thread)
{
	const std::string & label = tag.asString();
	assert(!ms_poolFactory.has(label));
	ms_poolFactory.assign(label, producer);
	ms_componentFactory.assign(label, cProducer);
	ms_componentsFactory.assign(label, csProducer);
	ms_poolThreads[tag] = thread;
}

// ----------------------------------------------------------------------------

void ComponentManager::uninstallComponent(const Tag tag)
{
	const std::string & label = tag.asString();
	assert(ms_poolFactory.has(label));
	ms_poolFactory.remove(label);
	ms_componentFactory.remove(label);
	ms_componentsFactory.remove(label);
	ms_poolThreads.erase(tag);
}

// ============================================================================

Factory<std::string, ComponentPool(ComponentManager &)> ComponentManager::ms_poolFactory;
Factory<std::string, void(ComponentManager &, Deserializer &, Handle)> ComponentManager::ms_componentFactory;
Factory<std::string, void(ComponentManager &, Deserializer &)> ComponentManager::ms_componentsFactory;
std::map<Tag, Thread> ComponentManager::ms_poolThreads;

// ============================================================================

ComponentManager::ComponentManager() :
	m_pools()
{
	for (auto const & entry : ms_poolThreads)
		m_pools.emplace_back(ms_poolFactory.create(entry.first.asString(), *this));
}

// ----------------------------------------------------------------------------

/*ComponentManager::ComponentManager(ComponentManager && other) :
	m_pools(std::move(other.m_pools))
{
}

// ----------------------------------------------------------------------------

ComponentManager & ComponentManager::operator=(ComponentManager && other)
{
	std::swap(m_pools, other.m_pools);
	return *this;
}*/

// ----------------------------------------------------------------------------

void ComponentManager::load(const char * const filename)
{
	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		Log::log(FormattedString<128>().sprintf("Could not open scene file '%s'", filename));
	else if (!*deserializer)
		Log::log(FormattedString<128>().sprintf("Failed to parse scene file '%s'", filename));
	else
	{
		while (*deserializer)
		{
			const std::string poolTag = deserializer->getString("cpnt");
			const std::string poolFileName = deserializer->getString("file");
			auto threadIt = ms_poolThreads.find(Tag(poolTag.c_str()));
			assert(threadIt != ms_poolThreads.end());
			Engine::getInstance().addWriteTask(*this,
				[=](ComponentManager & manager)
				{
					auto poolDeserializer = FileDeserializerFactory::getInstance().create(poolFileName.c_str());
					if (poolDeserializer != nullptr)
						ms_componentsFactory.create(poolTag, manager, *poolDeserializer);
					else
						Log::log(FormattedString<128>().sprintf("Could not open component pool file '%s'", poolFileName.c_str()));
				}, threadIt->second);
		}
	}
}

// ----------------------------------------------------------------------------

void ComponentManager::loadEntity(const Handle entityId, const char * const filename)
{
	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		Log::log(FormattedString<128>().sprintf("Could not open scene file '%s'", filename));
	else if (!*deserializer)
		Log::log(FormattedString<128>().sprintf("Failed to parse scene file '%s'", filename));
	else
	{
		while (*deserializer)
		{
			const std::string poolTag = deserializer->getString("cpnt");
			const std::string poolFileName = deserializer->getString("file");
			auto threadIt = ms_poolThreads.find(Tag(poolTag.c_str()));
			assert(threadIt != ms_poolThreads.end());
			Engine::getInstance().addWriteTask(*this,
				[=](ComponentManager & manager)
				{
					auto poolDeserializer = FileDeserializerFactory::getInstance().create(poolFileName.c_str());
					if (poolDeserializer != nullptr)
						ms_componentFactory.create(poolTag, manager, *poolDeserializer, entityId);
					else
						Log::log(FormattedString<128>().sprintf("Could not open component pool file '%s'", poolFileName.c_str()));
				}, threadIt->second);
		}
	}
}

// ----------------------------------------------------------------------------

bool ComponentManager::removeAll(const Handle entityId)
{
	bool found = false;
	for (auto & pool : m_pools)
		found |= pool.remove(entityId);

	return found;
}

// ----------------------------------------------------------------------------

void ComponentManager::clear()
{
	for (auto & pool : m_pools)
		pool.clear();
}

// ============================================================================

ComponentPool * ComponentManager::getPool(const Tag componentTag)
{
	auto it = std::find_if(m_pools.begin(), m_pools.end(), [=](ComponentPool & pool) { return pool.getComponentTag() == componentTag; });
	return it == m_pools.end() ? nullptr : &*it;
}

// ----------------------------------------------------------------------------

const ComponentPool * ComponentManager::getPool(const Tag componentTag) const
{
	auto it = std::find_if(m_pools.cbegin(), m_pools.cend(), [=](const ComponentPool & pool) { return pool.getComponentTag() == componentTag; });
	return it == m_pools.cend() ? nullptr : &*it;
}

// ============================================================================
