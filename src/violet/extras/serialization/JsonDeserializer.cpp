#include "violet/extras/serialization/JsonDeserializer.h"

#include <fstream>
#include <memory>
#include <algorithm>

using namespace Violet;

JsonDeserializer::JsonDeserializer(std::istream & stream) :
	m_root(),
	m_valueIterators()
{
	Json::Reader reader;
	if (reader.parse(stream, m_root, false))
	{
		const Json::Value * rootPtr = &m_root;
		m_valueIterators.push_front(std::make_pair(rootPtr->begin(), rootPtr->end()));
		moveToNextValue();
	}
}

JsonDeserializer::operator bool() const
{
	return !m_valueIterators.empty();
}

JsonDeserializer & JsonDeserializer::operator>>(bool & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = (*it).asBool();
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(uint8 & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = static_cast<uint8>((*it).asUInt());
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(int8 & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = static_cast<int8>((*it).asInt());
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(uint16 & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = static_cast<uint16>((*it).asUInt());
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(int16 & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = static_cast<int16>((*it).asInt());
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(uint32 & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = (*it).asUInt();
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(int32 & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = (*it).asInt();
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(float & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = static_cast<float>((*it).asDouble());
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(double & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = (*it).asDouble();
		++it;
		moveToNextValue();
	}
	return *this;
}

JsonDeserializer & JsonDeserializer::operator>>(std::string & value)
{
	if (*this)
	{
		Json::ValueConstIterator & it = m_valueIterators.front().first;
		value = (*it).asString();
		++it;
		moveToNextValue();
	}
	return *this;
}

void JsonDeserializer::moveToNextValue()
{
	bool found = false;

	while (!found && !m_valueIterators.empty())
	{
		if (m_valueIterators.front().first == m_valueIterators.front().second)
			m_valueIterators.pop_front();
		else
		{
			Json::ValueConstIterator & it = m_valueIterators.front().first;
			const Json::Value * nextValue = &*it;
			if (nextValue->isObject())
			{
				if (!nextValue->empty())
				{
					++it;
					m_valueIterators.push_front(std::make_pair(nextValue->begin(), nextValue->end()));
				}
			}
			else
				found = true;
		}
	}
}
