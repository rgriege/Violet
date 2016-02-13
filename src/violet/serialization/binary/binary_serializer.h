#ifndef VIOLET_BINARY_SERIALIZER_H
#define VIOLET_BINARY_SERIALIZER_H

#include "violet/serialization/serializer.h"

#include <ostream>

namespace vlt
{
	struct VIOLET_API binary_serializer final : public serializer
	{
		std::ostream & stream;

		static void install();

		binary_serializer(std::ostream & stream);
		virtual ~binary_serializer() override = default;

		virtual unique_val<serializer> create_segment(const char * label) override;

		virtual void write_b8(const char * label, b8 value) override;
		virtual void write_u32(const char * label, u32 value) override;
		virtual void write_s32(const char * label, int value) override;
		virtual void write_r32(const char * label, r32 value) override;
		virtual void write_r64(const char * label, r64 value) override;
		virtual void write_string(const char * label, const char * value) override;
	};
}

#endif