#pragma once

#include <vulkan/vulkan.h>

#define STB_IMAGE_IMPLEMENTATION

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define TINYOBJLOADER_IMPLEMENTATION

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "imgui.h"

#include <optional>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>
