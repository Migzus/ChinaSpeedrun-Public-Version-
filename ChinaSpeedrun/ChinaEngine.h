#pragma once

namespace cs
{
	namespace editor
	{
		class EngineEditor;
	}

	class ChinaEngine
	{
	public:
		static class EngineEditor editor;
		static class ImGuiLayer imGuiLayer;
		static class World world;
		static class VulkanEngineRenderer renderer;

		static void Run();
		static float AspectRatio();

	private:
		static void EngineInit();
		static void InitInput();
		static void MainLoop();
		static void EngineExit();
	};
}
