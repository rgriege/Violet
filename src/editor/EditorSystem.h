#ifndef VIOLET_EditorSystem_H
#define VIOLET_EditorSystem_H

#include "violet/system/System.h"

#include "editor/EditorConfig.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class Scene;
	class SystemFactory;

	class VIOLET_EDITOR_API EditorSystem : public System
	{
	public:

		static const char * getStaticLabel();
		static void install(SystemFactory & factory);
		static std::unique_ptr<System> init(Deserializer & deserializer);

	public:

		virtual ~EditorSystem() override = default;
		virtual void update(float dt, const Engine & engine) override;

		void loadScene(const char * filename, const Engine & engine) const;

		const std::unique_ptr<Scene> & getScene() const;

	private:

		EditorSystem();
		
	private:

		mutable std::unique_ptr<Scene> m_scene;
	};
}

#endif