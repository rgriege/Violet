#ifndef VIOLET_ENGINE_H
#define VIOLET_ENGINE_H

#include "violet/core/defines.h"
#include "violet/event/event_context_owner.h"
#include "violet/system/system.h"
#include "violet/task/thread.h"

#include <memory>
#include <vector>

namespace vlt
{
	struct Scene;
	struct System_Factory;

	struct VIOLET_API Engine final : public Event_Context_Owner
	{
		static bool bootstrap(const System_Factory & factory, const char * configFileName);

		static Engine & instance();

	public:

		~Engine();

		void switch_scene(const char * filename);
		Scene & get_current_scene();
		const Scene & get_current_scene() const;
		void stop();

		void add_system(std::unique_ptr<System> && system);
		template <typename SystemType>
		const std::unique_ptr<SystemType> & get_system();
		template <typename SystemType>
		const std::unique_ptr<const SystemType> & get_system() const;

	private:

		Engine();
		Engine(const Engine &) = delete;
		Engine & operator=(const Engine &) = delete;

		void begin();
		void run_frame(r32 frameTime);

	private:

		std::vector<std::unique_ptr<System>> systems;
		std::unique_ptr<Scene> scene;
		std::string next_scene_filename;
		bool running;
	};
}

#include "violet/core/engine.inl"

#endif
