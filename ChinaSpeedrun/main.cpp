#include <iostream>

#include "ChinaEngine.h"

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