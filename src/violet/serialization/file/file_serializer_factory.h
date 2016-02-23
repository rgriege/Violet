#ifndef VIOLET_FILE_SERIALIZER_FACTORY_H
#define VIOLET_FILE_SERIALIZER_FACTORY_H

#include "violet/serialization/file/file_serializer.h"
#include "violet/utility/factory.h"

#include <fstream>
#include <istream>
#include <memory>

namespace vlt
{
	struct VIOLET_API File_Serializer_Factory
	{
		static File_Serializer_Factory & instance();

	private:

		template <typename StreamSerializer>
		static std::unique_ptr<Serializer> produce(std::filebuf && file);

	public:

		template <typename T>
		void assign(const char * extension);
		std::unique_ptr<Serializer> create(const char * filename);
		void remove(const char * extension);

	private:

		File_Serializer_Factory();

		File_Serializer_Factory(const File_Serializer_Factory &) = delete;
		File_Serializer_Factory & operator=(const File_Serializer_Factory &) = delete;

	private:

		Factory<const char *, std::unique_ptr<Serializer>(std::filebuf &&)> m_factory;
	};

	template <typename StreamSerializer>
	std::unique_ptr<Serializer> File_Serializer_Factory::produce(std::filebuf && file)
	{
		return std::unique_ptr<Serializer>(new File_Serializer<StreamSerializer>(std::move(file)));
	}

	template <typename T>
	void File_Serializer_Factory::assign(const char * extension)
	{
		m_factory.assign(extension, &produce<T>);
	}
}

#endif
