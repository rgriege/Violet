#ifndef VIOLET_BINARY_DESERIALIZER_H
#define VIOLET_BINARY_DESERIALIZER_H

#include "violet/serialization/deserializer.h"

#include <istream>

namespace vlt
{
	struct VIOLET_API binary_deserializer final : public deserializer
	{
		std::istream & stream;
		std::string string_buffer;

		static void install();

		binary_deserializer(std::istream & stream);

		virtual bool is_valid() const override;

		virtual std::unique_ptr<deserializer> enter_segment(const char * label) override;
		virtual const char * next_label() const override;
		
		virtual bool get_b8(const char * label) override;
		virtual u32 get_u32(const char * label) override;
		virtual s32 get_s32(const char * label) override;
		virtual r32 get_r32(const char * label) override;
		virtual r64 get_r64(const char * label) override;
		virtual const char * get_string(const char * label) override;
	};
}

#endif
