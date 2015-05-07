#ifndef VIOLET_FileDeserializer_H
#define VIOLET_FileDeserializer_H

#include "engine/serialization/Deserializer.h"

namespace Violet
{
	template <class StreamDeserializer>
	class FileDeserializer : public Deserializer
	{
	public:

		FileDeserializer(std::filebuf && file);
		virtual ~FileDeserializer() override;

		virtual operator bool() const override;

		virtual std::unique_ptr<Deserializer> enterSegment(const char * label) override;
		virtual const char * nextLabel() const override;

		virtual bool getBoolean(const char * label) override;
		virtual uint32 getUint(const char * label) override;
		virtual int32 getInt(const char * label) override;
		virtual float getFloat(const char * label) override;
		virtual double getDouble(const char * label) override;
		virtual const char * getString(const char * label) override;

	private:

		FileDeserializer(const FileDeserializer &) = delete;

	private:

		std::filebuf m_file;
		std::istream m_stream;
		StreamDeserializer m_deserializer;
	};
}

#include "engine/serialization/file/FileDeserializer.inl"

#endif