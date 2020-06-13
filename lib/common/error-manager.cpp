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
    mFatal.clear();
}

void ErrorManager::log(const std::string& msg, Type type, const char* file, int line)
{
    std::string more;
    if (strcmp(file, "") != 0 && line != 0)     more = msg + "\n    in " + std::string(file) + " at " + std::to_string(line);
    else if (strcmp(file, "") != 0)             more = msg + "\n    in " + std::string(file);
    else                                        more = msg;

    if (type == MESSAGE)          mMessages.push_back(more);
    else if (type == WARNING)     mWarnings.push_back(more);
    else if (type == FATAL)       mFatal.push_back("Fatal: " + more);
    else                          mErrors.push_back(more);
}

std::string ErrorManager::toString() const
{
    std::list<std::string> all;
    std::string ret;
    
    all.insert(all.begin(), mFatal.begin(), mFatal.end());
    all.insert(all.begin(), mErrors.begin(), mErrors.end());
    all.insert(all.begin(), mWarnings.begin(), mWarnings.end());
    all.insert(all.begin(), mMessages.begin(), mMessages.end());
    
    if (!all.empty())
    { 
        ret += all.front();
        for (auto it = std::next(all.begin()); it != all.end(); ++it)
            ret += ("\n" + *it);
    }

    return ret;
}

}