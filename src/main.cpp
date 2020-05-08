#include <iostream>
#include <string>
#include "../lib/mathsolver.h"
#include "../src/interpreter.h"

using namespace MathSolver;

int main(int argc, char** argv)
{
    std::string input;
    bool exit = false;
    bool verbose = true;

    std::cout << "Math Solver " << MATHSOLVER_VERSION_STR << std::endl;
    while (!exit)
    {
        std::cout << "> ";
        std::getline(std::cin, input);
        int res = parseLine(input);
        
        if (res != 0)    
        {
            exit = true;
        }  
        else if (res < 0 || gErrorManager.hasFatal())
        {
            for (auto e : gErrorManager.fatal())     std::cout << e << std::endl;
            return -1;
            
        }

        if (gErrorManager.hasAny())
        {
            if (verbose)
            {
                for (auto e : gErrorManager.messages())     std::cout << e << std::endl;
                for (auto e : gErrorManager.warnings())     std::cout << e << std::endl;
            }

            for (auto e : gErrorManager.errors()) std::cout << e << std::endl;
        }

        gErrorManager.clear();
        input.clear();
    }
    
    return 0;
}