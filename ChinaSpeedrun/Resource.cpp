#include "Resource.h"

bool cs::Resource::operator==(const Resource& other)
{
	return resourcePath == other.GetResourcePath();
}

bool cs::Resource::operator!=(const Resource& other)
{
	return resourcePath != other.GetResourcePath();
}

std::string const& cs::Resource::GetResourcePath() const
{
	return resourcePath;
}
