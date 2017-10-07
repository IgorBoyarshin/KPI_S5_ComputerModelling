#ifndef LOG_H
#define LOG_H

#include <ostream>
#include <iostream>
#include <string>

class Log {
    public:
        enum LogLevel {
            LogLevel_NONE, LogLevel_QUEUE, LogLevel_VERBOSE
        };

        Log();
        Log(LogLevel logLevel);

    private:
        LogLevel m_LogLevel;

    public:
        void setLogLevel(LogLevel newLogLevel);
        std::ostream& withLogLevel(LogLevel logLevel) const;

        void Queue(const std::ostream& ostream) const;
        void Verbose(const std::ostream& ostream) const;

        // operator std::ostream() {
        //     return std::cout;
        // }
        //
        // std::ostream& operator<<(const std::string& str);

};


// template<typename T>
// Log& operator<<(Log& log, const T& data);


#endif
