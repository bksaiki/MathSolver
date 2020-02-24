#include <iostream>
#include <string>
#include "../include/Integer.h"

using namespace MathSolver;

int main()
{
	while (true)
	{
		std::string input;
		std::cout << " > ";
		std::cin >> input;

		if(input == "exit")
			break;

		Integer i(input);
		std::cout << i.toString() << std::endl;
	}

	return 0;
}
