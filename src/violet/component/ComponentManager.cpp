// ============================================================================

#include "violet/component/ComponentManager.h"

#include "violet/Engine.h"
#include "violet/component/ComponentDeserializer.h"
#include "violet/handle/HandleManager.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/FileDeserializerFactory.h"
#include "violet/serialization/file/FileSerializerFactory.h"
#include "violet/transform/component/WorldTransformComponent.h"
#include "violet/utility/FormattedString.h"

using namespace Violet;

// ============================================================================

namespace ComponentManagerNamespace
{
	class SceneFileSaver
	{
	public:

		SceneFileSaver(std::unique_ptr<Serializer> && serializer) :
			m_serializer(std::move(serializer))
		{
		}

		void onPoolSaved(const Tag componentTag, const std::string & poolFileName, const uint32 count)
		{
			if (count > 0)
			{
				const std::lock_guard<std::mutex> lk(m_mutex);
				m_serializer->writeString("cpnt", componentTag.asString().c_str());
				m_serializer->writeString("file", poolFileName.c_str());
			}
		}

	private:

		std::unique_ptr<Serializer> m_serializer;
		std::mutex m_mutex;
	};
}

using namespace ComponentManagerNamespace;

// ============================================================================

void ComponentManager::installComponent(const Tag tag, const PoolFactory::Producer & producer, const ComponentsFactory::Producer & csProducer, const PoolSaveFactory::Producer & sProducer, const Thread thread)
{
	assert(!ms_poolFactory.has(tag));
	ms_poolFactory.assign(tag, producer);
	ms_componentsFactory.assign(tag, csProducer);
	ms_saveFactory.assign(tag, sProducer);
	ms_poolThreads[tag] = thread;
}

// ----------------------------------------------------------------------------

void ComponentManager::uninstallComponent(const Tag tag)
{
	assert(ms_poolFactory.has(tag));
	ms_poolFactory.remove(tag);
	ms_componentsFactory.remove(tag);
	ms_saveFactory.remove(tag);
	ms_poolThreads.erase(tag);
}

// ============================================================================

ComponentManager::PoolFactory ComponentManager::ms_poolFactory;
ComponentManager::ComponentsFactory ComponentManager::ms_componentsFactory;
ComponentManager::PoolSaveFactory ComponentManager::ms_saveFactory;
std::map<Tag, Thread> ComponentManager::ms_poolThreads;

// ============================================================================

ComponentManager::ComponentManager() :
	m_handleManager(),
	m_handleManagerRecycleList(),
	m_pools()
{
	for (auto const & entry : ms_poolThreads)
		m_pools.emplace_back(ms_poolFactory.create(entry.first));
}

// ----------------------------------------------------------------------------

ComponentManager::ComponentManager(ComponentManager && other) :
	m_handleManager(std::move(other.m_handleManager)),
	m_handleManagerRecycleList(std::move(other.m_handleManagerRecycleList)),
	m_pools(std::move(other.m_pools))
{
}

// ----------------------------------------------------------------------------

ComponentManager & ComponentManager::operator=(ComponentManager && other)
{
	std::swap(m_handleManager, other.m_handleManager);
	std::swap(m_handleManagerRecycleList, other.m_handleManagerRecycleList);
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

std::vector<EntityId> ComponentManager::load(const char * const filename, const TagMap & tagMap)
{
	std::vector<EntityId> loadedEntityIds;
	auto deserializer = FileDeserializerFactory::getInstance().create(filename);
	if (deserializer == nullptr)
		Log::log(FormattedString<128>().sprintf("Could not open scene file '%s'", filename));
	else if (!*deserializer)
		Log::log(FormattedString<128>().sprintf("Failed to parse scene file '%s'", filename));
	else
	{
		auto handleIdMap = std::make_shared<std::unordered_map<uint32, EntityId>>();
		const std::string idFileName = deserializer->getString("idFile");
		auto idFileDeserializer = FileDeserializerFactory::getInstance().create(idFileName.c_str());
		if (idFileDeserializer != nullptr && *idFileDeserializer)
		{
			while (*idFileDeserializer)
			{
				const uint32 desiredId = idFileDeserializer->getUint("id");
				const EntityId createdHandle = m_handleManager.create(desiredId);
				if (createdHandle.getId() != desiredId)
					(*handleIdMap)[desiredId] = createdHandle;
				loadedEntityIds.emplace_back(createdHandle);
			}

			while (*deserializer)
			{
				const Tag componentTag = Tag(deserializer->getString("cpnt"));
				const auto tagMapIt = std::find_if(tagMap.cbegin(), tagMap.cend(), [=](const std::pair<Tag, Tag> & p) { return p.first == componentTag; });
				const Tag poolTag = tagMapIt != tagMap.end() ? tagMapIt->second : componentTag;
				const std::string poolFileName = deserializer->getString("file");
				auto threadIt = ms_poolThreads.find(poolTag);
				assert(threadIt != ms_poolThreads.end());
				Engine::getInstance().addWriteTask(*getPool(poolTag),
					[=](ComponentPool & pool)
					{
						auto poolDeserializer = FileDeserializerFactory::getInstance().create(poolFileName.c_str());
						if (poolDeserializer != nullptr && *poolDeserializer)
							ms_componentsFactory.create(poolTag, pool, ComponentDeserializer(std::move(poolDeserializer), handleIdMap));
						else
							Log::log(FormattedString<128>().sprintf("Could not open component pool file '%s'", poolFileName.c_str()));
					}, threadIt->second);
			}
		}
		else
			Log::log(FormattedString<128>().sprintf("Could not open scene id file '%s'", idFileName.c_str()));
	}
	Log::log(FormattedString<32>().sprintf("loaded %d entities", loadedEntityIds.size()));
	return loadedEntityIds;
}

// ----------------------------------------------------------------------------

void ComponentManager::save(const char * filename) const
{
	save(filename, std::move(m_handleManager.getUsed()));
}

// ----------------------------------------------------------------------------

void ComponentManager::save(const char * filename, std::vector<EntityId> entityIds, const TagMap & tagMap) const
{
	Log::log(FormattedString<32>().sprintf("saving %d entities", entityIds.size()));
	auto serializer = FileSerializerFactory::getInstance().create(filename);
	if (serializer == nullptr)
		Log::log(FormattedString<128>().sprintf("Could not save scene file '%s'", filename));
	else
	{
		std::string idFileName = filename;
		StringUtilities::replace(idFileName, ".", ".hndl.");
		serializer->writeString("idFile", idFileName.c_str());
		auto idFileSerializer = FileSerializerFactory::getInstance().create(idFileName.c_str());
		if (idFileSerializer != nullptr)
		{
			for (const auto & entityId : entityIds)
				idFileSerializer->writeUint("id", entityId.getId());
			idFileSerializer.reset();

			auto sharedEntityIds = std::make_shared<std::vector<EntityId>>(std::move(entityIds));
			std::shared_ptr<SceneFileSaver> sceneFileSaver = std::make_shared<SceneFileSaver>(std::move(serializer));
			for (const auto & pool : m_pools)
			{
				const Tag componentTag = pool.getComponentTag();
				if (std::none_of(tagMap.cbegin(), tagMap.cend(), [=](const std::pair<Tag, Tag> & p) { return p.first == componentTag; }))
				{
					const auto tagMapIt = std::find_if(tagMap.cbegin(), tagMap.cend(), [=](const std::pair<Tag, Tag> & p) { return p.second == componentTag; });
					const Tag poolTag = tagMapIt != tagMap.end() ? tagMapIt->first : pool.getComponentTag();
					std::string poolFileName = filename;
					StringUtilities::replace(poolFileName, ".", std::string(".") + poolTag.asString() + ".");

					Engine::getInstance().addReadTask(std::make_unique<DelegateTask>(
						[&pool, poolTag, poolFileName, sharedEntityIds, sceneFileSaver]()
						{
							auto poolSerializer = FileSerializerFactory::getInstance().create(poolFileName.c_str());
							if (poolSerializer != nullptr)
							{
								const uint32 count = ms_saveFactory.create(pool.getComponentTag(), pool, *poolSerializer, *sharedEntityIds);
								sceneFileSaver->onPoolSaved(poolTag, poolFileName, count);
								if (count == 0)
								{
									poolSerializer.reset();
									std::remove(poolFileName.c_str());
								}
							}
							else
								Log::log(FormattedString<128>().sprintf("Could not save component pool file '%s'", poolFileName.c_str()));
						}), ms_poolThreads[pool.getComponentTag()]);
				}
			}
		}
		else
			Log::log(FormattedString<128>().sprintf("Could not open scene id file '%s'", idFileName.c_str()));
	}
}

// ----------------------------------------------------------------------------

void ComponentManager::removeAll(const EntityId entityId)
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

void ComponentManager::removeAll(const EntityId entityId) thread_const
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
