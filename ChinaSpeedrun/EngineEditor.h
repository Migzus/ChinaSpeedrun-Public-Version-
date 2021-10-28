#pragma once

namespace cs
{
	namespace editor
	{
		class EngineEditor
		{
		public:
			enum class Playmode
			{
				EDITOR,
				PLAY,
				PAUSE
			};

			static const Playmode& GetPlaymodeState();
			static void SetPlaymode(const Playmode newPlaymode);

			static void Start();
			static void Update();
			static void Exit();

		private:
			static Playmode mode;
			static class EditorCamera* editorCamera;
		};
	}
}
