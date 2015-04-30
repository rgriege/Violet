#ifndef FILE_DESERIALIZER_FACTORY_H
#define FILE_DESERIALIZER_FACTORY_H

#include "engine/Defines.h"
#include "engine/serialization/file/FileDeserializer.h"
#include "engine/utility/Factory.h"

#include <fstream>
#include <istream>
#include <memory>

namespace Violet
{
	class FileDeserializerFactory
	{
	public:

		static VIOLET_API FileDeserializerFactory & getInstance();

	private:

		template <typename StreamDeserializer>
		static std::unique_ptr<Deserializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<Deserializer> VIOLET_API create(const char * filename);
		void VIOLET_API remove(const char * extension);

	private:

		VIOLET_API FileDeserializerFactory();

		FileDeserializerFactory(const FileDeserializerFactory &) = delete;
		FileDeserializerFactory & operator=(const FileDeserializerFactory &) = delete;

	private:

		Factory<const char *, std::unique_ptr<Deserializer>(std::filebuf &&)> m_factory;
	};

	template <typename StreamDeserializer>
	std::unique_ptr<Deserializer> FileDeserializerFactory::produce(std::filebuf && file)
	{
		return std::unique_ptr<Deserializer>(new FileDeserializer<StreamDeserializer>(std::move(file)));
	}

	template <typename T>
	void FileDeserializerFactory::assign(const char * extension)
	{
		m_factory.assign(extension, &produce<T>);
	}
}

#endif