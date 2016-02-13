#ifndef VIOLET_DESERIALIZER_H
#define VIOLET_DESERIALIZER_H

#include "violet/core/defines.h"
#include "violet/core/types.h"

#include <memory>

namespace vlt
{
	struct VIOLET_API deserializer
	{
		deserializer() = default;
		deserializer(const deserializer &) = delete;
		deserializer & operator=(const deserializer &) = delete;
		virtual ~deserializer() = default;

		virtual b8 is_valid() const = 0;

		virtual std::unique_ptr<deserializer> enter_segment(const char * label) = 0;
		virtual const char * next_label() const = 0;

		virtual b8 get_b8(const char * label) = 0;
		virtual u32 get_u32(const char * label) = 0;
		virtual s32 get_s32(const char * label) = 0;
		virtual r32 get_r32(const char * label) = 0;
		virtual r64 get_r64(const char * label) = 0;
		virtual const char * get_string(const char * label) = 0;
	};
}

#endif
