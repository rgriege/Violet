#ifndef VIOLET_COMPONENT_H
#define VIOLET_COMPONENT_H

#include "violet/core/defines.h"
#include "violet/core/tag.h"
#include "violet/core/types.h"

#include <utility>

namespace vlt
{
	struct serializer;

	struct VIOLET_API component
	{
		template <typename ComponentType>
		static u32 gather_flags();
		template <typename ComponentType, typename OtherComponentType, typename ... ComponentTypes>
		static u32 gather_flags();

	protected:

		template <typename Derived>
		static u32 get_flag();

	private:

		static u32 get_next_flag();
	};

	template <typename Derived, u32 Version>
	struct component_base : public component
	{
		static u32 get_flag();
		static u32 get_version();
	};
}

#include "violet/component/component.inl"

#endif
