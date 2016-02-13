#ifndef VIOLET_FILE_DESERIALIZER_H
#define VIOLET_FILE_DESERIALIZER_H

#include "violet/serialization/deserializer.h"

#include <fstream>
#include <streambuf>

namespace vlt
{
	template <typename StreamDeserializer>
	struct file_deserializer final : public deserializer
	{
		std::filebuf file;
		std::istream stream;
		StreamDeserializer inner_deserializer;
		
		file_deserializer(std::filebuf && file);
		file_deserializer(const file_deserializer &) = delete;
		virtual ~file_deserializer() override;

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

#include "violet/serialization/file/file_deserializer.inl"

#endif
