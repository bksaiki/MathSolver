#include <iostream>
#include <list>
#include "interpreter.h"

using namespace MathSolver;

int parseLine(const std::string& line)
{
    if (line == "exit" || line == "quit")  
        return 1;

    std::list<ExprNode*> tokens = tokenizeStr(line);
    ExprNode* eval = parseTokens(tokens);

    if (gErrorManager.hasError())
    {   
        for (ExprNode* e : tokens)  delete e;
        return 0;
    }

    flattenExpr(eval);
    eval = evaluateExpr(eval);
    std::cout << toInfixString(eval) << std::endl;

    return 0; 
}