#ifndef RESOURCE_CACHE_H
#define RESOURCE_CACHE_H

#include "violet/core/utility/StringUtilities.h"

#include <map>
#include <memory>

namespace Violet
{
	template <typename Resource, std::unique_ptr<Resource>(*Loader)(const char *)>
	class ResourceCache
	{
	public:

		std::shared_ptr<Resource> fetch(const char * filename)
		{
			std::shared_ptr<Resource> resource;
			auto const it = m_resources.find(filename);
			if (it == m_resources.end())
			{
				resource.reset(Loader(filename).release());
				m_resources.emplace_hint(it, std::make_pair(filename, resource));
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

		std::map<const char *, std::shared_ptr<Resource>, StringUtilities::Less> m_resources;
	};
}

#endif