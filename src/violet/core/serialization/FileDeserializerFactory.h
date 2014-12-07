#ifndef FILE_DESERIALIZER_H
#define FILE_DESERIALIZER_H

#include "violet/core/utility/Factory.h"

#include <istream>
#include <memory>

namespace Violet
{
	class Deserializer;

	class FileDeserializerFactory : public Factory<const char *, std::unique_ptr<Deserializer>(std::istream &)>
	{
	public:

		static FileDeserializerFactory & getInstance();

	public:

		std::unique_ptr<Deserializer> create(const char * filename);

	private:

		FileDeserializerFactory();
		FileDeserializerFactory(const FileDeserializerFactory &) = delete;
		FileDeserializerFactory & operator=(const FileDeserializerFactory &) = delete;
	};
}

#endif