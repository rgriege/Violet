#ifndef VIOLET_FileDeserializerFactory_H
#define VIOLET_FileDeserializerFactory_H

#include "violet/serialization/file/file_deserializer.h"
#include "violet/utility/factory.h"

#include <fstream>
#include <istream>
#include <memory>

namespace vlt
{
	struct VIOLET_API file_deserializer_factory
	{
		static file_deserializer_factory & instance();

	private:

		template <typename StreamDeserializer>
		static std::unique_ptr<deserializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<deserializer> create(const char * filename);
		void remove(const char * extension);

	private:

		file_deserializer_factory();

		file_deserializer_factory(const file_deserializer_factory &) = delete;
		file_deserializer_factory & operator=(const file_deserializer_factory &) = delete;

	private:

		factory<const char *, std::unique_ptr<deserializer>(std::filebuf &&)> m_factory;
	};
}

#include "violet/serialization/file/file_deserializer_factory.inl"

#endif