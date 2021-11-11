#include "VulkanEngineRenderer.h"

#include <stb_image.h>

#include <cstring>
#include <set>
#include <cstdint>
#include <algorithm>

#include "Vertex.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "RenderComponent.h"
#include "MeshRenderer.h"
#include "CameraComponent.h"
#include "ChinaEngine.h"
#include "ResourceManager.h"

#include "Scene.h"
#include "SceneManager.h"

#include "Debug.h"

cs::VulkanEngineRenderer::VulkanEngineRenderer() :
	vertexBuffer{ VulkanBufferInfo(MAX_BUFFER_SIZE) }, indexBuffer{ VulkanBufferInfo(MAX_BUFFER_SIZE) }
{
	status.indexDataSize = &indexBuffer.dataSize;
	status.vertexDataSize = &vertexBuffer.dataSize;
}

void cs::VulkanEngineRenderer::FramebufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	auto _app{ reinterpret_cast<VulkanEngineRenderer*>(glfwGetWindowUserPointer(window)) };
	_app->framebufferResized = true;
	_app->width = newWidth;
	_app->height = newHeight;

	ChinaEngine::FramebufferResizeCallback(window, newWidth, newHeight);
}

void cs::VulkanEngineRenderer::AllocateMesh(Mesh* mesh)
{
	// Test to see if we exceed the current buffer capacities, of both the vertex buffer and index buffer sizes
	VkDeviceSize _newVertexOffset{ vertexBuffer.dataSize + sizeof(Vertex) * mesh->vertices.size() },
		_newIndexOffset{ indexBuffer.dataSize + sizeof(uint32_t) * mesh->indices.size() };

	if (_newVertexOffset > vertexBuffer.bufferSize || _newIndexOffset > indexBuffer.bufferSize)
	{
		Debug::LogWarning("Cannot allocate more memory for this model. It is too large. Excess data:\n\t>",
			_newVertexOffset - vertexBuffer.bufferSize, "\t(bytes, vertices)\n\t>",
			_newIndexOffset - indexBuffer.bufferSize, "\t(bytes, indices)\n"
		);
		return;
	}

	// VERTEX ASSIGNMENT

	VkBuffer _stagingBuffer;
	VkDeviceMemory _stagingBufferMemory;
	CreateBuffer(vertexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);
	CopyBuffer(vertexBuffer.buffer, _stagingBuffer, vertexBuffer.bufferSize);

	mesh->vertexSize = sizeof(Vertex) * mesh->vertices.size();
	void* _vertexData;
	vkMapMemory(device, _stagingBufferMemory, vertexBuffer.dataSize, vertexBuffer.bufferSize - vertexBuffer.dataSize, 0, &_vertexData);
	memcpy(_vertexData, mesh->vertices.data(), (size_t)mesh->vertexSize);
	vkUnmapMemory(device, _stagingBufferMemory);

	mesh->vertexBufferOffset = vertexBuffer.dataSize;
	vertexBuffer.dataSize = _newVertexOffset;

	CopyBuffer(_stagingBuffer, vertexBuffer.buffer, vertexBuffer.bufferSize);

	vkDestroyBuffer(device, _stagingBuffer, nullptr);
	vkFreeMemory(device, _stagingBufferMemory, nullptr);

	mesh->vertexBufferRef = vertexBuffer.buffer;

	// INDEX ASSIGNMENT

	CreateBuffer(indexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);
	CopyBuffer(indexBuffer.buffer, _stagingBuffer, indexBuffer.bufferSize);

	mesh->indexSize = sizeof(uint32_t) * mesh->indices.size();
	void* _indexData;
	vkMapMemory(device, _stagingBufferMemory, indexBuffer.dataSize, indexBuffer.bufferSize - indexBuffer.dataSize, 0, &_indexData);
	memcpy(_indexData, mesh->indices.data(), (size_t)mesh->indexSize);
	vkUnmapMemory(device, _stagingBufferMemory);

	mesh->indexBufferOffset = indexBuffer.dataSize;
	indexBuffer.dataSize = _newIndexOffset;

	CopyBuffer(_stagingBuffer, indexBuffer.buffer, indexBuffer.bufferSize);

	vkDestroyBuffer(device, _stagingBuffer, nullptr);
	vkFreeMemory(device, _stagingBufferMemory, nullptr);

	mesh->indexBufferRef = indexBuffer.buffer;

	status.vertexDataFractionSize = vertexBuffer.UsedSpace();
	status.indexDataFractionSize = indexBuffer.UsedSpace();
}

void cs::VulkanEngineRenderer::AllocateTexture(Texture* texture)
{
	VkDeviceSize _imageSize{ static_cast<uint64_t>(texture->GetTextureByteSize()) };

	if (_imageSize <= 0)
	{
		Debug::LogWarning("Cannot allocate image. Image byte size is " + std::to_string(_imageSize));
		return;
	}

	VkBuffer _stagingBuffer;
	VkDeviceMemory _stagingBufferMemory;
	CreateBuffer(_imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);

	void* _data;
	vkMapMemory(device, _stagingBufferMemory, 0, _imageSize, 0, &_data);
	memcpy(_data, texture->pixels, static_cast<size_t>(_imageSize));
	vkUnmapMemory(device, _stagingBufferMemory);

	CreateImage(texture->width, texture->height, texture->mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture->texture, texture->textureMemory);

	TransitionImageLayout(texture->texture, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture->mipLevels);
	CopyBufferToImage(_stagingBuffer, texture->texture, texture->width, texture->height);
	GenerateMipmaps(texture->texture, VK_FORMAT_R8G8B8A8_SRGB, texture->width, texture->height, texture->mipLevels);

	vkDestroyBuffer(device, _stagingBuffer, nullptr);
	vkFreeMemory(device, _stagingBufferMemory, nullptr);

	// --- VIEW -------------------------------------------------------------------------

	texture->textureView = CreateImageView(texture->texture, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, texture->mipLevels);

	// --- SAMPLER ----------------------------------------------------------------------

	VkSamplerCreateInfo _samplerInfo{};
	_samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	_samplerInfo.magFilter = VK_FILTER_LINEAR;
	_samplerInfo.minFilter = VK_FILTER_LINEAR;
	_samplerInfo.addressModeU = static_cast<VkSamplerAddressMode>(texture->tilingX);
	_samplerInfo.addressModeV = static_cast<VkSamplerAddressMode>(texture->tilingY);
	_samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.anisotropyEnable = VK_FALSE;
	_samplerInfo.maxAnisotropy = 1.0f;
	_samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	_samplerInfo.compareEnable = VK_FALSE;
	_samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	_samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	_samplerInfo.mipLodBias = 0.0f;
	_samplerInfo.minLod = 0.0f;
	_samplerInfo.maxLod = static_cast<float>(texture->mipLevels);

	if (vkCreateSampler(device, &_samplerInfo, nullptr, &texture->textureSampler) != VK_SUCCESS)
		Debug::LogFail("Could not crate a texture sampler.");
}

void cs::VulkanEngineRenderer::SolveMesh(Mesh* mesh, Solve solveMode)
{
	if (mesh == nullptr)
	{
		Debug::LogIssue("Cannot assign a non existing mesh. -> mesh == nullptr");
		return;
	}

	solveMeshes[mesh] = solveMode;
}

void cs::VulkanEngineRenderer::SolveShader(Shader* shader, Solve solveMode)
{
	if (shader == nullptr)
	{
		Debug::LogIssue("Cannot assign a non existing shader. -> shader == nullptr");
		return;
	}

	solveShaders[shader] = solveMode;
}

void cs::VulkanEngineRenderer::SolveTexture(Texture* texture, Solve solveMode)
{
	if (texture == nullptr)
	{
		Debug::LogIssue("Cannot assign a non existing texture. -> texture == nullptr");
		return;
	}

	solveTextures[texture] = solveMode;
}

void cs::VulkanEngineRenderer::SolveMaterial(Material* material, Solve solveMode)
{
	if (material == nullptr)
	{
		Debug::LogIssue("Cannot assign a non existing material. -> material == nullptr");
		return;
	}

	solveMaterials[material] = solveMode;
}

void cs::VulkanEngineRenderer::SolveRenderer(RenderComponent* renderer, Solve solveMode)
{
	if (renderer == nullptr)
	{
		Debug::LogIssue("Cannot assign a new render type to vulkan. -> renderer == nullptr");
		return;
	}

	solveRenderers[renderer] = solveMode;
}

void cs::VulkanEngineRenderer::DestroyDescriptorPool(VkDescriptorPool& descriptorPool)
{
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void cs::VulkanEngineRenderer::MakeDescriptorPool(RenderComponent& renderer)
{
	CreateDescriptorPool(renderer);
	CreateDescriptorSets(renderer);
}

void cs::VulkanEngineRenderer::AllocateShader(Shader* shader)
{
	CreateDescriptorSetLayout(shader);
}

void cs::VulkanEngineRenderer::AllocateMaterial(Material* material)
{
	CreateGraphicsPipeline(material);
}

void cs::VulkanEngineRenderer::FreeMesh(Mesh* mesh)
{
	// we need to do some funnis with buffer allocation :)
}

void cs::VulkanEngineRenderer::FreeTexture(Texture* texture)
{
	vkDestroySampler(device, texture->textureSampler, nullptr);
	vkDestroyImageView(device, texture->textureView, nullptr);

	vkDestroyImage(device, texture->texture, nullptr);
	vkFreeMemory(device, texture->textureMemory, nullptr);
}

void cs::VulkanEngineRenderer::FreeShader(Shader* shader)
{
	vkDestroyPipelineLayout(device, shader->layout, nullptr);
}

void cs::VulkanEngineRenderer::FreeMaterial(Material* material)
{
	vkDestroyPipeline(device, material->pipeline, nullptr);
}

void cs::VulkanEngineRenderer::Create(int newWidth, int newHeight, const char* appTitle)
{
	width = newWidth;
	height = newHeight;
	appName = appTitle;

	InitWindow();
	InitVulkan();
	InitImGui();
}

void cs::VulkanEngineRenderer::GetViewportSize(int& widthRef, int& heightRef) const
{
	widthRef = width;
	heightRef = height;
}

cs::VulkanEngineRenderer::~VulkanEngineRenderer()
{
	Cleanup();
}

void cs::VulkanEngineRenderer::Resolve()
{
	for (auto& mesh : solveMeshes)
	{
		switch (mesh.second)
		{
		case Solve::NONE:
			break;
		case Solve::ADD:
			AllocateMesh(mesh.first);
			break;
		case Solve::UPDATE:
			// There is nothing to update for a mesh... (at least not yet)
			break;
		case Solve::REMOVE:
			FreeMesh(mesh.first);
			break;
		}
	}

	solveMeshes.clear();

	for (auto& shader : solveShaders)
	{
		switch (shader.second)
		{
		case Solve::NONE:
			break;
		case Solve::ADD:
			AllocateShader(shader.first);
			break;
		case Solve::UPDATE:
			UpdateShader(shader.first);
			break;
		case Solve::REMOVE:
			FreeShader(shader.first);
			break;
		}
	}

	solveShaders.clear();

	for (auto& texture : solveTextures)
	{
		switch (texture.second)
		{
		case Solve::NONE:
			break;
		case Solve::ADD:
			AllocateTexture(texture.first);
			break;
		case Solve::UPDATE:
			UpdateTexture(texture.first);
			break;
		case Solve::REMOVE:
			FreeTexture(texture.first);
			break;
		}
	}

	solveTextures.clear();

	for (auto& material : solveMaterials)
	{
		switch (material.second)
		{
		case Solve::NONE:
			break;
		case Solve::ADD:
			AllocateMaterial(material.first);
			break;
		case Solve::UPDATE:
			UpdateMaterial(material.first);
			break;
		case Solve::REMOVE:
			FreeMaterial(material.first);
			break;
		}
	}

	solveMaterials.clear();

	// then we finally edit the stuff for meshes

	for (auto& renderer : solveRenderers)
	{
		switch (renderer.second)
		{
		case Solve::NONE:
			break;
		case Solve::ADD:
			CreateDescriptorPool(*renderer.first);
			CreateDescriptorSets(*renderer.first);
			break;
		case Solve::UPDATE:
			break;
		case Solve::REMOVE:
			break;
		}
	}

	if (!solveRenderers.empty())
		CreateCommandBuffers();

	solveRenderers.clear();
}

void cs::VulkanEngineRenderer::UpdateShader(Shader* shader)
{
	FreeShader(shader);
	AllocateShader(shader);
}

void cs::VulkanEngineRenderer::UpdateTexture(Texture* texture)
{
	FreeTexture(texture);
	AllocateTexture(texture);
}

void cs::VulkanEngineRenderer::UpdateMaterial(Material* material)
{
	FreeMaterial(material);
	AllocateMaterial(material);
}

void cs::VulkanEngineRenderer::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
}

void cs::VulkanEngineRenderer::InitVulkan()
{
	CreateInstance();			// Necessarily Init Call
	SetupDebugMessenger();		// Init Call
	CreateSurface();			// Necessarily Init Call
	PickPhysicalDevice();		// Necessarily Init Call
	CreateLogicalDevice();		// Necessarily Init Call
	CreateSwapChain();			// Necessarily Init Call
	CreateImageViews();			// Necessarily Init Call
	CreateRenderPass();			// Necessarily Init Call (For Shader)
	//CreateDescriptorSetLayout();// Shader Spesific
	//CreateGraphicsPipeline();	// Material Spesific (Depends on a render pass)
	CreateColorResources();		// Necessarily Init Call
	CreateDepthResources();		// Necessarily Init Call
	CreateFramebuffers();		// Necessarily Init Call (But requires a render pass) (one way to counter this is to create a default shader)
	CreateCommandPool();		// Necessarily Init Call
	//CreateTextureImage();		// Texture Spesific
	//CreateTextureImageView();	// Texture Spesific
	//CreateTextureSampler();		// Texture Spesific
	CreateVertexBuffer();		// Necessarily Init Call (For Models)
	CreateIndexBuffer();		// Necessarily Init Call (For Models)
	CreateUniformBuffers();		// Necessarily Init Call (For Objects)
	//CreateDescriptorPool();		// Object Spesific (Technically we only need one descriptor pool, seeing as it describes all object traits in relation to the shader)
	//CreateDescriptorSets();		// Object Spesific (Here we mash up everything and create a material out of everything told to the shader)
	CreateCommandBuffers();		// Object Spesific (Whenever we create new objects we need to add them as draw commands) (also, depends on pipeline layout)
	CreateSyncObjects();		// Necessarily Init Call
}

void cs::VulkanEngineRenderer::DrawFrame()
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t _imageIndex;
	VkResult _result{ vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &_imageIndex) };

	if (_result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}
	else if (_result != VK_SUCCESS && _result != VK_SUBOPTIMAL_KHR)
		Debug::LogFail("Failed to acquire swap chain image.");

	if (imagesInFlight[_imageIndex] != VK_NULL_HANDLE)
		vkWaitForFences(device, 1, &imagesInFlight[_imageIndex], VK_TRUE, UINT64_MAX);

	imagesInFlight[_imageIndex] = inFlightFences[currentFrame];

	UpdateUniformBuffer(_imageIndex);

	if (_imageIndex == 0)
	{
		vkResetCommandPool(device, imGuiCommandPool, 0);
		vkResetCommandPool(device, commandPool, 0);
	}
	
	ImGuiUpdateDrawCommands(_imageIndex);
	UpdateDrawCommands(_imageIndex);

	VkSubmitInfo _submitInfo{};
	_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkCommandBuffer _commandBuffers[]{ commandBuffers[_imageIndex], imGuiCommandBuffers[_imageIndex] };
	VkSemaphore _waitSemaphores[]{ imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags _waitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	_submitInfo.waitSemaphoreCount = 1;
	_submitInfo.pWaitSemaphores = _waitSemaphores;
	_submitInfo.pWaitDstStageMask = _waitStages;
	_submitInfo.commandBufferCount = IM_ARRAYSIZE(_commandBuffers);
	_submitInfo.pCommandBuffers = _commandBuffers;

	VkSemaphore _signalSemaphores[]{ renderFinishedSemaphores[currentFrame] };
	_submitInfo.signalSemaphoreCount = 1;
	_submitInfo.pSignalSemaphores = _signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &_submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		Debug::LogFail("Failed to submit draw command buffer.");

	VkPresentInfoKHR _presentInfo{};
	_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	_presentInfo.waitSemaphoreCount = 1;
	_presentInfo.pWaitSemaphores = _signalSemaphores;

	VkSwapchainKHR _swapChains[]{ swapChain };
	_presentInfo.swapchainCount = 1;
	_presentInfo.pSwapchains = _swapChains;
	_presentInfo.pImageIndices = &_imageIndex;
	_presentInfo.pResults = nullptr;

	_result = vkQueuePresentKHR(presentQueue, &_presentInfo);

	if (_result == VK_ERROR_OUT_OF_DATE_KHR || _result == VK_SUBOPTIMAL_KHR || framebufferResized)
	{
		framebufferResized = false;
		RecreateSwapChain();
	}
	else if (_result != VK_SUCCESS)
		Debug::LogFail("Failed to present swap chain image.");

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkDevice const& cs::VulkanEngineRenderer::GetDevice()
{
	return device;
}

const cs::VulkanStatus& cs::VulkanEngineRenderer::GetStatus() const
{
	return status;
}

float cs::VulkanEngineRenderer::AspectRatio() const
{
	// This will change, I want to remove the if statement, and I want to not use this every frame, for every object
	static int _height{ 1 };

	if (height > 0)
		_height = height;

	return static_cast<float>(width) / static_cast<float>(_height);
}

GLFWwindow* cs::VulkanEngineRenderer::GetWindow()
{
	return window;
}

void cs::VulkanEngineRenderer::Cleanup()
{
	// Clean up all allocated resources
	Resolve();

	CleanupSwapChain();

	// --------------------------------------------------------------------------
	//     ImGui Stuff that we call when we end the program

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	vkDestroyDescriptorPool(device, imGuiDescriptorPool, nullptr);

	// --------------------------------------------------------------------------

	//for (auto shader : registeredShaders)
	//	vkDestroyDescriptorSetLayout(device, shader->descriptorSetLayout, nullptr);

	vkDestroyBuffer(device, indexBuffer.buffer, nullptr);
	vkFreeMemory(device, indexBuffer.bufferMemory, nullptr);

	vkDestroyBuffer(device, vertexBuffer.buffer, nullptr);
	vkFreeMemory(device, vertexBuffer.bufferMemory, nullptr);

	for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}

void cs::VulkanEngineRenderer::ImGuiRenderPass()
{
	VkAttachmentDescription _attachment{};
	_attachment.format = swapChainImageFormat;
	_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference _colorAttachment{};
	_colorAttachment.attachment = 0;
	_colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription _subpass{};
	_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	_subpass.colorAttachmentCount = 1;
	_subpass.pColorAttachments = &_colorAttachment;

	VkSubpassDependency _dependency{};
	_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	_dependency.dstSubpass = 0;
	_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	_dependency.srcAccessMask = 0;
	_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo _info{};
	_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	_info.attachmentCount = 1;
	_info.pAttachments = &_attachment;
	_info.subpassCount = 1;
	_info.pSubpasses = &_subpass;
	_info.dependencyCount = 1;
	_info.pDependencies = &_dependency;

	if (vkCreateRenderPass(device, &_info, nullptr, &imGuiRenderPass) != VK_SUCCESS)
		Debug::LogFail("Could not create ImGui render pass.");
}

void cs::VulkanEngineRenderer::ImGuiFramebuffers()
{
	imGuiFramebuffers.resize(swapChainImageViews.size());

	for (size_t i{ 0 }; i < imGuiFramebuffers.size(); i++)
	{
		VkFramebufferCreateInfo _framebufferInfo{};
		_framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		_framebufferInfo.renderPass = imGuiRenderPass;
		_framebufferInfo.attachmentCount = 1;
		_framebufferInfo.pAttachments = &swapChainImageViews[i];
		_framebufferInfo.width = swapChainExtent.width;
		_framebufferInfo.height = swapChainExtent.height;
		_framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &_framebufferInfo, nullptr, &imGuiFramebuffers[i]) != VK_SUCCESS)
			Debug::LogFail("Failed to create ImGui framebuffer.");
	}
}

void cs::VulkanEngineRenderer::ImGuiDescriptorPool()
{
	uint32_t _swapChainSize{ static_cast<uint32_t>(swapChainImages.size()) };

	VkDescriptorPoolSize _poolSizes[]
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, _swapChainSize }
	};

	VkDescriptorPoolCreateInfo _poolInfo{};
	_poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	_poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	_poolInfo.poolSizeCount = IM_ARRAYSIZE(_poolSizes);
	_poolInfo.pPoolSizes = _poolSizes;
	_poolInfo.maxSets = _swapChainSize;

	if (vkCreateDescriptorPool(device, &_poolInfo, nullptr, &imGuiDescriptorPool) != VK_SUCCESS)
		Debug::LogFail("Failed to create ImGui descriptor pool.");
}

void cs::VulkanEngineRenderer::ImGuiCommandPool()
{
	QueueFamilyIndices _queueFamilyIndices{ FindQueueFamilies(physicalDevice) };

	VkCommandPoolCreateInfo _poolInfo{};
	_poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	_poolInfo.queueFamilyIndex = _queueFamilyIndices.graphicsFamily.value();
	_poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &_poolInfo, nullptr, &imGuiCommandPool) != VK_SUCCESS)
		Debug::LogFail("Failed to create ImGui command pool.");
}

void cs::VulkanEngineRenderer::ImGuiCommandBuffers()
{
	imGuiCommandBuffers.resize(swapChainImageViews.size());

	VkCommandBufferAllocateInfo _commandBufferAllocateInfo{};
	_commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	_commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	_commandBufferAllocateInfo.commandPool = imGuiCommandPool;
	_commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(imGuiCommandBuffers.size());

	if (vkAllocateCommandBuffers(device, &_commandBufferAllocateInfo, imGuiCommandBuffers.data()) != VK_SUCCESS)
		Debug::LogFail("Failed to allocate ImGui command buffers.");
}

void cs::VulkanEngineRenderer::ImGuiUpdateDrawCommands(const uint32_t& imageIndex)
{
	VkCommandBufferBeginInfo _beginInfo{};
	_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	_beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(imGuiCommandBuffers[imageIndex], &_beginInfo);

	VkClearValue _clearValue{ 0.0f, 0.0f, 0.0f, 1.0f };
	VkRenderPassBeginInfo _renderPassInfo{};
	_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	_renderPassInfo.renderPass = imGuiRenderPass;
	_renderPassInfo.framebuffer = imGuiFramebuffers[imageIndex];
	_renderPassInfo.renderArea.offset = { 0, 0 };
	_renderPassInfo.renderArea.extent = swapChainExtent;
	_renderPassInfo.clearValueCount = 1;
	_renderPassInfo.pClearValues = &_clearValue;

	vkCmdBeginRenderPass(imGuiCommandBuffers[imageIndex], &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), imGuiCommandBuffers[imageIndex]);

	vkCmdEndRenderPass(imGuiCommandBuffers[imageIndex]);
	if (vkEndCommandBuffer(imGuiCommandBuffers[imageIndex]) != VK_SUCCESS)
		Debug::LogFail("Failed to record ImGui command buffer.");
}

void cs::VulkanEngineRenderer::UpdateDrawCommands(const uint32_t& imageIndex)
{
	VkCommandBufferBeginInfo _beginInfo{};
	_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	_beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffers[imageIndex], &_beginInfo);

	VkClearValue _clearValue[]{ { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0 } };
	VkRenderPassBeginInfo _renderPassInfo{};
	_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	_renderPassInfo.renderPass = renderPass;
	_renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	_renderPassInfo.renderArea.offset = { 0, 0 };
	_renderPassInfo.renderArea.extent = swapChainExtent;
	_renderPassInfo.clearValueCount = 2;
	_renderPassInfo.pClearValues = &_clearValue[0];

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	if (SceneManager::GetCurrentScene() != nullptr)
		for (auto* renderer : SceneManager::GetCurrentScene()->renderableObjects)
			if (renderer->visible)
				renderer->VulkanDraw(commandBuffers[imageIndex], imageIndex, vertexBuffer.buffer, indexBuffer.buffer);

	vkCmdEndRenderPass(commandBuffers[imageIndex]);
	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		Debug::LogFail("Failed to record command buffer.");
}

void cs::VulkanEngineRenderer::InitImGui()
{
	ImGuiRenderPass();
	ImGuiFramebuffers();
	ImGuiCommandPool();
	ImGuiDescriptorPool();
	ImGuiCommandBuffers();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& _io{ ImGui::GetIO() }; (void)_io;

	ImGui::StyleColorsDark();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForVulkan(window, false);
	ImGui_ImplVulkan_InitInfo _initInfo{};

	glfwSetWindowFocusCallback(window, ImGui_ImplGlfw_WindowFocusCallback);
	glfwSetCursorEnterCallback(window, ImGui_ImplGlfw_CursorEnterCallback);
	glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
	glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);

	_initInfo.Instance = instance;
	_initInfo.PhysicalDevice = physicalDevice;
	_initInfo.Device = device;
	_initInfo.QueueFamily = FindQueueFamilies(physicalDevice).graphicsFamily.value();
	_initInfo.Queue = graphicsQueue;
	_initInfo.PipelineCache = VK_NULL_HANDLE;
	_initInfo.DescriptorPool = imGuiDescriptorPool;
	_initInfo.Allocator = VK_NULL_HANDLE;
	_initInfo.MinImageCount = static_cast<uint32_t>(swapChainImages.size());
	_initInfo.ImageCount = static_cast<uint32_t>(swapChainImages.size());
	_initInfo.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&_initInfo, imGuiRenderPass);

	VkCommandBuffer _commandBuffer{ BeginSingleTimeCommands() };
	ImGui_ImplVulkan_CreateFontsTexture(_commandBuffer);
	EndSingleTimeCommands(_commandBuffer);
}

void cs::VulkanEngineRenderer::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationSupport())
		Debug::LogFail("Requested validation layer support, but non are available.");

	VkApplicationInfo _appInfo{};
	_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	_appInfo.pApplicationName = appName.c_str();
	_appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	_appInfo.pEngineName = ENGINE_NAME.c_str();
	_appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	_appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo _createInfo{};
	_createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	_createInfo.pApplicationInfo = &_appInfo;

	auto _extensions{ GetRequiredExtensions() };
	_createInfo.enabledExtensionCount = static_cast<uint32_t>(_extensions.size());
	_createInfo.ppEnabledExtensionNames = _extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT _debugCreateInfo{};

	if (enableValidationLayers)
	{
		_createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		_createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(_debugCreateInfo);
		_createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&_debugCreateInfo;
	}
	else
	{
		_createInfo.enabledLayerCount = 0;
		_createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&_createInfo, nullptr, &instance) != VK_SUCCESS)
		Debug::LogFail("Could not create a Vulkan Instance.");
}

void cs::VulkanEngineRenderer::SetupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT _createInfo{};
	PopulateDebugMessengerCreateInfo(_createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &_createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		Debug::LogFail("Failed to set up debug messenger.");
}

void cs::VulkanEngineRenderer::CreateSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		Debug::LogFail("Failed to create window surface.");
}

void cs::VulkanEngineRenderer::PickPhysicalDevice()
{
	uint32_t _deviceCount{ 0 };
	vkEnumeratePhysicalDevices(instance, &_deviceCount, nullptr);

	if (_deviceCount == 0)
		Debug::LogFail("No Vulkan supported GPUs on this device.");

	std::vector<VkPhysicalDevice> _devices{ _deviceCount };
	vkEnumeratePhysicalDevices(instance, &_deviceCount, _devices.data());

	for (const auto& device : _devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			msaaSamples = GetMaxSampleCount();
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
		Debug::LogFail("Failed to find a suitable GPU.");

	/*
	std::multimap<int, VkPhysicalDevice> _candidates;

	for (const auto& device : _devices)
		_candidates.insert(std::make_pair(RateDeviceSuitability(device), device));

	if (_candidates.rbegin()->first > 0)
		physicalDevice = _candidates.rbegin()->second;
	else
		throw std::runtime_error("[FAIL] :\tFailed to find a suitable GPU.");
	*/
}

void cs::VulkanEngineRenderer::CreateLogicalDevice()
{
	QueueFamilyIndices _indices{ FindQueueFamilies(physicalDevice) };

	std::vector<VkDeviceQueueCreateInfo> _queueCreateInfos;
	std::set<uint32_t> _uniqueQueueFamilies{ _indices.graphicsFamily.value(), _indices.presentFamily.value() };

	float queuePriority{ 1.0f };
	for (uint32_t queueFamily : _uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo _queueCreateInfo{};
		_queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		_queueCreateInfo.queueFamilyIndex = queueFamily;
		_queueCreateInfo.queueCount = 1;
		_queueCreateInfo.pQueuePriorities = &queuePriority;
		_queueCreateInfos.push_back(_queueCreateInfo);
	}

	VkPhysicalDeviceFeatures _deviceFeatures{};

	VkDeviceCreateInfo _createInfo{};
	_createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	_createInfo.queueCreateInfoCount = static_cast<uint32_t>(_queueCreateInfos.size());
	_createInfo.pQueueCreateInfos = _queueCreateInfos.data();
	_createInfo.pEnabledFeatures = &_deviceFeatures;
	_createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	_createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		_createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		_createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
		_createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(physicalDevice, &_createInfo, nullptr, &device) != VK_SUCCESS)
		Debug::LogFail("Failed to create logical device.");

	vkGetDeviceQueue(device, _indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, _indices.graphicsFamily.value(), 0, &presentQueue);
}

void cs::VulkanEngineRenderer::CreateSwapChain()
{
	SwapChainSupportDetails _swapChainSupport{ QuerySwapChainSupport(physicalDevice) };

	VkSurfaceFormatKHR _surfaceFormat{ ChooseSwapSurfaceFormat(_swapChainSupport.formats) };
	VkPresentModeKHR _presentMode{ ChooseSwapPresentMode(_swapChainSupport.presentModes) };
	VkExtent2D _extent{ ChooseSwapExtent(_swapChainSupport.capabilities) };

	uint32_t _imageCount{ _swapChainSupport.capabilities.minImageCount + 1 };

	if (_swapChainSupport.capabilities.maxImageCount > 0 && _imageCount > _swapChainSupport.capabilities.maxImageCount)
		_imageCount = _swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR _createInfo{};
	_createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	_createInfo.surface = surface;
	_createInfo.minImageCount = _imageCount;
	_createInfo.imageFormat = _surfaceFormat.format;
	_createInfo.imageColorSpace = _surfaceFormat.colorSpace;
	_createInfo.imageExtent = _extent;
	_createInfo.imageArrayLayers = 1;
	_createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices _indices{ FindQueueFamilies(physicalDevice) };
	uint32_t _queueFamilyIndices[]{ _indices.graphicsFamily.value(), _indices.presentFamily.value() };

	if (_indices.graphicsFamily != _indices.presentFamily)
	{
		_createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		_createInfo.queueFamilyIndexCount = 2;
		_createInfo.pQueueFamilyIndices = _queueFamilyIndices;
	}
	else
	{
		_createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		_createInfo.queueFamilyIndexCount = 0;
		_createInfo.pQueueFamilyIndices = nullptr;
	}

	_createInfo.preTransform = _swapChainSupport.capabilities.currentTransform;
	_createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	_createInfo.presentMode = _presentMode;
	_createInfo.clipped = VK_TRUE;
	_createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &_createInfo, nullptr, &swapChain) != VK_SUCCESS)
		Debug::LogFail("Failed to create swap chain.");

	vkGetSwapchainImagesKHR(device, swapChain, &_imageCount, nullptr);
	swapChainImages.resize(_imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &_imageCount, swapChainImages.data());

	swapChainImageFormat = _surfaceFormat.format;
	swapChainExtent = _extent;
}

void cs::VulkanEngineRenderer::CreateImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i{ 0 }; i < swapChainImages.size(); i++)
		swapChainImageViews[i] = CreateImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void cs::VulkanEngineRenderer::CreateDescriptorSetLayout(Shader* shader)
{
	std::vector<VkDescriptorSetLayoutBinding> _bindings;

	for (std::pair<std::string, VkDescriptorSetLayoutBinding> binding : shader->descriptorBindings)
		_bindings.push_back(binding.second);

	/*VkDescriptorSetLayoutBinding _uboLayoutBinding{};
	_uboLayoutBinding.binding = 0;
	_uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	_uboLayoutBinding.descriptorCount = 1;
	_uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	_uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding _samplerLayoutBinding{};
	_samplerLayoutBinding.binding = 1;
	_samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	_samplerLayoutBinding.descriptorCount = 1;
	_samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	_samplerLayoutBinding.pImmutableSamplers = nullptr;*/

	VkDescriptorSetLayoutCreateInfo _layoutInfo{};
	_layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	_layoutInfo.bindingCount = static_cast<uint32_t>(_bindings.size());
	_layoutInfo.pBindings = _bindings.data();

	if (vkCreateDescriptorSetLayout(device, &_layoutInfo, nullptr, &shader->descriptorSetLayout) != VK_SUCCESS)
		Debug::LogFail("Failed to create descriptor set layout.");

	VkPipelineLayoutCreateInfo _pipelineLayoutInfo{};
	_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	_pipelineLayoutInfo.setLayoutCount = 1;
	_pipelineLayoutInfo.pSetLayouts = &shader->descriptorSetLayout;
	_pipelineLayoutInfo.pushConstantRangeCount = 0;
	_pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &_pipelineLayoutInfo, nullptr, &shader->layout) != VK_SUCCESS)
		Debug::LogFail("Failed to create pipeline layout.");
}

void cs::VulkanEngineRenderer::CreateGraphicsPipeline(Material* material)
{
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	std::vector<VkShaderModule> _shaderModules;

	for (std::pair<std::string, RawData> code : material->shader->spvCode)
	{
		VkShaderModule _shaderModule{ CreateShaderModule(code.second) };

		VkPipelineShaderStageCreateInfo _shaderStageInfo{};
		_shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		_shaderStageInfo.stage = Shader::GetShaderStageFlag(code.first);
		_shaderStageInfo.module = _shaderModule;
		_shaderStageInfo.pName = "main";

		_shaderModules.push_back(_shaderModule);
		_shaderStages.push_back(_shaderStageInfo);
	}

	VkPipelineVertexInputStateCreateInfo _vertexInputInfo{};
	_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	// we get these from the shader instead
	std::vector<VkVertexInputBindingDescription> _bindingDescription{ Vertex::GetBindingDescription() };
	auto _attributeDescriptions{ Vertex::GetAttributeDescriptions() };

	// Add the extra vertex bindings
	if (!material->shader->vertexInputDescription.empty())
		_bindingDescription.insert(_bindingDescription.end(), material->shader->vertexInputDescription.size(), material->shader->vertexInputDescription.front());

	_vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(_bindingDescription.size());
	_vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions.size());
	_vertexInputInfo.pVertexBindingDescriptions = _bindingDescription.data();
	_vertexInputInfo.pVertexAttributeDescriptions = _attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo _inputAssembly{};
	_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	_inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport _viewport{};
	_viewport.x = 0.0f;
	_viewport.y = 0.0f;
	_viewport.width = (float)swapChainExtent.width;
	_viewport.height = (float)swapChainExtent.height;
	_viewport.minDepth = 0.0f;
	_viewport.maxDepth = 1.0f;

	VkRect2D _scissor{};
	_scissor.offset = { 0, 0 };
	_scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo _viewportState{};
	_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	_viewportState.viewportCount = 1;
	_viewportState.pViewports = &_viewport;
	_viewportState.scissorCount = 1;
	_viewportState.pScissors = &_scissor;

	VkPipelineRasterizationStateCreateInfo _rasterizer{};
	_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	_rasterizer.depthClampEnable = VK_FALSE;
	_rasterizer.rasterizerDiscardEnable = VK_FALSE;
	_rasterizer.polygonMode = static_cast<VkPolygonMode>(material->fillMode);
	_rasterizer.lineWidth = material->lineWidth;
	_rasterizer.cullMode = static_cast<VkCullModeFlags>(material->cullMode);
	_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	_rasterizer.depthBiasEnable = VK_FALSE;
	_rasterizer.depthBiasConstantFactor = 0.0f;
	_rasterizer.depthBiasClamp = 0.0f;
	_rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo _multisampling{};
	_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	_multisampling.sampleShadingEnable = VK_FALSE;
	_multisampling.rasterizationSamples = msaaSamples;
	_multisampling.minSampleShading = 1.0f;
	_multisampling.pSampleMask = nullptr;
	_multisampling.alphaToCoverageEnable = VK_FALSE;
	_multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo _depthStencil{};
	_depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	
	if (material->enableStencil)
	{
		_depthStencil.depthTestEnable = VK_FALSE;
		_depthStencil.depthWriteEnable = VK_FALSE;
		_depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
		_depthStencil.stencilTestEnable = VK_TRUE;
		_depthStencil.depthBoundsTestEnable = VK_TRUE;
	}
	else
	{
		_depthStencil.depthTestEnable = VK_TRUE;
		_depthStencil.depthWriteEnable = VK_TRUE;
		_depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		_depthStencil.stencilTestEnable = VK_FALSE;
		_depthStencil.depthBoundsTestEnable = VK_FALSE;
	}

	_depthStencil.minDepthBounds = 0.0f;
	_depthStencil.maxDepthBounds = 1.0f;
	_depthStencil.front = {};
	_depthStencil.back = {};

	// Make this attachment "visible" for materials would be preferable
	VkPipelineColorBlendAttachmentState _colorBlendAttachment{};
	_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	_colorBlendAttachment.blendEnable = VK_FALSE;
	
	switch (material->renderMode)
	{
	case Material::RenderMode::TRANSPARENT_:
		_colorBlendAttachment.blendEnable = VK_TRUE;
		_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		break;
	case Material::RenderMode::OPEQUE_:
	default:
		_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		break;
	}

	VkPipelineColorBlendStateCreateInfo _colorBlending{};
	_colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	_colorBlending.logicOpEnable = VK_FALSE;
	_colorBlending.logicOp = VK_LOGIC_OP_COPY;
	_colorBlending.attachmentCount = 1;
	_colorBlending.pAttachments = &_colorBlendAttachment;
	_colorBlending.blendConstants[0] = 0.0f;
	_colorBlending.blendConstants[1] = 0.0f;
	_colorBlending.blendConstants[2] = 0.0f;
	_colorBlending.blendConstants[3] = 0.0f;

	VkGraphicsPipelineCreateInfo _pipelineInfo{};
	_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	_pipelineInfo.stageCount = static_cast<uint32_t>(_shaderStages.size());
	_pipelineInfo.pStages = _shaderStages.data();
	_pipelineInfo.pVertexInputState = &_vertexInputInfo;
	_pipelineInfo.pInputAssemblyState = &_inputAssembly;
	_pipelineInfo.pViewportState = &_viewportState;
	_pipelineInfo.pRasterizationState = &_rasterizer;
	_pipelineInfo.pMultisampleState = &_multisampling;
	_pipelineInfo.pDepthStencilState = &_depthStencil;
	_pipelineInfo.pColorBlendState = &_colorBlending;
	_pipelineInfo.pDynamicState = nullptr;
	_pipelineInfo.layout = material->shader->layout;
	_pipelineInfo.renderPass = renderPass;
	_pipelineInfo.subpass = 0;
	_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	_pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &_pipelineInfo, nullptr, &material->pipeline) != VK_SUCCESS)
		Debug::LogFail("Failed to create graphics pipeline.");

	for (VkShaderModule shaderModule : _shaderModules)
		vkDestroyShaderModule(device, shaderModule, nullptr);
}

void cs::VulkanEngineRenderer::CreateRenderPass()
{
	VkAttachmentDescription _colorAttachment{};
	_colorAttachment.format = swapChainImageFormat;
	_colorAttachment.samples = msaaSamples;
	_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	_colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	_colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription _depthAttachment{};
	_depthAttachment.format = FindDepthFormat();
	_depthAttachment.samples = msaaSamples;
	_depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	_depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription _colorAttachmentResolve{};
	_colorAttachmentResolve.format = swapChainImageFormat;
	_colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	_colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	_colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference _colorAttachmentRef{};
	_colorAttachmentRef.attachment = 0;
	_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference _depthAttachmentRef{};
	_depthAttachmentRef.attachment = 1;
	_depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference _colorAttachmentResolveRef{};
	_colorAttachmentResolveRef.attachment = 2;
	_colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription _subpass{};
	_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	_subpass.colorAttachmentCount = 1;
	_subpass.pColorAttachments = &_colorAttachmentRef;
	_subpass.pDepthStencilAttachment = &_depthAttachmentRef;
	_subpass.pResolveAttachments = &_colorAttachmentResolveRef;

	VkSubpassDependency _dependency{};
	_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	_dependency.dstSubpass = 0;
	_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	_dependency.srcAccessMask = 0;
	_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> _attachments{ _colorAttachment, _depthAttachment, _colorAttachmentResolve };
	VkRenderPassCreateInfo _renderPassInfo{};
	_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	_renderPassInfo.attachmentCount = static_cast<uint32_t>(_attachments.size());
	_renderPassInfo.pAttachments = _attachments.data();
	_renderPassInfo.subpassCount = 1;
	_renderPassInfo.pSubpasses = &_subpass;
	_renderPassInfo.dependencyCount = 1;
	_renderPassInfo.pDependencies = &_dependency;

	if (vkCreateRenderPass(device, &_renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		Debug::LogFail("Failed to create render pass.");
}

void cs::VulkanEngineRenderer::CreateFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i{ 0 }; i < swapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 3> _attachments{ colorImageView, depthImageView, swapChainImageViews[i] };

		VkFramebufferCreateInfo _framebufferInfo{};
		_framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		_framebufferInfo.renderPass = renderPass;
		_framebufferInfo.attachmentCount = static_cast<uint32_t>(_attachments.size());
		_framebufferInfo.pAttachments = _attachments.data();
		_framebufferInfo.width = swapChainExtent.width;
		_framebufferInfo.height = swapChainExtent.height;
		_framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &_framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			Debug::LogFail("Failed to create framebuffer.");
	}
}

void cs::VulkanEngineRenderer::CreateCommandPool()
{
	QueueFamilyIndices _queueFamilyIndices{ FindQueueFamilies(physicalDevice) };

	VkCommandPoolCreateInfo _poolInfo{};
	_poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	_poolInfo.queueFamilyIndex = _queueFamilyIndices.graphicsFamily.value();
	_poolInfo.flags = 0;

	if (vkCreateCommandPool(device, &_poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		Debug::LogFail("Failed to create command pool.");
}

void cs::VulkanEngineRenderer::CreateColorResources()
{
	VkFormat colorFormat{ swapChainImageFormat };

	CreateImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
	colorImageView = CreateImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void cs::VulkanEngineRenderer::CreateDepthResources()
{
	VkFormat _depthFormat{ FindDepthFormat() };

	CreateImage(swapChainExtent.width, swapChainExtent.height, 1, msaaSamples, _depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = CreateImageView(depthImage, _depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

void cs::VulkanEngineRenderer::CreateTextureImage()
{
	int _texWidth, _texHeight, _texChannels;
	stbi_uc* _pixels{ stbi_load("../Resources/textures/chaika_smile.png", &_texWidth, &_texHeight, &_texChannels, STBI_rgb_alpha) };
	VkDeviceSize _imageSize{ static_cast<uint64_t>(_texWidth * _texHeight * 4) }; // 4 color channels (r, g, b, a)

	if (!_pixels)
		Debug::LogFail("Failed to load texture image.");

	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(_texWidth, _texHeight)))) + 1;

	VkBuffer _stagingBuffer;
	VkDeviceMemory _stagingBufferMemory;
	CreateBuffer(_imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _stagingBuffer, _stagingBufferMemory);

	void* _data;
	vkMapMemory(device, _stagingBufferMemory, 0, _imageSize, 0, &_data);
	memcpy(_data, _pixels, static_cast<size_t>(_imageSize));
	vkUnmapMemory(device, _stagingBufferMemory);

	stbi_image_free(_pixels);

	CreateImage(_texWidth, _texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
	CopyBufferToImage(_stagingBuffer, textureImage, static_cast<uint32_t>(_texWidth), static_cast<uint32_t>(_texHeight));
	GenerateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, _texWidth, _texHeight, mipLevels);

	vkDestroyBuffer(device, _stagingBuffer, nullptr);
	vkFreeMemory(device, _stagingBufferMemory, nullptr);
}

void cs::VulkanEngineRenderer::CreateTextureImageView()
{
	textureImageView = CreateImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

void cs::VulkanEngineRenderer::CreateTextureSampler()
{
	VkSamplerCreateInfo _samplerInfo{};
	_samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	_samplerInfo.magFilter = VK_FILTER_LINEAR;
	_samplerInfo.minFilter = VK_FILTER_LINEAR;
	_samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.anisotropyEnable = VK_FALSE;
	_samplerInfo.maxAnisotropy = 1.0f;
	_samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	_samplerInfo.compareEnable = VK_FALSE;
	_samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	_samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	_samplerInfo.mipLodBias = 0.0f;
	_samplerInfo.minLod = 0.0f;
	_samplerInfo.maxLod = static_cast<float>(mipLevels);

	if (vkCreateSampler(device, &_samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		Debug::LogFail("Failed to create texture sampler.");
}

void cs::VulkanEngineRenderer::CreateVertexBuffer()
{
	CreateBuffer(vertexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer.buffer, vertexBuffer.bufferMemory);
}

void cs::VulkanEngineRenderer::CreateIndexBuffer()
{
	CreateBuffer(indexBuffer.bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer.buffer, indexBuffer.bufferMemory);
}

void cs::VulkanEngineRenderer::CreateUniformBuffers()
{
	VkDeviceSize _bufferSize{ MAX_BUFFER_SIZE };

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i{ 0 }; i < swapChainImages.size(); i++)
		CreateBuffer(_bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
}

void cs::VulkanEngineRenderer::CreateDescriptorPool(RenderComponent& renderer)
{
	uint32_t _swapChainSize{ static_cast<uint32_t>(swapChainImages.size()) };
	VkDescriptorPoolSize _poolSizes[]
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, _swapChainSize },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _swapChainSize }
	};

	VkDescriptorPoolCreateInfo _poolInfo{};
	_poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	_poolInfo.poolSizeCount = IM_ARRAYSIZE(_poolSizes);
	_poolInfo.pPoolSizes = _poolSizes;
	_poolInfo.maxSets = _swapChainSize;

	if (vkCreateDescriptorPool(device, &_poolInfo, nullptr, &renderer.descriptorPool) != VK_SUCCESS)
		Debug::LogFail("Failed to create descriptor pool.");
}

void cs::VulkanEngineRenderer::CreateDescriptorSets(RenderComponent& renderer)
{
	if (renderer.material == nullptr)
		return;

	std::vector<VkDescriptorSetLayout> _layouts(swapChainImages.size(), renderer.material->shader->descriptorSetLayout);
	VkDescriptorSetAllocateInfo _allocInfo{};
	_allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	_allocInfo.descriptorPool = renderer.descriptorPool;
	_allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	_allocInfo.pSetLayouts = _layouts.data();

	renderer.descriptorSets.resize(swapChainImages.size());
	if (vkAllocateDescriptorSets(device, &_allocInfo, renderer.descriptorSets.data()) != VK_SUCCESS)
		Debug::LogFail("Failed to allocate descriptor sets.");

	for (size_t i{ 0 }; i < swapChainImages.size(); i++)
	{
		std::vector<VkWriteDescriptorSet> _descriptorWrites;

		for (auto& descriptorBinding : renderer.material->shader->descriptorBindings)
		{
			VkWriteDescriptorSet _descriptor{};

			_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			_descriptor.dstSet = renderer.descriptorSets[i];
			_descriptor.dstBinding = descriptorBinding.second.binding;
			_descriptor.dstArrayElement = 0;
			_descriptor.descriptorType = descriptorBinding.second.descriptorType;
			_descriptor.descriptorCount = 1;

			switch (descriptorBinding.second.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			{
				VkDescriptorBufferInfo _bufferInfo{};
				_bufferInfo.buffer = uniformBuffers[i];
				_bufferInfo.offset = renderer.uboOffset;
				_bufferInfo.range = static_cast<VkDeviceSize>(UniformBufferObject::GetByteSize());

				_descriptor.pBufferInfo = &_bufferInfo;
				break;
			}
			case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
				try
				{
					Texture* _texture{ std::get<Texture*>(renderer.material->shaderParams[descriptorBinding.first]) };

					VkDescriptorImageInfo _imageInfo{};
					_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					_imageInfo.imageView = _texture->textureView;
					_imageInfo.sampler = _texture->textureSampler;

					_descriptor.pImageInfo = &_imageInfo;
					break;
				}
				catch (const std::exception& e)
				{
					Debug::LogIssue(e.what());
					break;
				}
			}

			_descriptorWrites.push_back(_descriptor);
		}

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(_descriptorWrites.size()), _descriptorWrites.data(), 0, nullptr);
	}
}

void cs::VulkanEngineRenderer::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo _allocInfo{};
	_allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	_allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	_allocInfo.commandPool = commandPool;
	_allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &_allocInfo, commandBuffers.data()) != VK_SUCCESS)
		Debug::LogFail("Failed to allocate command buffers.");

	/*for (size_t i{ 0 }; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo _beginInfo{};
		_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		_beginInfo.flags = 0;
		_beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers[i], &_beginInfo) != VK_SUCCESS)
			Debug::LogFail("Failed to begin recording command buffer.");

		VkRenderPassBeginInfo _renderPassInfo{};
		_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		_renderPassInfo.renderPass = renderPass;
		_renderPassInfo.framebuffer = swapChainFramebuffers[i];
		_renderPassInfo.renderArea.offset = { 0, 0 };
		_renderPassInfo.renderArea.extent = swapChainExtent;

		std::array<VkClearValue, 2> _clearValues{};
		_clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		_clearValues[1].depthStencil = { 1.0f, 0 };

		_renderPassInfo.clearValueCount = static_cast<uint32_t>(_clearValues.size());
		_renderPassInfo.pClearValues = _clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		auto _renderers{ ChinaEngine::world.registry.view<MeshRendererComponent>() };
		for (auto e : _renderers)
		{
			MeshRendererComponent& _meshRenderer{ ChinaEngine::world.registry.get<MeshRendererComponent>(e) };

			if (_meshRenderer.mesh == nullptr || _meshRenderer.materials.empty())
				continue;

			// for now, let's support only one material per model
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _meshRenderer.materials[0]->pipeline);
			MeshRenderer::VulkanDraw(_meshRenderer, commandBuffers[i], _meshRenderer.materials[0]->shader->layout, i, vertexBuffer.buffer, indexBuffer.buffer);
		}

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			Debug::LogFail("Failed to record command buffer.");
	}*/
}

void cs::VulkanEngineRenderer::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo _semaphoreInfo{};
	_semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo _fenceInfo{};
	_fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	_fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; i++)
		if (vkCreateSemaphore(device, &_semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS || vkCreateSemaphore(device, &_semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS || vkCreateFence(device, &_fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			Debug::LogFail("Failed to create semaphores.");
}

void cs::VulkanEngineRenderer::CreateImage(uint32_t width, uint32_t height, uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkImageCreateInfo _imageInfo{};
	_imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	_imageInfo.imageType = VK_IMAGE_TYPE_2D;
	_imageInfo.extent.width = width;
	_imageInfo.extent.height = height;
	_imageInfo.extent.depth = 1;
	_imageInfo.mipLevels = mipmapLevels;
	_imageInfo.arrayLayers = 1;
	_imageInfo.format = format;
	_imageInfo.tiling = tiling;
	_imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_imageInfo.usage = usage;
	_imageInfo.samples = numSamples;
	_imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &_imageInfo, nullptr, &image) != VK_SUCCESS)
		Debug::LogFail("Failed to create image.");

	VkMemoryRequirements _memRequirements;
	vkGetImageMemoryRequirements(device, image, &_memRequirements);

	VkMemoryAllocateInfo _allocInfo{};
	_allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	_allocInfo.allocationSize = _memRequirements.size;
	_allocInfo.memoryTypeIndex = FindMemoryType(_memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &_allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		Debug::LogFail("Failed to allocate image memory.");

	vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView cs::VulkanEngineRenderer::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipmapLevels)
{
	VkImageViewCreateInfo _viewInfo{};
	_viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	_viewInfo.image = image;
	_viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	_viewInfo.format = format;
	_viewInfo.subresourceRange.aspectMask = aspectFlags;
	_viewInfo.subresourceRange.baseMipLevel = 0;
	_viewInfo.subresourceRange.levelCount = mipmapLevels;
	_viewInfo.subresourceRange.baseArrayLayer = 0;
	_viewInfo.subresourceRange.layerCount = 1;

	VkImageView _imageView;
	if (vkCreateImageView(device, &_viewInfo, nullptr, &_imageView) != VK_SUCCESS)
		Debug::LogFail("Failed to create texture image view.");

	return _imageView;
}

void cs::VulkanEngineRenderer::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		Debug::LogFail("Texture image format does not support linear blitting.");

	VkCommandBuffer _commandBuffer{ BeginSingleTimeCommands() };

	VkImageMemoryBarrier _barrier{};
	_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	_barrier.image = image;
	_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	_barrier.subresourceRange.baseArrayLayer = 0;
	_barrier.subresourceRange.layerCount = 1;
	_barrier.subresourceRange.levelCount = 1;

	int32_t _mipWidth{ texWidth };
	int32_t _mipHeight{ texHeight };

	for (uint32_t i{ 1 }; i < mipLevels; i++)
	{
		_barrier.subresourceRange.baseMipLevel = i - 1;
		_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &_barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { _mipWidth, _mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { _mipWidth > 1 ? _mipWidth / 2 : 1, _mipHeight > 1 ? _mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(_commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
	
		_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &_barrier);
	
		if (_mipWidth > 1)
			_mipWidth /= 2;
		if (_mipHeight > 1)
			_mipHeight /= 2;
	}

	_barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(_commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &_barrier);

	EndSingleTimeCommands(_commandBuffer);
}

VkSampleCountFlagBits cs::VulkanEngineRenderer::GetMaxSampleCount()
{
	VkPhysicalDeviceProperties _physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &_physicalDeviceProperties);

	VkSampleCountFlags _counts{ _physicalDeviceProperties.limits.framebufferColorSampleCounts & _physicalDeviceProperties.limits.framebufferDepthSampleCounts };
	if (_counts & VK_SAMPLE_COUNT_64_BIT)
		return VK_SAMPLE_COUNT_64_BIT;
	if (_counts & VK_SAMPLE_COUNT_32_BIT)
		return VK_SAMPLE_COUNT_32_BIT;
	if (_counts & VK_SAMPLE_COUNT_16_BIT)
		return VK_SAMPLE_COUNT_16_BIT;
	if (_counts & VK_SAMPLE_COUNT_8_BIT)
		return VK_SAMPLE_COUNT_8_BIT;
	if (_counts & VK_SAMPLE_COUNT_4_BIT) 
		return VK_SAMPLE_COUNT_4_BIT;
	if (_counts & VK_SAMPLE_COUNT_2_BIT)
		return VK_SAMPLE_COUNT_2_BIT;

	return VK_SAMPLE_COUNT_1_BIT;
}

bool cs::VulkanEngineRenderer::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat cs::VulkanEngineRenderer::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

VkFormat cs::VulkanEngineRenderer::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties _props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &_props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (_props.linearTilingFeatures & features) == features)
			return format;
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (_props.optimalTilingFeatures & features) == features)
			return format;
	}

	// extremely unlikely, unless the graphics card is really old...
	Debug::LogFail("Failed to find supported format.");
}

void cs::VulkanEngineRenderer::UpdateUniformBuffer(uint32_t currentImage)
{
	if (SceneManager::HasScenes())
	{
		auto _renderers{ SceneManager::GetRegistry().view<MeshRendererComponent>() };
		for (auto e : _renderers)
		{
			MeshRendererComponent& _meshRenderer{ SceneManager::GetRegistry().get<MeshRendererComponent>(e) };

			void* _data;
			vkMapMemory(device, uniformBuffersMemory[currentImage], _meshRenderer.uboOffset, UniformBufferObject::GetByteSize(), 0, &_data);
			memcpy(_data, &_meshRenderer.ubo, UniformBufferObject::GetByteSize());
			vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
		}
	}
}

VkCommandBuffer cs::VulkanEngineRenderer::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo _allocInfo{};
	_allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	_allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	_allocInfo.commandPool = commandPool;
	_allocInfo.commandBufferCount = 1;

	VkCommandBuffer _commandBuffer;
	vkAllocateCommandBuffers(device, &_allocInfo, &_commandBuffer);

	VkCommandBufferBeginInfo _beginInfo{};
	_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	_beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(_commandBuffer, &_beginInfo);

	return _commandBuffer;
}

void cs::VulkanEngineRenderer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	VkBufferCreateInfo _bufferInfo{};
	_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	_bufferInfo.size = size;
	_bufferInfo.usage = usage;
	_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &_bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		Debug::LogFail("Failed to create buffer.");

	VkMemoryRequirements _memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &_memRequirements);

	VkMemoryAllocateInfo _allocInfo{};
	_allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	_allocInfo.allocationSize = _memRequirements.size;
	_allocInfo.memoryTypeIndex = FindMemoryType(_memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &_allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		Debug::LogFail("Failed to allocate buffer memory.");

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void cs::VulkanEngineRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer _commandBuffer{ BeginSingleTimeCommands() };

	VkBufferCopy _copyRegion{};
	_copyRegion.srcOffset = 0;
	_copyRegion.dstOffset = 0;
	_copyRegion.size = size;
	vkCmdCopyBuffer(_commandBuffer, srcBuffer, dstBuffer, 1, &_copyRegion);

	EndSingleTimeCommands(_commandBuffer);
}

void cs::VulkanEngineRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer _commandBuffer{ BeginSingleTimeCommands() };

	VkBufferImageCopy _region{};
	_region.bufferOffset = 0;
	_region.bufferRowLength = 0;
	_region.bufferImageHeight = 0;
	_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	_region.imageSubresource.mipLevel = 0;
	_region.imageSubresource.baseArrayLayer = 0;
	_region.imageSubresource.layerCount = 1;
	_region.imageOffset = { 0, 0, 0 };
	_region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(_commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &_region);

	EndSingleTimeCommands(_commandBuffer);
}

void cs::VulkanEngineRenderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo _submitInfo{};
	_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	_submitInfo.commandBufferCount = 1;
	_submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &_submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void cs::VulkanEngineRenderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipmapLevels)
{
	VkCommandBuffer _commandBuffer{ BeginSingleTimeCommands() };

	VkImageMemoryBarrier _barrier{};
	_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	_barrier.oldLayout = oldLayout;
	_barrier.newLayout = newLayout;
	_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	_barrier.image = image;
	_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	_barrier.subresourceRange.baseMipLevel = 0;
	_barrier.subresourceRange.levelCount = mipmapLevels;
	_barrier.subresourceRange.baseArrayLayer = 0;
	_barrier.subresourceRange.layerCount = 1;
	_barrier.srcAccessMask = 0;
	_barrier.dstAccessMask = 0;

	VkPipelineStageFlags _sourceStage;
	VkPipelineStageFlags _destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		_barrier.srcAccessMask = 0;
		_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		_sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		_destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		_sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		_destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
		Debug::LogFail("Unsupported layout transition.");

	vkCmdPipelineBarrier(_commandBuffer, _sourceStage, _destinationStage, 0, 0, nullptr, 0, nullptr, 1, &_barrier);

	EndSingleTimeCommands(_commandBuffer);
}

void cs::VulkanEngineRenderer::RecreateSwapChain()
{
	int width{ 0 }, height{ 0 };

	glfwGetFramebufferSize(window, &width, &height);

	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	
	for (auto& shader : ResourceManager::shaders)
		UpdateShader(shader.second);
	for (auto& material : ResourceManager::materials)
		UpdateMaterial(material.second);

	CreateColorResources();
	CreateDepthResources();
	CreateFramebuffers();
	CreateUniformBuffers();
	SceneManager::CreateDescriptorPools();
	CreateCommandBuffers();

	ImGuiRenderPass();
	ImGuiFramebuffers();
	ImGuiCommandPool();
	ImGuiDescriptorPool();
	ImGuiCommandBuffers();

	ImGui_ImplVulkan_SetMinImageCount(static_cast<uint32_t>(swapChainImages.size()));
	
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
}

void cs::VulkanEngineRenderer::CleanupSwapChain()
{
	vkDestroyImageView(device, colorImageView, nullptr);
	vkDestroyImage(device, colorImage, nullptr);
	vkFreeMemory(device, colorImageMemory, nullptr);

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : swapChainFramebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);

	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapChainImageViews)
		vkDestroyImageView(device, imageView, nullptr);

	vkDestroySwapchainKHR(device, swapChain, nullptr);

	for (size_t i{ 0 }; i < swapChainImages.size(); i++)
	{
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	if (SceneManager::GetCurrentScene() != nullptr)
		SceneManager::DestroyDescriptorPools();

	// ----------------------------------------------------------------------
	//    Destroy ImGui Stuff

	for (auto framebuffer : imGuiFramebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);

	vkDestroyRenderPass(device, imGuiRenderPass, nullptr);

	vkFreeCommandBuffers(device, imGuiCommandPool, static_cast<uint32_t>(imGuiCommandBuffers.size()), imGuiCommandBuffers.data());
	vkDestroyCommandPool(device, imGuiCommandPool, nullptr);
}

VkShaderModule cs::VulkanEngineRenderer::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo _createInfo{};
	_createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	_createInfo.codeSize = code.size();
	_createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule _shaderModule;
	if (vkCreateShaderModule(device, &_createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		Debug::LogFail("Failed to create shader module.");

	return _shaderModule;
}

VkSurfaceFormatKHR cs::VulkanEngineRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	return availableFormats[0];
}

VkPresentModeKHR cs::VulkanEngineRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// VK_PRESENT_MODE_IMMEDIATE_KHR : May lead to screen tearing.
	// VK_PRESENT_MODE_FIFO_KHR :      Is similar to v-sync.

	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	return VK_PRESENT_MODE_FIFO_KHR;
}

cs::SwapChainSupportDetails cs::VulkanEngineRenderer::QuerySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails _details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &_details.capabilities);

	uint32_t _formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &_formatCount, nullptr);

	if (_formatCount != 0)
	{
		_details.formats.resize(_formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &_formatCount, _details.formats.data());
	}

	uint32_t _presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &_presentModeCount, nullptr);

	if (_presentModeCount != 0)
	{
		_details.presentModes.resize(_presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &_presentModeCount, _details.presentModes.data());
	}

	return _details;
}

VkExtent2D cs::VulkanEngineRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D _actualExtent{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		_actualExtent.width = std::clamp(_actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		_actualExtent.height = std::clamp(_actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return _actualExtent;
	}
}

cs::QueueFamilyIndices cs::VulkanEngineRenderer::FindQueueFamilies(VkPhysicalDevice device)
{
	int i{ 0 };
	uint32_t _queueFamilyCount{ 0 };
	QueueFamilyIndices _indices{};

	vkGetPhysicalDeviceQueueFamilyProperties(device, &_queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> _queueFamilies{ _queueFamilyCount };
	vkGetPhysicalDeviceQueueFamilyProperties(device, &_queueFamilyCount, _queueFamilies.data());

	for (const auto& queueFamily : _queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			_indices.graphicsFamily = i;

		VkBool32 presentSupport{ false };
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport)
			_indices.presentFamily = i;

		if (_indices.IsComplete())
			break;

		i++;
	}

	return _indices;
}

uint32_t cs::VulkanEngineRenderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i{ 0 }; i < memProperties.memoryTypeCount; i++)
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;

	Debug::LogFail("Failed to find suitable memory type.");
}

// currently an unused function, will use it in the future (probably) (not)
int cs::VulkanEngineRenderer::RateDeviceSuitability(VkPhysicalDevice device)
{
	int _score{ 0 };
	VkPhysicalDeviceProperties _deviceProperties;
	VkPhysicalDeviceFeatures _deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &_deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &_deviceFeatures);

	// If we have access to an external GPU, we give this choice a good score
	// If it is an integrated GPU, then we don't desire it that much
	_score += _deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0;
	// Prioritise the GPU with the highest 2D Image capacity.
	_score += _deviceProperties.limits.maxImageDimension2D;

	// As an example, if the gpu doesn't have a geometry shader
	// then we do not desire it at all.
	/*
	if (!deviceFeatures.geometryShader)
		return 0;
	*/

	return _score;
}

bool cs::VulkanEngineRenderer::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices _indices{ FindQueueFamilies(device) };
	bool _extensionsSupported{ CheckDeviceExtensionSupport(device) };

	bool _swapChainAdequate{ false };
	if (_extensionsSupported)
	{
		SwapChainSupportDetails _swapChainSupport{ QuerySwapChainSupport(device) };
		_swapChainAdequate = !_swapChainSupport.formats.empty() && !_swapChainSupport.presentModes.empty();
	}

	return FindQueueFamilies(device).IsComplete() && _extensionsSupported && _swapChainAdequate;
}

bool cs::VulkanEngineRenderer::CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t _extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &_extensionCount, nullptr);

	std::vector<VkExtensionProperties> _availableExtensions{ _extensionCount };
	vkEnumerateDeviceExtensionProperties(device, nullptr, &_extensionCount, _availableExtensions.data());

	std::set<std::string> _requiredExtensions{ deviceExtensions.begin(), deviceExtensions.end() };

	for (const auto& extension : _availableExtensions)
		_requiredExtensions.erase(extension.extensionName);


	return _requiredExtensions.empty();
}

bool cs::VulkanEngineRenderer::CheckValidationSupport()
{
	uint32_t _layerCount{ 0 };
	vkEnumerateInstanceLayerProperties(&_layerCount, nullptr);

	std::vector<VkLayerProperties> _availableLayers{ _layerCount };
	vkEnumerateInstanceLayerProperties(&_layerCount, _availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool _layerFound{ false };

		for (const auto& layerProperties : _availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				_layerFound = true;
				break;
			}
		}

		if (!_layerFound)
			return false;
	}

	return true;
}

std::vector<const char*> cs::VulkanEngineRenderer::GetRequiredExtensions()
{
	uint32_t _glfwExtensionCount{ 0 };
	const char** _glfwExtensions;
	_glfwExtensions = glfwGetRequiredInstanceExtensions(&_glfwExtensionCount);

	std::vector<const char*> _extensions{ _glfwExtensions, _glfwExtensions + _glfwExtensionCount };

	if (enableValidationLayers)
	{
		_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return _extensions;
}

VkResult cs::VulkanEngineRenderer::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto _func{ (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT") };

	if (_func != nullptr)
		return _func(instance, pCreateInfo, pAllocator, pDebugMessenger);

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void cs::VulkanEngineRenderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}

void cs::VulkanEngineRenderer::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto _func{ (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT") };

	if (_func != nullptr)
		_func(instance, debugMessenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL cs::VulkanEngineRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	Debug::LogValidationLayer(static_cast<std::string>(pCallbackData->pMessage));
	
	return VK_FALSE;
}

bool cs::QueueFamilyIndices::IsComplete()
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}

cs::VulkanBufferInfo::VulkanBufferInfo(VkDeviceSize newBufferSize) :
	buffer{ nullptr }, bufferMemory{ nullptr }, bufferSize{ newBufferSize }, dataSize{ 0 }
{}

const float cs::VulkanBufferInfo::UsedSpace() const
{
	return dataSize / (float)bufferSize;
}

cs::VulkanStatus::VulkanStatus() :
	indexDataFractionSize{ 0.0f }, vertexDataFractionSize{ 0.0f }, indexDataSize{ nullptr }, vertexDataSize{ nullptr }
{}
