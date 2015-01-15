#ifndef LUA_LIBRARY_H
#define LUA_LIBRARY_H

#include <lua.hpp>
#include <vector>

namespace Violet
{
	template <typename T>
	struct LuaWrapper
	{
		static const char * name;
	};

	#define CreateLuaWrapper(Type) \
		template <> \
		struct LuaWrapper<Type> \
				{ \
			static const char * name; \
				}; \
		const char * LuaWrapper<Type>::name = #Type;

	template <typename T>
	void luaV_pushudata(lua_State * lua, const T & udata)
	{
		const T ** p = static_cast<const T **>(lua_newuserdata(lua, sizeof(T *)));
		*p = &udata;
		luaL_setmetatable(lua, LuaWrapper<T>::name);
	}

	/*template <typename T>
	void luaV_push(lua_State * lua, const T & value)
	{
		luaV_pushudata(lua, value);
	}

	template <>
	void luaV_push(lua_State * lua, const int & value)
	{
		lua_pushinteger(lua, value);
	}

	class LuaClassBase
	{
	public:

		virtual void install(lua_State * lua) = 0;
	};

	template <typename T>
	class LuaClass : public LuaClassBase
	{
	public:

		struct MethodBase
		{
			virtual int eval(lua_State * lua) = 0;
		};

		template <typename Signature>
		struct Method;

		template <typename ReturnType, typename ... Args>
		struct Method<ReturnType(Args & ...)> : public MethodBase
		{
			Method(const char * name, ReturnType(T::*func)(Args & ...)) :
				m_name(name),
				m_func(func)
			{
			}

			Method(const char * name, ReturnType T::* member) :
				m_name(name)
			{
			}

			virtual int eval(lua_State * lua) override
			{
				assert_equal(lua_gettop(lua), 1);
				T ** p = static_cast<T **>(luaL_checkudata(lua, 1, LuaWrapper<T>::name));
				const ReturnType r = ((*p)->*m_func)();
				luaV_push(lua, r);
				return 1;
			}

		private:

			const char * m_name;
			(ReturnType)(T::*m_func)(Args & ...);
		};

		template <typename MemberType>
		struct Member : public MethodBase
		{
		public:

			Member(const char * name, MemberType T::*member) : 
				m_name(name),
				m_member(member)
			{
			}

			virtual int eval(lua_State * lua) override
			{
				assert_equal(lua_gettop(lua), 1);
				T ** p = static_cast<T **>(luaL_checkudata(lua, 1, LuaWrapper<T>::name));
				luaV_push(lua, (*p)->*m_member);
				return 1;
			}

		private:

			const char * m_name;
			MemberType T::*m_member;
		};

	public:

		LuaClass & addMethod(MethodBase && method);
		
		virtual void install(lua_State * lua) override
		{
			luaL_newmetatable(lua, LuaWrapper<T>::name);
			lua_pushvalue(lua, -1);
			lua_setfield(lua, -2, "__index");

			for (auto const & method : m_methods)
			{

			}
		}

	public:

		LuaClass(const char * name) :
			m_name(name)
		{
		}

	private:

		const char * m_name;
		std::vector<MethodBase> m_methods;
	};

	class LuaLibrary
	{
	public:

		static LuaLibrary & getInstance()
		{
			static LuaLibrary ms_instance;
			return ms_instance;
		}

	public:

		LuaLibrary & addClass(LuaClassBase && classDef)
		{
			m_classes.emplace_back(classDef);
			return *this;
		}

		void open(lua_State * lua)
		{
			for (auto const & classDef : m_classes)
				classDef->install(lua);
		}

	private:

		std::vector<LuaClassBase *> m_classes;
	};*/
}

#endif