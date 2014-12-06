#ifndef FILE_DESERIALIZER_H
#define FILE_DESERIALIZER_H

#include "violet/core/serialization/Deserializer.h"

#include <memory>
#include <fstream>

namespace Violet
{
	template <class StreamDeserializer>
	class FileDeserializer : public Deserializer
	{
	public:

		static std::unique_ptr<FileDeserializer<StreamDeserializer>> create(const char * filename);

	public:
		
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

	public:

		FileDeserializer(std::filebuf && file);
		FileDeserializer(const FileDeserializer &) = delete;

	private:

		std::filebuf m_file;
		std::istream m_stream;
		StreamDeserializer m_deserializer;
	};
}

#include "violet/core/serialization/FileDeserializer.ipp"

#endif