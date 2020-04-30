#include <iostream>
#include <string>
#include "../lib/mathsolver.h"
#include "../src/interpreter.h"

int main(int argc, char** argv)
{
    std::string input;
    bool exit = false;

    std::cout << "Math Solver " << MATHSOLVER_VERSION << std::endl;
    while (!exit)
    {
        std::cout << "> ";
        std::getline(std::cin, input);
        int res = parseLine(input);
        
        if (res != 0)       exit = true;
        else if (res < 0)   std::cout << "Error encountered\n" << std::endl;

        input.clear();
    }
    
    return 0;
}