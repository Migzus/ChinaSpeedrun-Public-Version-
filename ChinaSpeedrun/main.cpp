#include <iostream>

#include "ChinaEngine.h"

/// <summary>
/// 
///		The Vulkan code was part help by this tutorial : https://vulkan-tutorial.com/en 
///		Otherwise the base Vulkan code was modified by Mads Gjengaar and Jarle Kluge
/// 
///		What license should we use? MIT maybe?
/// 
/// </summary>
/// <returns></returns>

int main() {
    ChinaEngine engine;

	try
	{
		engine.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}