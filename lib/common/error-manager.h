#ifndef _MATHSOLVER_ERROR_MANAGER_H_
#define _MATHSOLVER_ERROR_MANAGER_H_

#include <list>
#include <string>

namespace MathSolver
{

// Handles internal messages, warnings and errors
class ErrorManager
{
public:

    // Error levels
    // MESSAGE - verbose mode or debugging
    // WARNING - may cause functions to return early depending on settings
    // MATH    - (math execution errors) may cause functions to return early depending on settings
    // ERROR   - (parsing, formatting errors) will almost always cause functions to return early
    // FATAL   - highest priority, will cause the application to stop
    enum Type
    {
        MESSAGE,
        WARNING,
        MATH,
        ERROR,
        FATAL
    };

public:

    // Default constructor
    inline ErrorManager() {}

    // Clears this error manager of any messages, warning, or errors. Fatal messages cannot be cleared
    void clear();

    // Returns a reference to the list of errors.
    inline std::list<std::string>& errors() { return mErrors; }
    inline const std::list<std::string>& errors() const { return mErrors; }

    // Returns a reference to the list of fatal error.
    inline std::list<std::string>& fatal() { return mFatal; }
    inline const std::list<std::string>& fatal() const { return mFatal; }

    // Returns true if this error manager has any error, message, or warning
    inline bool hasAny() const { return !mErrors.empty() || !mFatal.empty() || !mMessages.empty() || !mWarnings.empty(); }

    // Returns true if the error list is not empty.
    inline bool hasError() const { return !mErrors.empty(); }

    // Returns true if the fatal list is not empty.
    inline bool hasFatal() const { return !mFatal.empty(); }

    // Returns true if the message list is not empty.
    inline bool hasMessage() const { return !mMessages.empty(); }

    // Returns true if the warning list is not empty.
    inline bool hasWarning() const { return !mWarnings.empty(); }

    // Adds a message, warning, or error to this error manager.
    void log(const std::string& msg, Type type, const char* file = "", int line = 0);

    // Returns a reference to the list of messages.
    inline std::list<std::string>& messages() { return mMessages; }
    inline const std::list<std::string>& messages() const { return mMessages; }

    // Returns a reference to the list of warnings.
    inline std::list<std::string>& warnings() { return mWarnings; }
    inline const std::list<std::string>& warnings() const { return mWarnings; }

private:

    std::list<std::string> mMessages;
    std::list<std::string> mWarnings;
    std::list<std::string> mErrors;
    std::list<std::string> mFatal;
};

// Global error managing object
extern ErrorManager gErrorManager;

}

#endif