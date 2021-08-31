#include "ChinaEngine.h"

#include <cstring>
#include <map>

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
	PickPhysicalDevice();
}

void ChinaEngine::MainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}
}

void ChinaEngine::Cleanup()
{
	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

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
	return FindQueueFamilies(device).IsComplete();
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
	return graphicsFamily.has_value();
}
