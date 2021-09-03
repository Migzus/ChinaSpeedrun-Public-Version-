#include "ChinaEngine.h"

#include <cstring>
#include <map>
#include <set>
#include <cstdint>
#include <algorithm>

std::vector<char> ChinaEngine::ReadFile(const std::string& filename)
{
	std::ifstream _file{ filename, std::ios::ate | std::ios::binary };

	if (!_file.is_open())
		throw std::runtime_error("[FAIL] :\tFailed to open " + filename);

	size_t _fileSize{ (size_t)_file.tellg() };
	std::vector<char> _buffer(_fileSize);

	_file.seekg(0);
	_file.read(_buffer.data(), _fileSize);

	_file.close();

	return _buffer;
}

void ChinaEngine::Run()
{
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}

void ChinaEngine::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, APP_NAME.c_str(), nullptr, nullptr);
}

void ChinaEngine::InitVulkan()
{
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFramebuffers();
	CreateCommandPool();
	CreateCommandBuffers();
	CreateSyncObjects();
}

void ChinaEngine::MainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		DrawFrame();
	}

	vkDeviceWaitIdle(device);
}

void ChinaEngine::DrawFrame()
{
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t _imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &_imageIndex);

	if (imagesInFlight[_imageIndex] != VK_NULL_HANDLE)
		vkWaitForFences(device, 1, &imagesInFlight[_imageIndex], VK_TRUE, UINT64_MAX);
	
	imagesInFlight[_imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo _submitInfo{};
	_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore _waitSemaphores[]{ imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags _waitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	_submitInfo.waitSemaphoreCount = 1;
	_submitInfo.pWaitSemaphores = _waitSemaphores;
	_submitInfo.pWaitDstStageMask = _waitStages;
	_submitInfo.commandBufferCount = 1;
	_submitInfo.pCommandBuffers = &commandBuffers[_imageIndex];

	VkSemaphore _signalSemaphores[]{ renderFinishedSemaphores[currentFrame] };
	_submitInfo.signalSemaphoreCount = 1;
	_submitInfo.pSignalSemaphores = _signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &_submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to submit draw command buffer.");

	VkPresentInfoKHR _presentInfo{};
	_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	_presentInfo.waitSemaphoreCount = 1;
	_presentInfo.pWaitSemaphores = _signalSemaphores;
	
	VkSwapchainKHR _swapChains[]{ swapChain };
	_presentInfo.swapchainCount = 1;
	_presentInfo.pSwapchains = _swapChains;
	_presentInfo.pImageIndices = &_imageIndex;
	_presentInfo.pResults = nullptr;

	vkQueuePresentKHR(presentQueue, &_presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void ChinaEngine::Cleanup()
{
	for (size_t i{ 0 }; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	for (auto framebuffer : swapChainFramebuffers)
		vkDestroyFramebuffer(device, framebuffer, nullptr);

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapChainImageViews)
		vkDestroyImageView(device, imageView, nullptr);

	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	
	glfwDestroyWindow(window);

	glfwTerminate();
}

void ChinaEngine::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationSupport())
		throw std::runtime_error("[ERROR] :\tRequested validation layer support, but non are available.");

	VkApplicationInfo _appInfo{};
	_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	_appInfo.pApplicationName = APP_NAME.c_str();
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
		throw std::runtime_error("[FAIL] :\tCould not create a Vulkan Instance.");
}

void ChinaEngine::SetupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT _createInfo{};
	PopulateDebugMessengerCreateInfo(_createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &_createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to set up debug messenger.");
}

void ChinaEngine::CreateSurface()
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to create window surface.");
}

void ChinaEngine::PickPhysicalDevice()
{
	uint32_t _deviceCount{ 0 };
	vkEnumeratePhysicalDevices(instance, &_deviceCount, nullptr);

	if (_deviceCount == 0)
		throw std::runtime_error("[FAIL] :\tNo Vulkan supported GPUs on this device.");

	std::vector<VkPhysicalDevice> _devices{ _deviceCount };
	vkEnumeratePhysicalDevices(instance, &_deviceCount, _devices.data());

	for (const auto& device : _devices)
	{
		if (IsDeviceSuitable(device))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("[FAIL] :\tFailed to find a suitable GPU.");

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

void ChinaEngine::CreateLogicalDevice()
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
	{
		_createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &_createInfo, nullptr, &device) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to create logical device.");

	vkGetDeviceQueue(device, _indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, _indices.graphicsFamily.value(), 0, &presentQueue);
}

void ChinaEngine::CreateSwapChain()
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
		throw std::runtime_error("[FAIL] :\tFailed to create swap chain.");

	vkGetSwapchainImagesKHR(device, swapChain, &_imageCount, nullptr);
	swapChainImages.resize(_imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &_imageCount, swapChainImages.data());

	swapChainImageFormat = _surfaceFormat.format;
	swapChainExtent = _extent;
}

void ChinaEngine::CreateImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i{ 0 }; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo _createInfo{};
		_createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		_createInfo.image = swapChainImages[i];
		_createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		_createInfo.format = swapChainImageFormat;
		_createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		_createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		_createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		_createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		_createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		_createInfo.subresourceRange.baseMipLevel = 0;
		_createInfo.subresourceRange.levelCount = 1;
		_createInfo.subresourceRange.baseArrayLayer = 0;
		_createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &_createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
			throw std::runtime_error("[FAIL] :\tFailed to create image views.");
	}
}

void ChinaEngine::CreateGraphicsPipeline()
{
	auto _vertShaderCode{ ReadFile("../Resources/shaders/vert.spv") };
	auto _fragShaderCode{ ReadFile("../Resources/shaders/frag.spv") };

	VkShaderModule _vertShaderModule{ CreateShaderModule(_vertShaderCode) };
	VkShaderModule _fragShaderModule{ CreateShaderModule(_fragShaderCode) };

	VkPipelineShaderStageCreateInfo _vertShaderStageInfo{};
	_vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	_vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	_vertShaderStageInfo.module = _vertShaderModule;
	_vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo _fragShaderStageInfo{};
	_fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	_fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	_fragShaderStageInfo.module = _fragShaderModule;
	_fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo _shaderStages[]{ _vertShaderStageInfo, _fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo _vertexInputInfo{};
	_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	_vertexInputInfo.vertexBindingDescriptionCount = 0;
	_vertexInputInfo.pVertexBindingDescriptions = nullptr;
	_vertexInputInfo.vertexAttributeDescriptionCount = 0;
	_vertexInputInfo.pVertexAttributeDescriptions = nullptr;

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
	_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	_rasterizer.lineWidth = 1.0f;
	_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	_rasterizer.depthBiasEnable = VK_FALSE;
	_rasterizer.depthBiasConstantFactor = 0.0f;
	_rasterizer.depthBiasClamp = 0.0f;
	_rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo _multisampling{};
	_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	_multisampling.sampleShadingEnable = VK_FALSE;
	_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	_multisampling.minSampleShading = 1.0f;
	_multisampling.pSampleMask = nullptr;
	_multisampling.alphaToCoverageEnable = VK_FALSE;
	_multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState _colorBlendAttachment{};
	_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	_colorBlendAttachment.blendEnable = VK_FALSE;
	_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

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

	VkPipelineLayoutCreateInfo _pipelineLayoutInfo{};
	_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	_pipelineLayoutInfo.setLayoutCount = 0;
	_pipelineLayoutInfo.pSetLayouts = nullptr;
	_pipelineLayoutInfo.pushConstantRangeCount = 0;
	_pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &_pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to create pipeline layout.");

	VkGraphicsPipelineCreateInfo _pipelineInfo{};
	_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	_pipelineInfo.stageCount = 2;
	_pipelineInfo.pStages = _shaderStages;
	_pipelineInfo.pVertexInputState = &_vertexInputInfo;
	_pipelineInfo.pInputAssemblyState = &_inputAssembly;
	_pipelineInfo.pViewportState = &_viewportState;
	_pipelineInfo.pRasterizationState = &_rasterizer;
	_pipelineInfo.pMultisampleState = &_multisampling;
	_pipelineInfo.pDepthStencilState = nullptr;
	_pipelineInfo.pColorBlendState = &_colorBlending;
	_pipelineInfo.pDynamicState = nullptr;
	_pipelineInfo.layout = pipelineLayout;
	_pipelineInfo.renderPass = renderPass;
	_pipelineInfo.subpass = 0;
	_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	_pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &_pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to create graphics pipeline.");

	vkDestroyShaderModule(device, _fragShaderModule, nullptr);
	vkDestroyShaderModule(device, _vertShaderModule, nullptr);
}

void ChinaEngine::CreateRenderPass()
{
	VkAttachmentDescription _colorAttachment{};
	_colorAttachment.format = swapChainImageFormat;
	_colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	_colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	_colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	_colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	_colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference _colorAttachmentRef{};
	_colorAttachmentRef.attachment = 0;
	_colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription _subpass{};
	_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	_subpass.colorAttachmentCount = 1;
	_subpass.pColorAttachments = &_colorAttachmentRef;

	VkSubpassDependency _dependency{};
	_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	_dependency.dstSubpass = 0;
	_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	_dependency.srcAccessMask = 0;
	_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo _renderPassInfo{};
	_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	_renderPassInfo.attachmentCount = 1;
	_renderPassInfo.pAttachments = &_colorAttachment;
	_renderPassInfo.subpassCount = 1;
	_renderPassInfo.pSubpasses = &_subpass;
	_renderPassInfo.dependencyCount = 1;
	_renderPassInfo.pDependencies = &_dependency;

	if (vkCreateRenderPass(device, &_renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to create render pass.");
}

void ChinaEngine::CreateFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i{ 0 }; i < swapChainImageViews.size(); i++) {
		VkImageView _attachments[]{ swapChainImageViews[i] };

		VkFramebufferCreateInfo _framebufferInfo{};
		_framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		_framebufferInfo.renderPass = renderPass;
		_framebufferInfo.attachmentCount = 1;
		_framebufferInfo.pAttachments = _attachments;
		_framebufferInfo.width = swapChainExtent.width;
		_framebufferInfo.height = swapChainExtent.height;
		_framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &_framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to create framebuffer!");
	}
}

void ChinaEngine::CreateCommandPool()
{
	QueueFamilyIndices _queueFamilyIndices{ FindQueueFamilies(physicalDevice) };

	VkCommandPoolCreateInfo _poolInfo{};
	_poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	_poolInfo.queueFamilyIndex = _queueFamilyIndices.graphicsFamily.value();
	_poolInfo.flags = 0;

	if (vkCreateCommandPool(device, &_poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to create command pool.");
}

void ChinaEngine::CreateCommandBuffers()
{
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo _allocInfo{};
	_allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	_allocInfo.commandPool = commandPool;
	_allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	_allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &_allocInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("[FAIL] :\tFailed to allocate command buffers.");

	for (size_t i{ 0 }; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo _beginInfo{};
		_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		_beginInfo.flags = 0;
		_beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers[i], &_beginInfo) != VK_SUCCESS)
			throw std::runtime_error("[FAIL] :\tFailed to begin recording command buffer.");

		VkRenderPassBeginInfo _renderPassInfo{};
		_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		_renderPassInfo.renderPass = renderPass;
		_renderPassInfo.framebuffer = swapChainFramebuffers[i];
		_renderPassInfo.renderArea.offset = { 0, 0 };
		_renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue _clearColor{ {{0.0f, 0.0f, 0.0f, 1.0f}} };
		_renderPassInfo.clearValueCount = 1;
		_renderPassInfo.pClearValues = &_clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("[FAIL] :\tFailed to record command buffer.");
	}
}

void ChinaEngine::CreateSyncObjects()
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
			throw std::runtime_error("[FAIL] :\tFailed to create semaphores.");
}

VkShaderModule ChinaEngine::CreateShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo _createInfo{};
	_createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	_createInfo.codeSize = code.size();
	_createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule _shaderModule;
	if (vkCreateShaderModule(device, &_createInfo, nullptr, &_shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");

	return _shaderModule;
}

VkSurfaceFormatKHR ChinaEngine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;

	return availableFormats[0];
}

VkPresentModeKHR ChinaEngine::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	// VK_PRESENT_MODE_IMMEDIATE_KHR : May lead to screen tearing.
	// VK_PRESENT_MODE_FIFO_KHR :      Is similar to v-sync.

	for (const auto& availablePresentMode : availablePresentModes)
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;

	return VK_PRESENT_MODE_FIFO_KHR;
}

SwapChainSupportDetails ChinaEngine::QuerySwapChainSupport(VkPhysicalDevice device)
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

VkExtent2D ChinaEngine::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

QueueFamilyIndices ChinaEngine::FindQueueFamilies(VkPhysicalDevice device)
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

// currently an unused function, will use it in the future
int ChinaEngine::RateDeviceSuitability(VkPhysicalDevice device)
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

bool ChinaEngine::IsDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices _indices{ FindQueueFamilies(device) };
	bool _extensionsSupported{ CheckDeviceExtensionSupport(device) };

	bool _swapChainAdequate{ false };
	if (_extensionsSupported)
	{
		SwapChainSupportDetails _swapChainSupport{ QuerySwapChainSupport(device) };
		_swapChainAdequate = !_swapChainSupport.formats.empty() && !_swapChainSupport.presentModes.empty();
	}

	return FindQueueFamilies(device).IsComplete()  && _extensionsSupported && _swapChainAdequate;
}

bool ChinaEngine::CheckDeviceExtensionSupport(VkPhysicalDevice device)
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

bool ChinaEngine::CheckValidationSupport()
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

std::vector<const char*> ChinaEngine::GetRequiredExtensions()
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

VkResult ChinaEngine::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto _func{ (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT") };

	if (_func != nullptr)
		return _func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void ChinaEngine::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr;
}

void ChinaEngine::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto _func{ (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT") };

	if (_func != nullptr)
		_func(instance, debugMessenger, pAllocator);
}

VKAPI_ATTR VkBool32 VKAPI_CALL ChinaEngine::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "[INFO] :\t Validation layers: " << pCallbackData->pMessage << '\n';

	return VK_FALSE;
}

bool QueueFamilyIndices::IsComplete()
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}
