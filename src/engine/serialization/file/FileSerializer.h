#ifndef FILE_SERIALIZER_H
#define FILE_SERIALIZER_H

#include "engine/serialization/Serializer.h"

namespace Violet
{
	template <class StreamSerializer>
	class FileSerializer : public Serializer
	{
	public:

		FileSerializer(std::filebuf && file);
		virtual ~FileSerializer() override;

		virtual unique_val<Serializer> createSegment(const char * label) override;

		virtual void writeBoolean(const char * label, bool value) override;
		virtual void writeUint(const char * label, uint32 value) override;
		virtual void writeInt(const char * label, int value) override;
		virtual void writeFloat(const char * label, float value) override;
		virtual void writeDouble(const char * label, double value) override;
		virtual void writeString(const char * label, const char * value) override;

	private:

		FileSerializer(const FileSerializer &) = delete;

	private:

		std::filebuf m_file;
		std::ostream m_stream;
		StreamSerializer m_serializer;
	};
}

#include "engine/serialization/file/FileSerializer.inl"

#endif