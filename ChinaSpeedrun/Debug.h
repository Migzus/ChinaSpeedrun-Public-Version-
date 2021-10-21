#pragma once

namespace cs
{
	class Debug
	{
	public:
		enum class Status
		{
			NONE,
			PING,
			NOTICE,
			LETHAL
		};

		// Log, here you may log anything.
		static void Log(const char* string, ...);
		// Log importance, it is the same as Log, however; this is highlighted in a color.
		static void LogImportant(const char* string, ...);
		// Log a success, this is called when something succeeds, usually when the compilation of scripts was successful.
		static void LogSuccess(const char* string, ...);
		// Log a warning, this is not essential to your game. This is usually called when something didn't import correctly.
		static void LogWarning(const char* string, ...);
		// Log an issue, giving the user the ability to know what is bugged and needs to be fixed.
		static void LogIssue(const char* string, ...);
		// Calling this will prevent your game from running, calling it in-game will stop the game.
		static void LogError(const char* string, ...);
		// Send your own custom message
		static void LogCustom(const char* message, class Texture* texture, class Color color, Status status, ...);
	};
}
