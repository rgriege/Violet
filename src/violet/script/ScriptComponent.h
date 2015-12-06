#ifndef VIOLET_ScriptComponent_H
#define VIOLET_ScriptComponent_H

#include "violet/component/Component.h"
#include "violet/task/Thread.h"

#include "violet/script/Script.h"

#include <memory>

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API ScriptComponent : public ComponentBase<ScriptComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		ScriptComponent(Handle entityId, Deserializer & deserializer);
		ScriptComponent(Handle entityId, const char * fileName);
		ScriptComponent(ScriptComponent && other);
		virtual ~ScriptComponent() override;

	public:

		std::unique_ptr<Script> m_script;
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const ScriptComponent & component);

	DEFINE_METHOD(BindToComponentMethod, void(Handle entityId));
	DEFINE_METHOD(UnbindFromComponentMethod, void(Handle entityId));
}

#endif