#ifndef VIOLET_FileDeserializerFactory_H
#define VIOLET_FileDeserializerFactory_H

#include "violet/serialization/file/file_deserializer.h"
#include "violet/utility/factory.h"

#include <fstream>
#include <istream>
#include <memory>

namespace vlt
{
	struct VIOLET_API File_Deserializer_Factory
	{
		static File_Deserializer_Factory & instance();

	private:

		template <typename StreamDeserializer>
		static std::unique_ptr<Deserializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<Deserializer> create(const char * filename);
		void remove(const char * extension);

	private:

		File_Deserializer_Factory();

		File_Deserializer_Factory(const File_Deserializer_Factory &) = delete;
		File_Deserializer_Factory & operator=(const File_Deserializer_Factory &) = delete;

	private:

		Factory<const char *, std::unique_ptr<Deserializer>(std::filebuf &&)> m_factory;
	};
}

#include "violet/serialization/file/file_deserializer_factory.inl"

#endif
