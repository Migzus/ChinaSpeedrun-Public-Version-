#include "RenderComponent.h"

cs::UniformBufferObject::UniformBufferObject() :
	model{ Matrix4x4(1.0f) }, view{ Matrix4x4(1.0f) }, proj{ Matrix4x4(1.0f) }
{}

uint32_t cs::UniformBufferObject::GetByteSize()
{
	return sizeof(UniformBufferObject);
}
