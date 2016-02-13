#ifndef VIOLET_JsonDeserializer_H
#define VIOLET_JsonDeserializer_H

#include <istream>
#include <json/json.h>

#include "violet/serialization/deserializer.h"

namespace vlt
{
	struct VIOLET_API json_deserializer final : public deserializer
	{
		Json::Value root;
		Json::ValueConstIterator position;

		static void install();

		json_deserializer(std::istream & stream);
		json_deserializer(json_deserializer && other);
		virtual ~json_deserializer() override = default;

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