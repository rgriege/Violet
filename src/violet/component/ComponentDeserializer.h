#ifndef VIOLET_ComponentDeserializer_H
#define VIOLET_ComponentDeserializer_H

#include "violet/serialization/Deserializer.h"

#include <unordered_map>

namespace Violet
{
	class VIOLET_API ComponentDeserializer : public Deserializer
	{
	public:

		ComponentDeserializer(std::unique_ptr<Deserializer> && deserializer, std::shared_ptr<const std::unordered_map<uint32, EntityId>> handleIdMap);
		virtual ~ComponentDeserializer() override = default;

		virtual operator bool() const override;

		virtual std::unique_ptr<Deserializer> enterSegment(const char * label) override;
		virtual const char * nextLabel() const override;

		virtual bool getBoolean(const char * label) override;
		virtual uint32 getUint(const char * label) override;
		virtual int32 getInt(const char * label) override;
		virtual float getFloat(const char * label) override;
		virtual double getDouble(const char * label) override;
		virtual const char * getString(const char * label) override;

		EntityId getEntityId(const char * label);

	private:

		ComponentDeserializer(const ComponentDeserializer &) = delete;

	private:

		std::unique_ptr<Deserializer> m_deserializer;
		const std::shared_ptr<const std::unordered_map<uint32, EntityId>> m_handleIdMap;
	};
}

#endif