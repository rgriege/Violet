#ifndef VIOLET_FILE_SERIALIZER_H
#define VIOLET_FILE_SERIALIZER_H

#include "violet/serialization/serializer.h"

#include <fstream>
#include <streambuf>

namespace vlt
{
	template <typename StreamSerializer>
	struct file_serializer final : public serializer
	{
		std::filebuf file;
		std::ostream stream;
		StreamSerializer stream_serializer;

		file_serializer(std::filebuf && file);
		file_serializer(const file_serializer &) = delete;
		virtual ~file_serializer() override;

		virtual unique_val<serializer> create_segment(const char * label) override;

		virtual void write_b8(const char * label, bool value) override;
		virtual void write_u32(const char * label, u32 value) override;
		virtual void write_s32(const char * label, int value) override;
		virtual void write_r32(const char * label, r32 value) override;
		virtual void write_r64(const char * label, r64 value) override;
		virtual void write_string(const char * label, const char * value) override;
	};
}

#include "violet/serialization/file/file_serializer.inl"

#endif
