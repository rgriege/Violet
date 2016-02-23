// ============================================================================

#include "violet/component/scene.h"

#include "violet/core/engine.h"
#include "violet/component/component_deserializer.h"
#include "violet/handle/handle_manager.h"
#include "violet/log/log.h"
#include "violet/serialization/file/file_deserializer_factory.h"
#include "violet/serialization/file/file_serializer_factory.h"
#include "violet/serialization/serializer.h"
#include "violet/utility/formatted_string.h"
#include "violet/utility/memory.h"

using namespace vlt;

// ============================================================================

struct Scene_File_Saver
{
	std::vector<Handle> entity_ids;
	std::unique_ptr<Serializer> scene_serializer;
	std::mutex mutex;

	Scene_File_Saver(std::unique_ptr<Serializer> && serializer) :
		scene_serializer(std::move(serializer))
	{
	}

	void on_pool_saved(const Tag component_tag, const std::string & pool_filename, const u32 count)
	{
		if (count > 0)
		{
			const std::lock_guard<std::mutex> lk(mutex);
			scene_serializer->write_string("cpnt", component_tag.as_string().c_str());
			scene_serializer->write_string("file", pool_filename.c_str());
		}
	}
};

// ============================================================================

Scene::Pool_Factory g_component_pool_factory;

// ============================================================================

void Scene::install_component(const Tag tag, const Pool_Factory::Producer & producer)
{
	assert(!g_component_pool_factory.has(tag));
	g_component_pool_factory.assign(tag, producer);
}

// ----------------------------------------------------------------------------

void Scene::uninstall_component(const Tag tag)
{
	assert(g_component_pool_factory.has(tag));
	g_component_pool_factory.remove(tag);
}

// ============================================================================

Scene::Scene() :
	handle_manager(),
	pools()
{
	for (auto const & entry : g_component_pool_factory.producers)
		pools.emplace_back(g_component_pool_factory.create(entry.first));
}

// ----------------------------------------------------------------------------

Scene::Scene(Scene && other) :
	handle_manager(std::move(other.handle_manager)),
	pools(std::move(other.pools))
{
}

// ----------------------------------------------------------------------------

Scene & Scene::operator=(Scene && other)
{
	std::swap(handle_manager, other.handle_manager);
	std::swap(pools, other.pools);
	return *this;
}

// ----------------------------------------------------------------------------

static void delete_pool(void * pool)
{
	delete static_cast<Component_Pool*>(pool);
}

Scene::~Scene()
{
	for (auto & pool : pools)
	{
		Component_Pool * moved_pool = new Component_Pool(std::move(pool));
		add_task(delete_pool, moved_pool, moved_pool->metadata->thread, task_type::del);
	}
}

// ----------------------------------------------------------------------------

struct load_pool_data
{
	Component_Pool * pool;
	std::string filename;
	std::shared_ptr<std::unordered_map<u32, Handle>> handle_map;
};

static void load_pool(void * mem)
{
	auto data = make_unique<load_pool_data>(mem);
	auto deserializer = File_Deserializer_Factory::instance().create(data->filename.c_str());
	if (deserializer && deserializer->is_valid())
	{
		const u32 version = deserializer->get_u32("version");
		Component_Deserializer cd(std::move(deserializer), version, data->handle_map);
		data->pool->load(cd);
	}
	else
	{
		log(Formatted_String<128>().sprintf("Could not open component file '%s'", data->filename.c_str()));
	}
}

std::vector<Handle> Scene::load(const char * const filename)
{
	std::vector<Handle> loaded_entity_ids;
	auto deserializer = File_Deserializer_Factory::instance().create(filename);
	if (deserializer == nullptr)
	{
		log(Formatted_String<128>().sprintf("Could not open Scene file '%s'", filename));
	}
	else if (!deserializer->is_valid())
	{
		log(Formatted_String<128>().sprintf("Failed to parse Scene file '%s'", filename));
	}
	else
	{
		auto handle_map = std::make_shared<std::unordered_map<u32, Handle>>();
		const std::string id_filename = deserializer->get_string("idFile");
		auto id_file_deserializer = File_Deserializer_Factory::instance().create(id_filename.c_str());
		if (id_file_deserializer != nullptr && id_file_deserializer->is_valid())
		{
			while (id_file_deserializer->is_valid())
			{
				const u32 originalId = id_file_deserializer->get_u32("id");
				const Handle handle = handle_manager.create();
				(*handle_map)[originalId] = handle;
				loaded_entity_ids.emplace_back(handle);
			}

			while (deserializer->is_valid())
			{
				const Tag component_tag = Tag(deserializer->get_string("cpnt"));
				Component_Pool * pool = get_pool(component_tag);
				if (pool != nullptr)
				{
					const std::string pool_filename = deserializer->get_string("file");

					load_pool_data * task_data = new load_pool_data{ pool, pool_filename, handle_map };
					add_task(load_pool, task_data, pool->metadata->thread, task_type::read);
				}
				else
					log(Formatted_String<256>().sprintf("Error loading '%s': no component pool for Tag '%s'", id_filename.c_str(), component_tag.as_string().c_str()));
			}
		}
		else
			log(Formatted_String<128>().sprintf("Could not open Scene id file '%s'", id_filename.c_str()));
	}
	log(Formatted_String<32>().sprintf("loaded %d entities", loaded_entity_ids.size()));
	return loaded_entity_ids;
}

// ----------------------------------------------------------------------------

void Scene::save(const char * filename) const
{
	save(filename, std::move(get_entity_ids()));
}

// ----------------------------------------------------------------------------

struct save_pool_task_data
{
	const Component_Pool * pool;
	std::string filename;
	std::shared_ptr<Scene_File_Saver> scene_file_saver;
};

static void save_pool_task(void * mem)
{
	auto data = make_unique<save_pool_task_data>(mem);
	auto pool_serializer = File_Serializer_Factory::instance().create(data->filename.c_str());
	if (pool_serializer != nullptr)
	{
		const u32 count = data->pool->save(*pool_serializer, data->scene_file_saver->entity_ids);
		data->scene_file_saver->on_pool_saved(data->pool->metadata->tag, data->filename, count);
		if (count == 0)
		{
			pool_serializer.reset();
			std::remove(data->filename.c_str());
		}
	}
	else
		log(Formatted_String<128>().sprintf("Could not save component pool file '%s'", data->filename.c_str()));
}

void Scene::save(const char * const filename, std::vector<Handle> entity_ids) const
{
	log(Formatted_String<32>().sprintf("saving %d entities", entity_ids.size()));
	auto serializer = File_Serializer_Factory::instance().create(filename);
	if (serializer == nullptr)
		log(Formatted_String<128>().sprintf("Could not save Scene file '%s'", filename));
	else
	{
		std::string id_filename = filename;
		String_Utilities::replace(id_filename, ".", ".hndl.");
		serializer->write_string("idFile", id_filename.c_str());
		auto id_file_serializer = File_Serializer_Factory::instance().create(id_filename.c_str());
		if (id_file_serializer != nullptr)
		{
			for (const auto & entity_id : entity_ids)
				id_file_serializer->write_u32("id", entity_id.id);
			id_file_serializer.reset();

			std::shared_ptr<Scene_File_Saver> scene_file_saver = std::make_shared<Scene_File_Saver>(std::move(serializer));
			scene_file_saver->entity_ids = std::move(entity_ids);
			for (const auto & pool : pools)
			{
				std::string pool_filename = filename;
				String_Utilities::replace(pool_filename, ".", std::string(".") + pool.metadata->tag.as_string() + ".");

				add_task(save_pool_task, new save_pool_task_data{ &pool, pool_filename, scene_file_saver }, pool.metadata->thread, task_type::read);
			}
		}
		else
			log(Formatted_String<128>().sprintf("Could not open Scene id file '%s'", id_filename.c_str()));
	}
}

// ----------------------------------------------------------------------------

Handle Scene::create_entity()
{
	return handle_manager.create();
}

// ----------------------------------------------------------------------------

bool Scene::exists(const Handle entity_id) const
{
	return handle_manager.used(entity_id);
}

// ----------------------------------------------------------------------------

std::vector<Handle> Scene::get_entity_ids() const
{
	return handle_manager.get_used();
}

// ----------------------------------------------------------------------------

u16 Scene::get_entity_version(const u16 id) const
{
	return handle_manager.get_version(id);
}

// ----------------------------------------------------------------------------

struct remove_from_pool_data
{
	Component_Pool * pool;
	Handle entity_id;
};

static void remove_from_pool(void * mem)
{
	auto data = make_unique<remove_from_pool_data>(mem);
	data->pool->remove(data->entity_id);
}

void Scene::remove_all(const Handle entity_id)
{
	for (auto & pool : pools)
		add_task(remove_from_pool, new remove_from_pool_data{ &pool, entity_id }, pool.metadata->thread, task_type::del);

	handle_manager.free(entity_id);
}

// ----------------------------------------------------------------------------

struct remove_all_data
{
	Scene * scene;
	Handle entity_id;
};

static void remove_all_internal(void * mem)
{
	auto data = make_unique<remove_all_data>(mem);
	data->scene->remove_all(data->entity_id);
}

void Scene::remove_all(const Handle entity_id) thread_const
{
	// todo: assign Scene tasks to Thread
	add_task(remove_all_internal, new remove_all_data{ const_cast<Scene*>(this), entity_id }, 0, task_type::del);
}

// ----------------------------------------------------------------------------

void Scene::clear()
{
	for (auto & pool : pools)
		pool.clear();
	handle_manager.free_all();
}

// ============================================================================

Component_Pool * Scene::get_pool(const Tag component_tag)
{
	auto it = std::find_if(pools.begin(), pools.end(), [=](Component_Pool & pool) { return pool.metadata->tag == component_tag; });
	return it == pools.end() ? nullptr : &*it;
}

// ----------------------------------------------------------------------------

const Component_Pool * Scene::get_pool(const Tag component_tag) const
{
	auto it = std::find_if(pools.cbegin(), pools.cend(), [=](const Component_Pool & pool) { return pool.metadata->tag == component_tag; });
	return it == pools.cend() ? nullptr : &*it;
}

// ============================================================================
