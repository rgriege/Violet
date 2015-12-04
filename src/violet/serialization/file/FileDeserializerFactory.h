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
	class VIOLET_API FileDeserializerFactory
	{
	public:

		static FileDeserializerFactory & getInstance();

	private:

		template <typename StreamDeserializer>
		static std::unique_ptr<Deserializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<Deserializer> create(const char * filename);
		void remove(const char * extension);

	private:

		FileDeserializerFactory();

		FileDeserializerFactory(const FileDeserializerFactory &) = delete;
		FileDeserializerFactory & operator=(const FileDeserializerFactory &) = delete;

	private:

		Factory<const char *, std::unique_ptr<Deserializer>(std::filebuf &&)> m_factory;
	};
}

#include "violet/serialization/file/FileDeserializerFactory.inl"

#endif