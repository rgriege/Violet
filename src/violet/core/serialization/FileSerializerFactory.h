#ifndef FILE_SERIALIZER_FACTORY_H
#define FILE_SERIALIZER_FACTORY_H

#include "violet/core/Defines.h"
#include "violet/core/serialization/FileSerializer.h"
#include "violet/core/utility/Factory.h"

#include <fstream>
#include <istream>
#include <memory>

namespace Violet
{
	class FileSerializerFactory
	{
	public:

		static VIOLET_API FileSerializerFactory & getInstance();

	private:

		template <typename StreamSerializer>
		static std::unique_ptr<Serializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<Serializer> create(const char * filename);
		void remove(const char * extension);

	private:

		FileSerializerFactory();

		FileSerializerFactory(const FileSerializerFactory &) = delete;
		FileSerializerFactory & operator=(const FileSerializerFactory &) = delete;

	private:

		Factory<const char *, std::unique_ptr<Serializer>(std::filebuf &&)> m_factory;
	};

	template <typename StreamSerializer>
	std::unique_ptr<Serializer> FileSerializerFactory::produce(std::filebuf && file)
	{
		return std::unique_ptr<Serializer>(new FileSerializer<StreamSerializer>(std::move(file)));
	}

	template <typename T>
	void FileSerializerFactory::assign(const char * extension)
	{
		m_factory.assign(extension, &produce<T>);
	}
}

#endif