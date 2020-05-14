#include <iostream>
#include <string>
#include "../lib/mathsolver.h"
#include "../src/interpreter.h"

using namespace MathSolver;

int main(int argc, char** argv)
{
    std::string input;
    bool exit = false;

    std::cout << "Math Solver " << MATHSOLVER_VERSION_STR << std::endl;
    while (!exit)
    {
        std::cout << "> ";
        std::getline(std::cin, input);
        int res = parseLine(input);
        
        if (res != 0)  exit = true;
        else if (res < 0 || gErrorManager.hasFatal()) return -1;


        gErrorManager.clear();
        input.clear();
    }
    
    return 0;
}