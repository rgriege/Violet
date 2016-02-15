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
	struct scene;
	struct system_factory;

	struct VIOLET_API engine : public event_context_owner
	{
		static bool bootstrap(const system_factory & factory, const char * configFileName);

		static engine & instance();

	public:

		~engine();

		void switch_scene(const char * filename);
		scene & get_current_scene();
		const scene & get_current_scene() const;
		void stop();

		void add_system(std::unique_ptr<system> && s);
		template <typename SystemType>
		const std::unique_ptr<SystemType> & get_system();
		template <typename SystemType>
		const std::unique_ptr<const SystemType> & get_system() const;

	private:

		engine();
		engine(const engine &) = delete;
		engine & operator=(const engine &) = delete;

		void begin();
		void run_frame(r32 frameTime);

	private:

		std::vector<std::unique_ptr<system>> m_systems;
		std::unique_ptr<scene> m_scene;
		std::string m_nextSceneFileName;
		bool m_running;
	};
}

#include "violet/core/engine.inl"

#endif
