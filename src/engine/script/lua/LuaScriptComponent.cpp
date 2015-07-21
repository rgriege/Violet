// ============================================================================

#include "engine/script/lua/LuaScriptComponent.h"

#include "engine/entity/Entity.h"
#include "engine/script/lua/LuaLibrary.h"
#include "engine/serialization/Deserializer.h"

#include <assert.h>
#include <fstream>
#include <lua.hpp>

using namespace Violet;

namespace LuaScriptComponentNamespace
{
	// ----------------------------------------------------------------------------

	class BlockStreamReader
	{
	public:

		static const char * readChunk(lua_State * lua, void * data, size_t * size);

	public:

		BlockStreamReader(std::istream & stream, uint32 bufferSize);
		~BlockStreamReader();

	public:

		const char * read(size_t & size);

	private:

		std::istream & m_stream;
		uint32 const m_bufferSize;
		char * const m_buffer;
	};

	// ----------------------------------------------------------------------------
}

using namespace LuaScriptComponentNamespace;

// ============================================================================

Tag LuaScriptComponent::getTag()
{
	return Tag('l', 'u', 'a', 's');
}

// ============================================================================

LuaScriptComponent::LuaScriptComponent(const Entity & owner, Deserializer & deserializer) :
	ComponentBase<LuaScriptComponent>(owner),
	m_filename(deserializer.getString("file")),
	m_lua(),
	m_valid(false)
{
	load();
}

// ----------------------------------------------------------------------------

LuaScriptComponent::LuaScriptComponent(LuaScriptComponent && other) :
	ComponentBase<LuaScriptComponent>(std::move(other)),
	m_filename(std::move(other.m_filename)),
	m_lua(other.m_lua),
	m_valid(other.m_valid)
{
	other.m_lua = nullptr;
	other.m_valid = false;
}

// ----------------------------------------------------------------------------

LuaScriptComponent::~LuaScriptComponent()
{
	if (m_valid)
		unload();
}

// ----------------------------------------------------------------------------

std::string const & LuaScriptComponent::getFilename() const
{
	return m_filename;
}

// ----------------------------------------------------------------------------

void LuaScriptComponent::reload()
{
	if (m_valid)
		unload();
	load();
}

// ============================================================================

void LuaScriptComponent::load()
{
	m_lua = luaL_newstate();
	m_valid = false;
	
	luaL_openlibs(m_lua);
	LuaLibrary::open(m_lua);

	std::ifstream ifstream(m_filename);
	if (ifstream.is_open())
	{
		BlockStreamReader reader(ifstream, 512);
		if (lua_load(m_lua, BlockStreamReader::readChunk, &reader, m_filename.c_str(), "bt") == LUA_OK)
		{
			if (lua_pcall(m_lua, 0, 0, 0) == LUA_OK)
			{
				lua_getglobal(m_lua, "init");
				if (lua_pcall(m_lua, 0, 0, 0) == LUA_OK)
					m_valid = true;
				else
				{
					const char * error = lua_tostring(m_lua, -1);
					printf("%s\n", error);
				}
			}
			else
			{
				const char * error = lua_tostring(m_lua, -1);
				printf("%s compilation error: %s\n", m_filename.c_str(), error);
				lua_pop(m_lua, 1);
			}
		}
	}

	if (!m_valid)
	{
		lua_close(m_lua);
		m_lua = nullptr;
	}
}

// ----------------------------------------------------------------------------

void LuaScriptComponent::unload()
{
	lua_getglobal(m_lua, "clean");
	lua_pcall(m_lua, 0, 0, 0);
	lua_close(m_lua);
	m_valid = false;
}

// ============================================================================

LuaScriptComponentNamespace::BlockStreamReader::BlockStreamReader(std::istream & stream, uint32 bufferSize) :
	m_stream(stream),
	m_bufferSize(bufferSize),
	m_buffer(new char[bufferSize])
{
}

// ----------------------------------------------------------------------------

LuaScriptComponentNamespace::BlockStreamReader::~BlockStreamReader()
{
	delete[] m_buffer;
}

// ----------------------------------------------------------------------------

const char * LuaScriptComponentNamespace::BlockStreamReader::read(size_t & size)
{
	if (!m_stream)
		return nullptr;

	m_stream.read(m_buffer, m_bufferSize - 1);
	size = static_cast<uint32>(m_stream.gcount());
	m_buffer[size] = 0;
	return m_buffer;
}

// ----------------------------------------------------------------------------

const char * LuaScriptComponentNamespace::BlockStreamReader::readChunk(lua_State * /*lua*/, void * data, size_t * size)
{
	auto reader = static_cast<BlockStreamReader *>(data);
	return reader->read(*size);
}

// ============================================================================
