#ifndef VIOLET_Script_H
#define VIOLET_Script_H

#include "violet/Defines.h"
#include "violet/Engine.h"

#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

namespace Violet
{
	class VIOLET_API Script
	{
	public:

		template <typename Derived, typename Signature>
		class Method;

		template <typename Derived, typename ResultType, typename ... Args>
		class Method<Derived, ResultType(Args...)>
		{
		public:

			typedef std::function<ResultType(Args...)> Delegate;

		public:

			static void assign(Script & script, const Delegate & func);
			static void assign(const Engine & engine, const Script & script, const Delegate & func);
			static bool has(const Script & script);
			static ResultType run(const Script & script, Args && ... args);
			static void remove(Script & script);
			static void remove(const Engine & engine, const Script & script);
			
		private:

			static void removeHook(Script & script);

		private:

			Method() = default;
		};

	private:

		static void addHook(Script & script, uint32 id, void * hook);

	public:

		Script() = default;
		virtual ~Script();

		virtual std::string const & getFileName() const = 0;
		virtual bool isValid() const = 0;
		virtual void reload() = 0;
		
	protected:

		void warn(const char * procedureName, const char * context) const;

	private:

		Script(const Script &) = delete;
		Script & operator=(const Script &) = delete;

	private:

		std::unordered_map<uint32, void *> m_boundMethods;
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

#define DEFINE_METHOD(MethodName, Signature) class SCRIPT_API MethodName : public Violet::Script::Method<MethodName, Signature> \
	{ \
	public: \
		static const char * getName() { return #MethodName; } \
		static uint32 getIdentifier() { return std::hash<std::string>()(getName()); } \
	}

#define DEFINE_EXTERNAL_METHOD(MethodName, Signature) class MethodName : public Violet::Script::Method<MethodName, Signature> \
	{ \
	public: \
		static const char * getName() { return #MethodName; } \
		static uint32 getIdentifier() { return std::hash<std::string>()(getName()); } \
	}
}

#include "violet/script/Script.inl"

#endif
