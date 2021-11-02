#pragma once

namespace ImGuizmo
{
	enum OPERATION;
	enum MODE;
}

namespace cs
{
	namespace editor
	{
		class EditorCamera;
		class ImGuiLayer;

		class EngineEditor
		{
		public:
			friend EditorCamera;
			friend ImGuiLayer;

			enum class Playmode
			{
				EDITOR,
				PLAY,
				PAUSE
			};

			const ImGuizmo::OPERATION& GetOperationState();
			const ImGuizmo::MODE& GetMode();
			const Playmode& GetPlaymodeState();
			void SetPlaymode(const Playmode newPlaymode);

			void Start();
			void Update();
			void Exit();

		private:
			ImGuizmo::OPERATION operation;
			ImGuizmo::MODE operationMode;
			Playmode mode;
			ImGuiLayer* uiLayer;
			EditorCamera* editorCamera;
		};
	}
}
