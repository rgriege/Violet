#ifndef VIOLET_SCRIPT_H
#define VIOLET_SCRIPT_H

#include "violet/core/defines.h"
#include "violet/core/engine.h"
#include "violet/utility/delegate.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace vlt
{
	struct VIOLET_API script
	{
		template <typename Derived, typename Signature>
		struct method;

		template <typename Derived, typename ResultType, typename ... Args>
		struct method<Derived, ResultType(Args...)>
		{
		public:

			typedef delegate<ResultType(Args...)> Handler;

		public:

			static void assign(script & script, const Handler & func);
			static void assign(const script & script, const Handler & func);
			static bool has(const script & script);
			static ResultType run(const script & script, Args && ... args);
			static void remove(script & script);
			static void remove(const script & script);

		private:

			method() = default;
		};

		static void add_hook(script & script, u32 id, const delegate_store & func);
		static void add_hook(const script & script, u32 id, const delegate_store & func);
		static void remove_hook(script & script, u32 id, const char * name);
		static void remove_hook(const script & script, u32 id, const char * name);

	public:

		script() = default;
		virtual ~script();

		virtual std::string const & get_filename() const = 0;
		virtual bool is_valid() const = 0;
		virtual void reload() = 0;
		
	protected:

		void warn(const char * procedureName, const char * context) const;

	private:

		script(const script &) = delete;
		script & operator=(const script &) = delete;

	private:

		std::unordered_map<u32, delegate_store> m_boundMethods;
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

#define DEFINE_METHOD(MethodName, Signature) struct SCRIPT_API MethodName : public vlt::script::method<MethodName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #MethodName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}

#define DEFINE_EXTERNAL_METHOD(MethodName, Signature) struct MethodName : public vlt::script::method<MethodName, Signature> \
	{ \
	public: \
		static const char * get_name() { return #MethodName; } \
		static u32 get_identifier() { return std::hash<std::string>()(get_name()); } \
	}
}

#include "violet/script/script.inl"

#endif
