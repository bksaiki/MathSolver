#include <iostream>
#include <list>
#include "interpreter.h"

using namespace MathSolver;

int parseLine(const std::string& line)
{
    if (line == "exit" || line == "quit")  
        return 1;

    ExprNode* eval = parseTokens(tokenizeStr(line));
    flattenExpr(eval);
    if(!evaluateExpr(eval))     std::cout << "Evaluation failed!" << std::endl;
    else                        std::cout << toInfixString(eval) << std::endl;

    return 0; 
}