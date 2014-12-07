#include "violet/core/serialization/FileDeserializerFactory.h"

#include "violet/core/serialization/Deserializer.h"
#include "violet/core/utility/StringUtilities.h"

#include <fstream>

using namespace Violet;

namespace FileDeserializerFactoryNamespace
{
	class FileDeserializer : public Deserializer
	{
	public:

		FileDeserializer(std::filebuf && file);
		virtual ~FileDeserializer() override;

		virtual operator bool() const override;

		virtual std::unique_ptr<Deserializer> enterSegment(const char * label) override;
		virtual const char * nextLabel() const override;

		virtual bool getBoolean(const char * label) override;
		virtual uint32 getUint(const char * label) override;
		virtual int32 getInt(const char * label) override;
		virtual float getFloat(const char * label) override;
		virtual double getDouble(const char * label) override;
		virtual const char * getString(const char * label) override;

		std::istream & getStream();
		void setInner(std::unique_ptr<Deserializer> && inner);

	private:

		std::filebuf m_file;
		std::istream m_stream;
		std::unique_ptr<Deserializer> m_deserializer;
	};
}

using namespace FileDeserializerFactoryNamespace;

FileDeserializerFactory & FileDeserializerFactory::getInstance()
{
	static FileDeserializerFactory ms_instance;
	return ms_instance;
}

std::unique_ptr<Deserializer> FileDeserializerFactory::create(const char * filename)
{
	std::filebuf fb;
	if (!fb.open(filename, std::ios::in))
		return nullptr;

	auto fileDeserializer = new FileDeserializer(std::move(fb));
	fileDeserializer->setInner(Factory<const char *, std::unique_ptr<Deserializer>(std::istream &)>::create(StringUtilities::right(filename, '.'), fileDeserializer->getStream()));
	return std::unique_ptr<Deserializer>(fileDeserializer);
}

FileDeserializer::FileDeserializer(std::filebuf && file) :
	m_file(std::move(file)),
	m_stream(&m_file),
	m_deserializer()
{
}

FileDeserializerFactory::FileDeserializerFactory()
{
}

FileDeserializer::~FileDeserializer()
{
	m_file.close();
}

FileDeserializer::operator bool() const
{
	return *m_deserializer;
}

std::unique_ptr<Deserializer> FileDeserializer::enterSegment(const char * label)
{
	return m_deserializer->enterSegment(label);
}

const char * FileDeserializer::nextLabel() const
{
	return m_deserializer->nextLabel();
}

bool FileDeserializer::getBoolean(const char * label)
{
	return m_deserializer->getBoolean(label);
}

uint32 FileDeserializer::getUint(const char * label)
{
	return m_deserializer->getUint(label);
}

int32 FileDeserializer::getInt(const char * label)
{
	return m_deserializer->getInt(label);
}

float FileDeserializer::getFloat(const char * label)
{
	return m_deserializer->getFloat(label);
}

double FileDeserializer::getDouble(const char * label)
{
	return m_deserializer->getDouble(label);
}

const char * FileDeserializer::getString(const char * label)
{
	return m_deserializer->getString(label);
}


std::istream & FileDeserializer::getStream()
{
	return m_stream;
}

void FileDeserializer::setInner(std::unique_ptr<Deserializer> && inner)
{
	m_deserializer = std::move(inner);
}