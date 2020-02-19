#include <iostream>
#include "../include/Integer.h"

using namespace MathSolver;

int main()
{
	unsigned int x = 3500000000;
	signed int y = -2000000000;
	Integer a(x);
	Integer b(y);
	Integer i1 = a;
	Integer i2 = b;
	Integer i3 = -a;
	Integer i4 = -b;

	i1 -= b;
	i2 -= a;
	i3 -= b;
	i4 -= a;

	std::cout << i1.size() << " bytes : " << (i1.sign() ? "-" : "") << *(unsigned long*)i1.data() << std::endl;
	std::cout << i2.size() << " bytes : " << (i2.sign() ? "-" : "") << *(unsigned long*)i2.data() << std::endl;
	std::cout << i3.size() << " bytes : " << (i3.sign() ? "-" : "") << *(unsigned long*)i3.data() << std::endl;
	std::cout << i4.size() << " bytes : " << (i4.sign() ? "-" : "") << *(unsigned long*)i4.data() << std::endl;

	return 0;
}
