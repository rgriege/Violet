#ifndef VIOLET_Handle_H
#define VIOLET_Handle_H

#include "violet/math/LogConst.h"

#include <functional>
#include <type_traits>

namespace Violet
{
	template <size_t MaxId, size_t MaxVersion>
	class Handle
	{
	public:

		enum { MaxId = MaxId };
		enum { MaxVersion = MaxVersion };
		enum { IdStorageSize = log_ceil<MaxId>::value };
		enum { VersionStorageSize = log_ceil<MaxVersion>::value };
		enum { StorageSize = IdStorageSize + VersionStorageSize };
		using StorageType = typename std::conditional<StorageSize <= sizeof(unsigned) * 8, unsigned, long long unsigned>::type;

	public:

		static const Handle ms_invalid;

	public:

		Handle();
		Handle(StorageType id, StorageType version);

		StorageType getId() const;
		StorageType getVersion() const;
		StorageType getRaw() const;

		bool isValid() const;

	private:

		StorageType m_id : IdStorageSize;
		StorageType m_version : VersionStorageSize;
	};

	template <size_t MaxId, size_t MaxVersion>
	bool operator<(Handle<MaxId, MaxVersion> lhs, Handle<MaxId, MaxVersion> rhs);

	template <size_t MaxId, size_t MaxVersion>
	bool operator==(Handle<MaxId, MaxVersion> lhs, Handle<MaxId, MaxVersion> rhs);

	template <size_t MaxId, size_t MaxVersion>
	bool operator!=(Handle<MaxId, MaxVersion> lhs, Handle<MaxId, MaxVersion> rhs);
}

#include "violet/handle/Handle.inl"

namespace std
{
	template<size_t MaxId, size_t MaxVersion>
	struct hash<Violet::Handle<MaxId, MaxVersion>>
	{
		std::size_t operator()(const Violet::Handle<MaxId, MaxVersion> handle) const
		{
			return std::hash<typename Violet::Handle<MaxId, MaxVersion>::StorageType>()(handle.getRaw());
		}
	};
}

#endif