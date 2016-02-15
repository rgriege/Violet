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
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

namespace sceneNamespace
{
	struct SceneFileSaver
	{
		std::vector<handle> entity_ids;
		std::unique_ptr<serializer> scene_serializer;
		std::mutex mutex;

		SceneFileSaver(std::unique_ptr<serializer> && serializer) :
			scene_serializer(std::move(serializer))
		{
		}

		void onPoolSaved(const tag componentTag, const std::string & poolFileName, const u32 count)
		{
			if (count > 0)
			{
				const std::lock_guard<std::mutex> lk(mutex);
				scene_serializer->write_string("cpnt", componentTag.as_string().c_str());
				scene_serializer->write_string("file", poolFileName.c_str());
			}
		}
	};
}

using namespace sceneNamespace;

// ============================================================================

void scene::install_component(const tag tag, const pool_factory::producer & producer)
{
	assert(!ms_poolFactory.has(tag));
	ms_poolFactory.assign(tag, producer);
}

// ----------------------------------------------------------------------------

void scene::uninstall_component(const tag tag)
{
	assert(ms_poolFactory.has(tag));
	ms_poolFactory.remove(tag);
}

// ============================================================================

scene::pool_factory scene::ms_poolFactory;

// ============================================================================

scene::scene() :
	m_handleManager(),
	m_pools()
{
	for (auto const & entry : ms_poolFactory.producers)
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

static void delete_pool(void * pool)
{
	delete static_cast<component_pool*>(pool);
}

scene::~scene()
{
	for (auto & pool : m_pools)
	{
		component_pool * moved_pool = new component_pool(std::move(pool));
		add_task(delete_pool, moved_pool, moved_pool->metadata->thread, task_type::del);
	}
}

// ----------------------------------------------------------------------------

struct load_pool_data
{
	component_pool * pool;
	std::string filename;
	std::shared_ptr<std::unordered_map<u32, handle>> handle_map;
};

static void load_pool(void * mem)
{
	auto data = make_unique<load_pool_data>(mem);
	auto pool_deserializer = file_deserializer_factory::instance().create(data->filename.c_str());
	if (pool_deserializer && pool_deserializer->is_valid())
	{
		const u32 version = pool_deserializer->get_u32("version");
		component_deserializer c(std::move(pool_deserializer), version, data->handle_map);
		data->pool->load(c);
	}
	else
	{
		log(formatted_string<128>().sprintf("Could not open component file '%s'", data->filename.c_str()));
	}
}

std::vector<handle> scene::load(const char * const filename)
{
	std::vector<handle> loadedEntityIds;
	auto deserializer = file_deserializer_factory::instance().create(filename);
	if (deserializer == nullptr)
	{
		log(formatted_string<128>().sprintf("Could not open scene file '%s'", filename));
	}
	else if (!deserializer->is_valid())
	{
		log(formatted_string<128>().sprintf("Failed to parse scene file '%s'", filename));
	}
	else
	{
		auto handle_map = std::make_shared<std::unordered_map<u32, handle>>();
		const std::string idFileName = deserializer->get_string("idFile");
		auto idFileDeserializer = file_deserializer_factory::instance().create(idFileName.c_str());
		if (idFileDeserializer != nullptr && idFileDeserializer->is_valid())
		{
			while (idFileDeserializer->is_valid())
			{
				const u32 originalId = idFileDeserializer->get_u32("id");
				const handle createdHandle = m_handleManager.create();
				(*handle_map)[originalId] = createdHandle;
				loadedEntityIds.emplace_back(createdHandle);
			}

			while (deserializer->is_valid())
			{
				const tag componentTag = tag(deserializer->get_string("cpnt"));
				component_pool * pool = get_pool(componentTag);
				if (pool != nullptr)
				{
					const std::string pool_file_name = deserializer->get_string("file");

					load_pool_data * task_data = new load_pool_data{ pool, pool_file_name, handle_map };
					add_task(load_pool, task_data, pool->metadata->thread, task_type::read);
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

struct save_pool_task_data
{
	const component_pool * pool;
	std::string filename;
	std::shared_ptr<SceneFileSaver> scene_file_saver;
};

static void save_pool_task(void * mem)
{
	auto data = make_unique<save_pool_task_data>(mem);
	auto poolSerializer = file_serializer_factory::instance().create(data->filename.c_str());
	if (poolSerializer != nullptr)
	{
		const u32 count = data->pool->save(*poolSerializer, data->scene_file_saver->entity_ids);
		data->scene_file_saver->onPoolSaved(data->pool->metadata->tag, data->filename, count);
		if (count == 0)
		{
			poolSerializer.reset();
			std::remove(data->filename.c_str());
		}
	}
	else
		log(formatted_string<128>().sprintf("Could not save component pool file '%s'", data->filename.c_str()));
}

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

			std::shared_ptr<SceneFileSaver> sceneFileSaver = std::make_shared<SceneFileSaver>(std::move(serializer));
			sceneFileSaver->entity_ids = std::move(entityIds);
			for (const auto & pool : m_pools)
			{
				std::string poolFileName = filename;
				string_utilities::replace(poolFileName, ".", std::string(".") + pool.metadata->tag.as_string() + ".");

				add_task(save_pool_task, new save_pool_task_data{ &pool, poolFileName, sceneFileSaver }, pool.metadata->thread, task_type::read);
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

struct remove_from_pool_data
{
	component_pool * pool;
	handle entity_id;
};

static void remove_from_pool(void * mem)
{
	auto data = make_unique<remove_from_pool_data>(mem);
	data->pool->remove(data->entity_id);
}

void scene::remove_all(const handle entity_id)
{
	for (auto & pool : m_pools)
		add_task(remove_from_pool, new remove_from_pool_data{ &pool, entity_id }, pool.metadata->thread, task_type::del);

	m_handleManager.free(entity_id);
}

// ----------------------------------------------------------------------------

struct remove_all_data
{
	scene * scene;
	handle entity_id;
};

static void remove_all_internal(void * mem)
{
	auto data = make_unique<remove_all_data>(mem);
	data->scene->remove_all(data->entity_id);
}

void scene::remove_all(const handle entity_id) thread_const
{
	// todo: assign scene tasks to thread
	add_task(remove_all_internal, new remove_all_data{ const_cast<scene*>(this), entity_id }, 0, task_type::del);
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
	auto it = std::find_if(m_pools.begin(), m_pools.end(), [=](component_pool & pool) { return pool.metadata->tag == componentTag; });
	return it == m_pools.end() ? nullptr : &*it;
}

// ----------------------------------------------------------------------------

const component_pool * scene::get_pool(const tag componentTag) const
{
	auto it = std::find_if(m_pools.cbegin(), m_pools.cend(), [=](const component_pool & pool) { return pool.metadata->tag == componentTag; });
	return it == m_pools.cend() ? nullptr : &*it;
}

// ============================================================================
