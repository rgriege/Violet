#ifndef VIOLET_FileDeserializerFactory_H
#define VIOLET_FileDeserializerFactory_H

#include "violet/Defines.h"
#include "violet/serialization/file/FileDeserializer.h"
#include "violet/utility/Factory.h"

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
}

#include "violet/serialization/file/FileDeserializerFactory.inl"

#endif