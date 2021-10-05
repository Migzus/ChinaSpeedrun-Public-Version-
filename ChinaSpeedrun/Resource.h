#pragma once

#include <string>

namespace cs
{
	class Resource
	{
	public:
		std::string resourcePath;

		bool operator==(const Resource& other);
		bool operator!=(const Resource& other);

		virtual void Initialize() = 0;

		std::string const& GetResourcePath() const;
	};
}
