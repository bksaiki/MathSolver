#include <iostream>
#include <string>
#include "../include/Integer.h"

using namespace MathSolver;

int main()
{
	std::string str = "123456789123456789";
	Integer i1(str);

	std::cout << i1.size() << " bytes : " << (i1.sign() ? "-" : "") << *(unsigned long*)i1.data() << std::endl;

	return 0;
}
