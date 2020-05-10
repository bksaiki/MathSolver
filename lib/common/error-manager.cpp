#include <cstring>
#include "error-manager.h"

namespace MathSolver
{

ErrorManager gErrorManager; // externed symbol

void ErrorManager::clear()
{   
    mMessages.clear();
    mWarnings.clear();
    mErrors.clear();
}

void ErrorManager::log(const std::string& msg, Type type, const char* file, int line)
{
    std::string more;
    if (strcmp(file, "") != 0 && line != 0)     more = msg + " in " + std::string(file) + " at " + std::to_string(line);
    else if (strcmp(file, "") != 0)             more = msg + " in " + std::string(file);
    else                                        more = msg;

    if (type == MESSAGE)          mMessages.push_back(more);
    else if (type == WARNING)     mWarnings.push_back(more);
    else if (type == FATAL)       mFatal.push_back("Fatal: " + more);
    else                          mErrors.push_back(more);
}

}