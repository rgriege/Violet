#ifndef VIOLET_COMPONENT_DESERIALIZER_H
#define VIOLET_COMPONENT_DESERIALIZER_H

#include <unordered_map>

#include "violet/core/handle.h"
#include "violet/serialization/deserializer.h"

namespace vlt
{
	struct VIOLET_API Component_Deserializer final : public Deserializer
	{
		std::unique_ptr<Deserializer> inner_deserializer;
		const u32 version;
		const std::shared_ptr<const std::unordered_map<u32, Handle>> handle_id_map;

		Component_Deserializer(std::unique_ptr<Deserializer> && deserializer, u32 version, std::shared_ptr<const std::unordered_map<u32, Handle>> handle_id_map);
		Component_Deserializer(const Component_Deserializer &) = delete;
		virtual ~Component_Deserializer() override = default;

		virtual b8 is_valid() const override;

		virtual std::unique_ptr<Deserializer> enter_segment(const char * label) override;
		virtual const char * next_label() const override;

		virtual b8 get_b8(const char * label) override;
		virtual u32 get_u32(const char * label) override;
		virtual s32 get_s32(const char * label) override;
		virtual r32 get_r32(const char * label) override;
		virtual r64 get_r64(const char * label) override;
		virtual const char * get_string(const char * label) override;

		u32 get_version() const;
		Handle get_entity_id(const char * label) const;
	};
}

#endif
