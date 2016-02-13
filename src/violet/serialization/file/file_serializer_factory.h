#ifndef VIOLET_FILE_SERIALIZER_FACTORY_H
#define VIOLET_FILE_SERIALIZER_FACTORY_H

#include "violet/serialization/file/file_serializer.h"
#include "violet/utility/factory.h"

#include <fstream>
#include <istream>
#include <memory>

namespace vlt
{
	struct VIOLET_API file_serializer_factory
	{
		static file_serializer_factory & instance();

	private:

		template <typename StreamSerializer>
		static std::unique_ptr<serializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<serializer> create(const char * filename);
		void remove(const char * extension);

	private:

		file_serializer_factory();

		file_serializer_factory(const file_serializer_factory &) = delete;
		file_serializer_factory & operator=(const file_serializer_factory &) = delete;

	private:

		factory<const char *, std::unique_ptr<serializer>(std::filebuf &&)> m_factory;
	};

	template <typename StreamSerializer>
	std::unique_ptr<serializer> file_serializer_factory::produce(std::filebuf && file)
	{
		return std::unique_ptr<serializer>(new file_serializer<StreamSerializer>(std::move(file)));
	}

	template <typename T>
	void file_serializer_factory::assign(const char * extension)
	{
		m_factory.assign(extension, &produce<T>);
	}
}

#endif