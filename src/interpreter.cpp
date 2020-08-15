#include <iostream>
#include <list>
#include "interpreter.h"

using namespace MathSolver;

int parseLine(const std::string& line)
{
    if (line == "exit" || line == "quit")  
        return 1;

    ExprNode* eval = parseString(line);

    if (gErrorManager.hasError())
        std::cout << gErrorManager.toString() << std::endl;

    flattenExpr(eval);
    eval = evaluateExpr(eval);
    if (gErrorManager.hasError())
        std::cout << gErrorManager.toString() << std::endl;
    
    std::cout << toInfixString(eval) << std::endl;
    freeExpression(eval);

    return 0; 
}