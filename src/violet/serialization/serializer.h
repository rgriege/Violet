#ifndef VIOLET_Serializer_H
#define VIOLET_Serializer_H

#include "violet/core/defines.h"
#include "violet/core/types.h"
#include "violet/utility/unique_val.h"

namespace vlt
{
	struct VIOLET_API Serializer
	{
		virtual ~Serializer() = default;

		virtual unique_val<Serializer> create_segment(const char * label) = 0;

		virtual void write_b8(const char * label, b8 value) = 0;
		virtual void write_u32(const char * label, u32 value) = 0;
		virtual void write_s32(const char * label, int value) = 0;
		virtual void write_r32(const char * label, r32 value) = 0;
		virtual void write_r64(const char * label, r64 value) = 0;
		virtual void write_string(const char * label, const char * value) = 0;
	};
}

#endif
