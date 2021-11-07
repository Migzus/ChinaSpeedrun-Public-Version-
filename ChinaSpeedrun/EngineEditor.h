#pragma once

namespace ImGuizmo
{
	enum OPERATION;
	enum MODE;
}

namespace cs
{
	class GameObject;

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
			GameObject* GetSelectedGameObject() const;
			void SetSelectedGameObject(GameObject* gameObject);

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
