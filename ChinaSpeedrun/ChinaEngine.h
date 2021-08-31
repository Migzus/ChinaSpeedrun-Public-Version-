#pragma once

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>
#include <string>
#include <iostream>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;

	bool IsComplete();
};

class ChinaEngine
{
public:
	void Run();
protected:
	
private:
	const std::string APP_NAME{ "China Speedrun" };
	const std::string ENGINE_NAME{ "ChinaEngine" };
	const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
	const bool enableValidationLayers{ flase };
#else
	const bool enableValidationLayers{ true };
#endif

	const uint32_t WIDTH{ 800 };
	const uint32_t HEIGHT{ 600 };

	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void Cleanup();

	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	int RateDeviceSuitability(VkPhysicalDevice device);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckValidationSupport();
	std::vector<const char*> GetRequiredExtensions();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};
