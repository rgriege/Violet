#ifndef __VIOLET_COMPONENT_LISTENER_H__
#define __VIOLET_COMPONENT_LISTENER_H__

#include "violet/core/Config.h"
#include "violet/core/component/Component.h"

namespace Violet
{
	/**
	 * An event subscriber interface for Component creation and destruction.
	 */
	template<class T>
	class VIOLET_API ComponentListener
	{
	public:
		/** Event handler for component creation events */
        virtual void onComponentCreated(T* component) {};
		/** Event handler for component destruction events */
        virtual void onComponentDestroyed(T* component) {};
	};
}

#endif