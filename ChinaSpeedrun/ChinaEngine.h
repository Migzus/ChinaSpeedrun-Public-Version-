#pragma once

#include <vulkan/vulkan.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool IsComplete();
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class ChinaEngine
{
public:
	static std::vector<char> ReadFile(const std::string& filename);

	void Run();
protected:
	
private:
	const std::string APP_NAME{ "China Speedrun" };
	const std::string ENGINE_NAME{ "ChinaEngine" };
	const std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
	const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
	const bool enableValidationLayers{ flase };
#else
	const bool enableValidationLayers{ true };
#endif

	const uint32_t WIDTH{ 800 };
	const uint32_t HEIGHT{ 600 };

	GLFWwindow* window;

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkImage> swapChainImages;

	VkSemaphore imageAvailableSemaphore, renderFinishedSemaphore;
	VkCommandPool commandPool;
	VkPipeline graphicsPipeline;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkSwapchainKHR swapChain;
	VkSurfaceKHR surface;
	VkQueue graphicsQueue, presentQueue;
	VkDevice device;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };

	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void DrawFrame();
	void Cleanup();

	void CreateInstance();
	void SetupDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateGraphicsPipeline();
	void CreateRenderPass();
	void CreateFramebuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSemaphores();

	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	int RateDeviceSuitability(VkPhysicalDevice device);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	bool CheckValidationSupport();
	std::vector<const char*> GetRequiredExtensions();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};
