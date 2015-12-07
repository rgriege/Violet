// ============================================================================

#include "violet/component/ComponentManager.h"

#include "violet/Engine.h"
#include "violet/handle/HandleManager.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

void ComponentManager::installComponent(const Tag tag, const PoolFactory::Producer & producer, const ComponentsFactory::Producer & csProducer, const Thread thread)
{
	assert(!ms_poolFactory.has(tag));
	ms_poolFactory.assign(tag, producer);
	ms_componentsFactory.assign(tag, csProducer);
	ms_poolThreads[tag] = thread;
}

// ----------------------------------------------------------------------------

void ComponentManager::uninstallComponent(const Tag tag)
{
	assert(ms_poolFactory.has(tag));
	ms_poolFactory.remove(tag);
	ms_componentsFactory.remove(tag);
	ms_poolThreads.erase(tag);
}

// ============================================================================

ComponentManager::PoolFactory ComponentManager::ms_poolFactory;
ComponentManager::ComponentsFactory ComponentManager::ms_componentsFactory;
std::map<Tag, Thread> ComponentManager::ms_poolThreads;

// ============================================================================

ComponentManager::ComponentManager() :
	m_handleManager(),
	m_pools()
{
	for (auto const & entry : ms_poolThreads)
		m_pools.emplace_back(ms_poolFactory.create(entry.first));
}

// ----------------------------------------------------------------------------

ComponentManager::ComponentManager(ComponentManager && other) :
	m_handleManager(std::move(other.m_handleManager)),
	m_pools(std::move(other.m_pools))
{
}

// ----------------------------------------------------------------------------

ComponentManager & ComponentManager::operator=(ComponentManager && other)
{
	std::swap(m_handleManager, other.m_handleManager);
	std::swap(m_pools, other.m_pools);
	return *this;
}

// ----------------------------------------------------------------------------

ComponentManager::~ComponentManager()
{
	for (auto & pool : m_pools)
	{
		ComponentPool * movedPool = new ComponentPool(std::move(pool));
		Engine::getInstance().addDeleteTask(std::make_unique<DelegateTask>(
			[=]()
			{
				delete movedPool;
			}), ms_poolThreads[pool.getComponentTag()]);
	}
}

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
		const std::string idFileName = deserializer->getString("idFile");
		auto idFileDeserializer = FileDeserializerFactory::getInstance().create(idFileName.c_str());
		if (idFileDeserializer != nullptr && *idFileDeserializer)
		{
			auto handleReplacementMap = std::make_shared<std::unordered_map<uint32, Handle>>();
			while (*idFileDeserializer)
			{
				const uint32 desiredId = idFileDeserializer->getUint("id");
				const Handle actualHandle = m_handleManager.create(desiredId);
				if (desiredId != actualHandle.getId())
					(*handleReplacementMap)[desiredId] = actualHandle;
			}

			while (*deserializer)
			{
				const Tag poolTag = Tag(deserializer->getString("cpnt"));
				const std::string poolFileName = deserializer->getString("file");
				auto threadIt = ms_poolThreads.find(poolTag);
				assert(threadIt != ms_poolThreads.end());
				Engine::getInstance().addWriteTask(*getPool(poolTag),
					[=](ComponentPool & pool)
					{
						auto poolDeserializer = FileDeserializerFactory::getInstance().create(poolFileName.c_str());
						if (poolDeserializer != nullptr && *poolDeserializer)
							ms_componentsFactory.create(poolTag, pool, *poolDeserializer, *handleReplacementMap);
						else
							Log::log(FormattedString<128>().sprintf("Could not open component pool file '%s'", poolFileName.c_str()));
					}, threadIt->second);
			}
		}
		else
			Log::log(FormattedString<128>().sprintf("Could not open scene id file '%s'", idFileName.c_str()));
	}
}

// ----------------------------------------------------------------------------

void ComponentManager::removeAll(const Handle entityId)
{
	for (auto & pool : m_pools)
	{
		Engine::getInstance().addWriteTask(pool,
			[=](ComponentPool & pool)
			{
				pool.remove(entityId);
			}, ms_poolThreads[pool.getComponentTag()]);
	}

	m_handleManager.free(entityId);
}

// ----------------------------------------------------------------------------

void ComponentManager::removeAll(const Handle entityId) thread_const
{
	Engine::getInstance().addWriteTask(*this,
		[=](ComponentManager & manager)
		{
			manager.removeAll(entityId);
		});
}

// ----------------------------------------------------------------------------

void ComponentManager::clear()
{
	for (auto & pool : m_pools)
		pool.clear();
	m_handleManager.freeAll();
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
