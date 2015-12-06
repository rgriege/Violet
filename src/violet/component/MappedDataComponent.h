#ifndef VIOLET_MappedDataComponent_H
#define VIOLET_MappedDataComponent_H

#include "violet/component/Component.h"
#include "violet/task/Thread.h"

#include <map>

namespace Violet
{
	class Deserializer;

	class VIOLET_API MappedDataComponent : public ComponentBase<MappedDataComponent>
	{
	public:

		static Tag getStaticTag();
		static Thread getStaticThread();

	public:

		MappedDataComponent(Handle entityId);
		MappedDataComponent(Handle entityId, Deserializer & deserializer);
		MappedDataComponent(MappedDataComponent && other);

	public:

		std::map<std::string, std::string> m_data;
	};

	VIOLET_API Serializer & operator<<(Serializer & serializer, const MappedDataComponent & component);
}

#endif