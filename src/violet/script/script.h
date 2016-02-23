#ifndef VIOLET_SCRIPT_H
#define VIOLET_SCRIPT_H

#include <string>
#include <vector>
#include <unordered_map>

#include "violet/core/defines.h"
#include "violet/core/engine.h"
#include "violet/utility/delegate.h"

namespace vlt
{
	struct VIOLET_API Script
	{
		template <typename Derived, typename Signature>
		struct Method;

		template <typename Derived, typename ResultType, typename ... Args>
		struct Method<Derived, ResultType(Args...)>
		{
		public:

			typedef Delegate<ResultType(Args...)> Handler;

		public:

			static void assign(Script & Script, const Handler & func);
			static void assign(const Script & Script, const Handler & func);
			static bool has(const Script & Script);
			static ResultType run(const Script & Script, Args && ... args);
			static void remove(Script & Script);
			static void remove(const Script & Script);

		private:

			Method() = default;
		};

		static void add_hook(Script & script, u32 id, const Delegate_Store & func);
		static void add_hook(const Script & script, u32 id, const Delegate_Store & func);
		static void remove_hook(Script & script, u32 id, const char * name);
		static void remove_hook(const Script & script, u32 id, const char * name);

	public:

		Script() = default;
		virtual ~Script();

		virtual std::string const & get_filename() const = 0;
		virtual bool is_valid() const = 0;
		virtual void reload() = 0;
		
	protected:

		void warn(const char * procedureName, const char * context) const;

	private:

		Script(const Script &) = delete;
		Script & operator=(const Script &) = delete;

	private:

		std::unordered_map<u32, Delegate_Store> m_boundMethods;
	};

#ifdef WIN32
#ifdef VIOLET_SCRIPT
#define SCRIPT_API __declspec(dllimport)
#else
#define SCRIPT_API __declspec(dllexport)
#endif
#else
#define SCRIPT_API
#endif

#define DEFINE_METHOD(MethodName, Signature) struct SCRIPT_API MethodName : public vlt::Script::Method<MethodName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #MethodName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}

#define DEFINE_EXTERNAL_METHOD(MethodName, Signature) struct MethodName : public vlt::Script::Method<MethodName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #MethodName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}
}

#include "violet/script/script.inl"

#endif
