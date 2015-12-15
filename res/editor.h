#include "violet/component/Component.h"
#include "violet/task/Thread.h"

using namespace Violet;

template <typename ComponentType>
class EditorComponentWrapper : public ComponentBase<EditorComponentWrapper<ComponentType>>
{
public:

    static Tag getStaticTag()
    {
        Tag s_tag = Tag((std::string("e") + ComponentType::getStaticTag().asString()).c_str());
        return s_tag;
    }

    static Thread getStaticThread()
    {
        return ComponentType::getStaticThread();
    }

public:

    EditorComponentWrapper(const EntityId entityId, ComponentDeserializer & deserializer) :
        ComponentBase<EditorComponentWrapper<ComponentType>>(entityId),
        m_data(entityId, deserializer)
    {
    }

    EditorComponentWrapper(EditorComponentWrapper && other) :
        ComponentBase<EditorComponentWrapper<ComponentType>>(std::move(other)),
        m_data(std::move(other.m_data))
    {
    }

	EditorComponentWrapper(const EntityId entityId, ComponentType && other) :
        ComponentBase<EditorComponentWrapper<ComponentType>>(entityId),
        m_data(std::move(other))
    {
    }

    ComponentType m_data;
};

template <typename ComponentType>
Serializer & operator<<(Serializer & serializer, const EditorComponentWrapper<ComponentType> & component)
{
    return serializer << component.m_data;
}

