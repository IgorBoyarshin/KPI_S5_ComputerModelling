#include "Log.h"

Log::Log() : m_LogLevel(LogLevel_NONE) {}
Log::Log(LogLevel logLevel) : m_LogLevel(logLevel) {}


void Log::setLogLevel(LogLevel newLogLevel) {
    m_LogLevel = newLogLevel;
}


std::ostream& Log::withLogLevel(LogLevel logLevel) const {
    std::ostream stream;
    return stream;
}

// void Log::Queue(const std::ostream& ostream) const {
//     std::cout << ostream;
// }


// template<typename T>
// Log& operator<<(Log& log, const T& data) {
//     std::cout << data;
//     return log;
// }
