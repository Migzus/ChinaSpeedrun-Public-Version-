#pragma once

namespace cs
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

	private:
		static Playmode mode;
	};
}
