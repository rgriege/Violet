#ifndef VIOLET_CppScriptComponent_H
#define VIOLET_CppScriptComponent_H

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

		static Tag getTag();

	public:

		CppScriptComponent(const Entity & owner, Deserializer & deserializer);
		CppScriptComponent(CppScriptComponent && other);
		//CppScriptComponent & operator=(CppScriptComponent && other);
		~CppScriptComponent();

		std::string getFilename() const;
		void reload();
		template <typename ResultType, typename... Args>
		ResultType run(const char * method, Args&&... args) const;

	private:

		CppScriptComponent(const CppScriptComponent &) = delete;
		CppScriptComponent & operator=(const CppScriptComponent &) = delete;

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
}

#include "CppScriptComponent.inl"

#endif