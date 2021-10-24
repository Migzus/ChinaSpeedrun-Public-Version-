#pragma once

#include <iostream>
#include <string>

namespace cs
{
	namespace iocolors
	{
		constexpr char WHITE[8]{ 0x1b, '[', '0', ';', '3', '9', 'm', 0 };
		constexpr char RED[8]{ 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
		constexpr char GREEN[8]{ 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
		constexpr char LIGHT_YELLOW[8]{ 0x1b, '[', '1', ';', '3', '3', 'm', 0 };
		constexpr char CYAN[8]{ 0x1b, '[', '1', ';', '3', '6', 'm', 0 };
	}

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
		template<class ...Params>
		static void Log(Params&&...message);
		// Log importance, it is the same as Log, however; this is highlighted in a color.
		template<class ...Params>
		static void LogInfo(Params&&...message);
		// Log a success, this is called when something succeeds, usually when the compilation of scripts was successful.
		template<class ...Params>
		static void LogSuccess(Params&&...message);
		// Log a warning, this is not essential to your game. This is usually called when something didn't import correctly.
		template<class ...Params>
		static void LogWarning(Params&&...message);
		// Log an issue, giving the user the ability to know what is bugged and needs to be fixed.
		template<class ...Params>
		static void LogIssue(Params&&...message);
		// Calling this will prevent your game from running, calling it in-game will stop the game.
		template<class ...Params>
		static void LogError(Params&&...message);
		// Calling this closes the engine, potentially preventing damage.
		// It will also write a log file to the logs folder. (hehe not really...)
		template<class ...Params>
		static void LogFail(Params&&...message);
		// Send your own custom message
		//template<class ...Params>
		//static void LogCustom(std::string message, class Texture* texture, class Color color, Status status, Params...all);
	};
	
	template<class ...Params>
	inline void Debug::Log(Params&&...message)
	{
		std::cout << iocolors::WHITE << "[LOG]\t\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		std::cout << '\n';
	}
	
	template<class ...Params>
	inline void Debug::LogInfo(Params&&...message)
	{
		std::cout << iocolors::CYAN << "[INFO]\t\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		std::cout << '\n';
	}
	
	template<class ...Params>
	inline void Debug::LogSuccess(Params&&...message)
	{
		std::cout << iocolors::GREEN << "[SUCCESS]\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		std::cout << '\n';
	}
	
	template<class ...Params>
	inline void Debug::LogWarning(Params&&...message)
	{
		std::cout << iocolors::LIGHT_YELLOW << "[WARNING]\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		std::cout << '\n';
	}
	
	template<class ...Params>
	inline void Debug::LogIssue(Params&&...message)
	{
		std::cout << iocolors::RED << "[ISSUE]\t\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		std::cout << '\n';
	}
	
	template<class ...Params>
	inline void Debug::LogError(Params&&...message)
	{
		std::cout << iocolors::RED << "[ERROR]\t\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		std::cout << '\n';
	}
	
	template<class ...Params>
	inline void Debug::LogFail(Params&&...message)
	{
		std::cout << iocolors::RED << "[FAIL]\t\t" << iocolors::WHITE << ": ";
		((std::cout << std::forward<Params>(message)), ...);
		throw std::runtime_error("\n--- TERMINATED ---------------------------------------------\n");
	}
}
