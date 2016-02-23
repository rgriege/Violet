#ifndef VIOLET_JSON_SERIALIZER_H
#define VIOLET_JSON_SERIALIZER_H

#include <ostream>
#include <json/json.h>

#include "violet/serialization/serializer.h"

namespace vlt
{
	struct VIOLET_API Json_Serializer final : public Serializer
	{
		std::ostream & stream;
		Json::Value value;

		static void install();

		Json_Serializer(std::ostream & stream);
		Json_Serializer(Json_Serializer && other);
		virtual ~Json_Serializer() override;

		virtual unique_val<Serializer> create_segment(const char * label) override;

		virtual void write_b8(const char * label, b8 value) override;
		virtual void write_u32(const char * label, u32 value) override;
		virtual void write_s32(const char * label, int value) override;
		virtual void write_r32(const char * label, r32 value) override;
		virtual void write_r64(const char * label, r64 value) override;
		virtual void write_string(const char * label, const char * value) override;
	};
}

#endif
