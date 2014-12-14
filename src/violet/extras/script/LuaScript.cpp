#include "violet/extras/script/LuaScript.h"

#include "violet/core/Defines.h"
#include "violet/core/entity/Entity.h"
#include "violet/core/script/ScriptFactory.h"
#include "violet/core/utility/Assert.h"
#include "violet/extras/script/LuaLibrary.h"

#include <iostream>

using namespace Violet;

namespace LuaScriptNamespace
{
	class BlockStreamReader
	{
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

	const char * readChunk(lua_State * lua, void * data, size_t * size);

	std::unique_ptr<Script> create(std::istream & stream);

	int getId(lua_State * lua)
	{
		assert_equal(lua_gettop(lua), 1);
		const Entity ** p = static_cast<const Entity **>(luaL_checkudata(lua, 1, "Entity"));
		lua_pushnumber(lua, (*p)->m_id);
		return 1;
	}
}

using namespace LuaScriptNamespace;

void LuaScript::install()
{
	ScriptFactory::getInstance().assign("lua", &create);
}

LuaScript::LuaScript(std::istream & stream) :
	m_lua(luaL_newstate()),
	m_valid()
{
	luaL_openlibs(m_lua);

	luaL_newmetatable(m_lua, "Entity");
	lua_pushvalue(m_lua, -1);
	lua_setfield(m_lua, -2, "__index");
	lua_pushcfunction(m_lua, getId);
	lua_setfield(m_lua, -2, "getId");

	auto data = std::make_unique<BlockStreamReader>(stream, 512);
	m_valid = lua_load(m_lua, readChunk, data.get(), "lua file", "bt") == 0;
}

LuaScript::~LuaScript()
{
	lua_close(m_lua);
}

CreateLuaWrapper(Entity);

void LuaScript::run(const Entity & entity) const
{
	lua_pushudata(m_lua, entity);
	lua_setglobal(m_lua, "entity");

	lua_pcall(m_lua, 0, 0, 0);
}

LuaScriptNamespace::BlockStreamReader::BlockStreamReader(std::istream & stream, uint32 bufferSize) :
	m_stream(stream),
	m_bufferSize(bufferSize),
	m_buffer(new char[bufferSize])
{
}

LuaScriptNamespace::BlockStreamReader::~BlockStreamReader()
{
	delete[] m_buffer;
}

const char * LuaScriptNamespace::BlockStreamReader::read(size_t & size)
{
	if (!m_stream)
		return nullptr;

	m_stream.read(m_buffer, m_bufferSize - 1);
	size = static_cast<uint32>(m_stream.gcount());
	m_buffer[size] = 0;
	return m_buffer;
}

const char * LuaScriptNamespace::readChunk(lua_State * /*lua*/, void * data, size_t * size)
{
	auto reader = static_cast<BlockStreamReader *>(data);
	return reader->read(*size);
}

std::unique_ptr<Script> LuaScriptNamespace::create(std::istream & stream)
{
	return std::unique_ptr<Script>(new LuaScript(stream));
}