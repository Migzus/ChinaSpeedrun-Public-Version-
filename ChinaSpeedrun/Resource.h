#pragma once

#include <string>

namespace cs
{
	class Resource
	{
	public:
		friend class ResourceManager;

		virtual void Initialize() = 0;

		bool operator==(const Resource& other);
		bool operator!=(const Resource& other);

		std::string const& GetResourcePath() const;

	protected:
		std::string resourcePath;
	};
}
