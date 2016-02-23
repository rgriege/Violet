#ifndef VIOLET_DESERIALIZER_H
#define VIOLET_DESERIALIZER_H

#include <memory>

#include "violet/core/defines.h"
#include "violet/core/types.h"

namespace vlt
{
	struct VIOLET_API Deserializer
	{
		Deserializer() = default;
		Deserializer(const Deserializer &) = delete;
		Deserializer & operator=(const Deserializer &) = delete;
		virtual ~Deserializer() = default;

		virtual b8 is_valid() const = 0;

		virtual std::unique_ptr<Deserializer> enter_segment(const char * label) = 0;
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
