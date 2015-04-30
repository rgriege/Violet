#ifndef CPP_SCRIPT_COMPONENT_H
#define CPP_SCRIPT_COMPONENT_H

#include "engine/component/Component.h"

#include <string>

#ifndef _WINDEF_
struct HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
typedef HINSTANCE HMODULE;
#endif

namespace Violet
{
	class Deserializer;
	class Serializer;

	class VIOLET_API CppScriptComponent : public Component
	{
	public:

		class VIOLET_API Allocator
		{
		public:

			Allocator();
			Allocator(Allocator && other);
			Allocator & operator=(Allocator && other);
			~Allocator();

			template <typename T>
			T * allocate();
			void * fetch() const;

		private:

			void * allocate(size_t size);

			Allocator(const Allocator &) = delete;
			Allocator & operator=(const Allocator &) = delete;

		private:

			void * m_memory;
		};

	public:

		static const char * getLabel();

	public:

		CppScriptComponent(const Entity & entity, Deserializer & deserializer);
		CppScriptComponent(CppScriptComponent && other);
		CppScriptComponent & operator=(CppScriptComponent && other);
		~CppScriptComponent();

		std::string getFilename() const;
		void reload();
		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args);

	private:

		std::string getFilenameWithPath() const;
		void * getMethodPtr(const char * name) const;
		void * getMemoryPtr() const;

		void load(const char * filename);
		void unload();

	private:

		HMODULE m_lib;
		Allocator m_allocator;
	};

	Serializer & operator<<(Serializer & serializer, const CppScriptComponent & component);


	template <typename T>
	T * CppScriptComponent::Allocator::allocate()
	{
		return static_cast<T*>(allocate(sizeof(T)));
	}

	template <typename ResultType, typename... Args>
	ResultType CppScriptComponent::run(const char * method, Args&&... args)
	{
		void * methodPtr = getMethodPtr(method);
		if (methodPtr != nullptr)
		{
			if (getMemoryPtr() == nullptr)
			{
				auto m = (ResultType(*)(Args...)) methodPtr;
				return m(std::forward<Args>(args)...);
			}
			else
			{
				auto m = (ResultType(*)(Args..., void *)) methodPtr;
				return m(std::forward<Args>(args)..., getMemoryPtr());
			}
		}

		return ResultType();
	}
}

#endif