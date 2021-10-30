#pragma once

#include "ChinaCoreHeader.h"

namespace cs
{
	namespace editor
	{
		class EngineEditor;
	}

	class ChinaEngine
	{
	public:
		static editor::EngineEditor editor;
		static class ImGuiLayer imGuiLayer;
		static class World world;
		static class VulkanEngineRenderer renderer;

		static void Run();
		static float AspectRatio();

		static void FramebufferResizeCallback(GLFWwindow* window, int newWidth, int newHeight);

	private:
		static void EngineInit();
		static void InitInput();
		static void MainLoop();
		static void EngineExit();
	};
}
