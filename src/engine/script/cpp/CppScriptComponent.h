#ifndef VIOLET_CppScriptComponent_H
#define VIOLET_CppScriptComponent_H

#include "engine/component/Component.h"

#include <memory>
#include <string>

namespace Violet
{
	class Deserializer;
	class Serializer;
    class SharedLibrary;

	class VIOLET_API CppScriptComponent : public ComponentBase<CppScriptComponent>
	{
	public:

		class VIOLET_API Allocator
		{
		public:

			Allocator();
			Allocator(Allocator && other);
			Allocator & operator=(Allocator && other);
			~Allocator();

			template <typename T, typename ... Args>
			T * allocate(Args && ... args);
			template <typename T>
			void deallocate();
			void * fetch() const;

		private:

			void * allocate(size_t size);

			Allocator(const Allocator &) = delete;
			Allocator & operator=(const Allocator &) = delete;

		private:

			void * m_memory;
		};

	public:

		static Tag getStaticTag();

	public:

		CppScriptComponent(const Entity & owner, Deserializer & deserializer);
		CppScriptComponent(CppScriptComponent && other);
		~CppScriptComponent();

		std::string getFilename() const;
		void reload();
		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		CppScriptComponent(const CppScriptComponent &) = delete;
		CppScriptComponent & operator=(const CppScriptComponent &) = delete;

		void * getMethodPtr(const char * name) const;
		void * getMemoryPtr() const;

		void load(const char * filename);
		void unload();

	private:

        std::shared_ptr<SharedLibrary> m_lib;
		Allocator m_allocator;
	};

	Serializer & operator<<(Serializer & serializer, const CppScriptComponent & component);
}

#include "engine/script/cpp/CppScriptComponent.inl"

#ifdef _WIN32
#define VIOLET_SCRIPT_EXPORT extern "C" __declspec(dllexport)
#else
#define VIOLET_SCRIPT_EXPORT extern "C"
#endif

#endif
