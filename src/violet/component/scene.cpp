// ============================================================================

#include "violet/component/scene.h"

#include "violet/core/engine.h"
#include "violet/component/component_deserializer.h"
#include "violet/handle/handle_manager.h"
#include "violet/log/Log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/file/file_serializer_factory.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/formatted_string.h"

using namespace vlt;

// ============================================================================

namespace sceneNamespace
{
	struct SceneFileSaver
	{
	public:

		SceneFileSaver(std::unique_ptr<serializer> && serializer) :
			m_serializer(std::move(serializer))
		{
		}

		void onPoolSaved(const tag componentTag, const std::string & poolFileName, const u32 count)
		{
			if (count > 0)
			{
				const std::lock_guard<std::mutex> lk(m_mutex);
				m_serializer->write_string("cpnt", componentTag.as_string().c_str());
				m_serializer->write_string("file", poolFileName.c_str());
			}
		}

	private:

		std::unique_ptr<serializer> m_serializer;
		std::mutex m_mutex;
	};
}

using namespace sceneNamespace;

// ============================================================================

void scene::install_component(const tag tag, const pool_factory::producer & producer, const thread thread)
{
	assert(!ms_poolFactory.has(tag));
	ms_poolFactory.assign(tag, producer);
	ms_poolThreads[tag] = thread;
}

// ----------------------------------------------------------------------------

void scene::uninstall_component(const tag tag)
{
	assert(ms_poolFactory.has(tag));
	ms_poolFactory.remove(tag);
	ms_poolThreads.erase(tag);
}

// ============================================================================

scene::pool_factory scene::ms_poolFactory;
std::map<tag, thread> scene::ms_poolThreads;

// ============================================================================

scene::scene() :
	m_handleManager(),
	m_pools()
{
	for (auto const & entry : ms_poolThreads)
		m_pools.emplace_back(ms_poolFactory.create(entry.first));
}

// ----------------------------------------------------------------------------

scene::scene(scene && other) :
	m_handleManager(std::move(other.m_handleManager)),
	m_pools(std::move(other.m_pools))
{
}

// ----------------------------------------------------------------------------

scene & scene::operator=(scene && other)
{
	std::swap(m_handleManager, other.m_handleManager);
	std::swap(m_pools, other.m_pools);
	return *this;
}

// ----------------------------------------------------------------------------

scene::~scene()
{
	for (auto & pool : m_pools)
	{
		component_pool * movedPool = new component_pool(std::move(pool));
		engine::instance().add_delete_task(std::make_unique<delegate_task>(
			[=]()
			{
				delete movedPool;
			}), ms_poolThreads[pool.get_tag()]);
	}
}

// ----------------------------------------------------------------------------

std::vector<handle> scene::load(const char * const filename)
{
	std::vector<handle> loadedEntityIds;
	auto deserializer = file_deserializer_factory::instance().create(filename);
	if (deserializer == nullptr)
		log(formatted_string<128>().sprintf("Could not open scene file '%s'", filename));
	else if (!deserializer->is_valid())
		log(formatted_string<128>().sprintf("Failed to parse scene file '%s'", filename));
	else
	{
		auto handleIdMap = std::make_shared<std::unordered_map<u32, handle>>();
		const std::string idFileName = deserializer->get_string("idFile");
		auto idFileDeserializer = file_deserializer_factory::instance().create(idFileName.c_str());
		if (idFileDeserializer != nullptr && idFileDeserializer->is_valid())
		{
			while (idFileDeserializer->is_valid())
			{
				const u32 originalId = idFileDeserializer->get_u32("id");
				const handle createdHandle = m_handleManager.create();
				(*handleIdMap)[originalId] = createdHandle;
				loadedEntityIds.emplace_back(createdHandle);
			}

			while (deserializer->is_valid())
			{
				const tag componentTag = tag(deserializer->get_string("cpnt"));
				const component_pool * pool = get_pool(componentTag);
				if (pool != nullptr)
				{
					const std::string poolFileName = deserializer->get_string("file");
					auto threadIt = ms_poolThreads.find(componentTag);
					assert(threadIt != ms_poolThreads.end());
					engine::instance().add_write_task(*pool,
					[=](component_pool & pool)
					{
						auto poolDeserializer = file_deserializer_factory::instance().create(poolFileName.c_str());
						if (poolDeserializer != nullptr && poolDeserializer->is_valid())
						{
							const u32 version = poolDeserializer->get_u32("version");
							component_deserializer componentDeserializer(std::move(poolDeserializer), version, handleIdMap);
							pool.load(componentDeserializer);
						}
						else
							log(formatted_string<128>().sprintf("Could not open component pool file '%s'", poolFileName.c_str()));
					}, threadIt->second);
				}
				else
					log(formatted_string<256>().sprintf("Error loading '%s': no component pool for tag '%s'", idFileName.c_str(), componentTag.as_string().c_str()));
			}
		}
		else
			log(formatted_string<128>().sprintf("Could not open scene id file '%s'", idFileName.c_str()));
	}
	log(formatted_string<32>().sprintf("loaded %d entities", loadedEntityIds.size()));
	return loadedEntityIds;
}

// ----------------------------------------------------------------------------

void scene::save(const char * filename) const
{
	save(filename, std::move(get_entity_ids()));
}

// ----------------------------------------------------------------------------

void scene::save(const char * filename, std::vector<handle> entityIds) const
{
	log(formatted_string<32>().sprintf("saving %d entities", entityIds.size()));
	auto serializer = file_serializer_factory::instance().create(filename);
	if (serializer == nullptr)
		log(formatted_string<128>().sprintf("Could not save scene file '%s'", filename));
	else
	{
		std::string idFileName = filename;
		string_utilities::replace(idFileName, ".", ".hndl.");
		serializer->write_string("idFile", idFileName.c_str());
		auto idFileSerializer = file_serializer_factory::instance().create(idFileName.c_str());
		if (idFileSerializer != nullptr)
		{
			for (const auto & entity_id : entityIds)
				idFileSerializer->write_u32("id", entity_id.id);
			idFileSerializer.reset();

			auto sharedEntityIds = std::make_shared<std::vector<handle>>(std::move(entityIds));
			std::shared_ptr<SceneFileSaver> sceneFileSaver = std::make_shared<SceneFileSaver>(std::move(serializer));
			for (const auto & pool : m_pools)
			{
				std::string poolFileName = filename;
				string_utilities::replace(poolFileName, ".", std::string(".") + pool.get_tag().as_string() + ".");

				engine::instance().add_read_task(std::make_unique<delegate_task>(
				[&pool, poolFileName, sharedEntityIds, sceneFileSaver]()
				{
					auto poolSerializer = file_serializer_factory::instance().create(poolFileName.c_str());
					if (poolSerializer != nullptr)
					{
						const u32 count = pool.save(*poolSerializer, *sharedEntityIds);
						sceneFileSaver->onPoolSaved(pool.get_tag(), poolFileName, count);
						if (count == 0)
						{
							poolSerializer.reset();
							std::remove(poolFileName.c_str());
						}
					}
					else
						log(formatted_string<128>().sprintf("Could not save component pool file '%s'", poolFileName.c_str()));
				}), ms_poolThreads[pool.get_tag()]);
			}
		}
		else
			log(formatted_string<128>().sprintf("Could not open scene id file '%s'", idFileName.c_str()));
	}
}

// ----------------------------------------------------------------------------

handle scene::create_entity()
{
	return m_handleManager.create();
}

// ----------------------------------------------------------------------------

bool scene::exists(const handle entity_id) const
{
	return m_handleManager.used(entity_id);
}

// ----------------------------------------------------------------------------

std::vector<handle> scene::get_entity_ids() const
{
	return m_handleManager.getUsed();
}

// ----------------------------------------------------------------------------

u16 scene::get_entity_version(const u16 id) const
{
	return m_handleManager.get_version(id);
}

// ----------------------------------------------------------------------------

void scene::remove_all(const handle entity_id)
{
	for (auto & pool : m_pools)
	{
		engine::instance().add_write_task(pool,
			[=](component_pool & pool)
			{
				pool.remove(entity_id);
			}, ms_poolThreads[pool.get_tag()]);
	}

	m_handleManager.free(entity_id);
}

// ----------------------------------------------------------------------------

void scene::remove_all(const handle entity_id) thread_const
{
	engine::instance().add_write_task(*this,
		[=](scene & manager)
		{
			manager.remove_all(entity_id);
		});
}

// ----------------------------------------------------------------------------

void scene::clear()
{
	for (auto & pool : m_pools)
		pool.clear();
	m_handleManager.freeAll();
}

// ============================================================================

component_pool * scene::get_pool(const tag componentTag)
{
	auto it = std::find_if(m_pools.begin(), m_pools.end(), [=](component_pool & pool) { return pool.get_tag() == componentTag; });
	return it == m_pools.end() ? nullptr : &*it;
}

// ----------------------------------------------------------------------------

const component_pool * scene::get_pool(const tag componentTag) const
{
	auto it = std::find_if(m_pools.cbegin(), m_pools.cend(), [=](const component_pool & pool) { return pool.get_tag() == componentTag; });
	return it == m_pools.cend() ? nullptr : &*it;
}

// ============================================================================
