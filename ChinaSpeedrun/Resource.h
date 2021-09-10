#pragma once

#include <string>

namespace cs
{
	class Resource
	{
	public:
		bool operator==(const Resource& other);
		bool operator!=(const Resource& other);

		std::string const& GetResourcePath() const;

	protected:
		std::string resourcePath;
	};
}
