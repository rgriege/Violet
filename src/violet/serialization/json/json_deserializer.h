#ifndef VIOLET_JsonDeserializer_H
#define VIOLET_JsonDeserializer_H

#include <istream>
#include <json/json.h>

#include "violet/serialization/deserializer.h"

namespace vlt
{
	struct VIOLET_API Json_Deserializer final : public Deserializer
	{
		Json::Value root;
		Json::ValueConstIterator position;

		static void install();

		Json_Deserializer(std::istream & stream);
		Json_Deserializer(Json_Deserializer && other);
		virtual ~Json_Deserializer() override = default;

		virtual bool is_valid() const override;

		virtual std::unique_ptr<Deserializer> enter_segment(const char * label) override;
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
