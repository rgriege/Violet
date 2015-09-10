#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include "violet/template/TupleUtilities.h"

#include <unordered_map>
#include <memory>

namespace Violet
{
	template <typename Resource, typename ... Args>
	class ResourceCache
	{
	public:

		typedef std::unique_ptr<Resource>(*Loader)(Args ...);

	public:

		ResourceCache(Loader loader) :
			m_loader(loader)
		{
		}

		std::shared_ptr<Resource> fetch(Args ... args)
		{
			std::shared_ptr<Resource> resource;
			const auto key = std::make_tuple(args...);
			const auto it = m_resources.find(key);
			if (it == m_resources.end())
			{
				resource.reset(m_loader(get<Args>(key)...).release());
				m_resources.emplace_hint(it, std::make_pair(key, resource));
			}
			else
				resource = it->second;
			return resource;
		}

		void clear()
		{
			m_resources.clear();
		}

	private:

		Loader m_loader;
		std::unordered_map<std::tuple<Args...>, std::shared_ptr<Resource>> m_resources;
	};
}

#endif