#pragma once
#include <string>
#include <iostream>
#include <mutex>
#include <vector>
#include <functional>
#include <fstream>
#include <streambuf>
#include <cstdio>
#include <tuple>
#include <type_traits>
#include <filesystem>
#ifdef _WIN32
#include <imgui.h>
#else
struct ImVec4
{
    float x, y, z, w;
    constexpr ImVec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    constexpr ImVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};
#endif

enum class LogLevel
{
    Info = 0,
    Warning = 1,
    Error = 2,
    Debug = 3
};

struct LogEntry
{
    std::string message;
    std::string timestamp;
    LogLevel level;

    LogEntry(const std::string &msg, LogLevel lvl);
    ImVec4 getColor() const;
    const char *getLevelString() const;
};

class StreamCapture : public std::streambuf
{
public:
    StreamCapture(std::ostream &stream, std::function<void(const std::string &, LogLevel)> callback, LogLevel level);
    ~StreamCapture();

protected:
    int overflow(int c) override;

private:
    std::ostream &stream_;
    std::streambuf *old_buf_;
    std::string buffer_;
    std::function<void(const std::string &, LogLevel)> callback_;
    LogLevel level_;
};

// Logger - Handles data storage and file logging
class Logger
{
public:
    Logger();
    ~Logger();

    void log(const std::string &message, LogLevel level);
    void logInfo(const std::string &message);
    void logWarning(const std::string &message);
    void logError(const std::string &message);
    void logDebug(const std::string &message);
    void clear();

    // Access for UI
    const std::vector<LogEntry> &getLogEntries() const;
    std::mutex &getLogMutex() const;

private:
    void initializeLogFile();
    void setupStreamCapture();
    void cleanupStreamCapture();
    void addLogEntry(const std::string &message, LogLevel level);

    std::vector<LogEntry> logEntries_;
    mutable std::mutex logMutex_;
    std::ofstream logFile_;

    std::unique_ptr<StreamCapture> stdoutCapture_;
    std::unique_ptr<StreamCapture> stderrCapture_;
};

// Global function declarations
void logMessage(const std::string &message, LogLevel level);
void logInfo(const std::string &message);
void logWarning(const std::string &message);
void logError(const std::string &message);
void logDebug(const std::string &message);

// Format String Helper Function

// Helper to convert arguments to C-compatible types
template <typename T>
auto toCArg(T &&arg) -> decltype(auto)
{
    if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
    {
        return arg.c_str();
    }
    else
    {
        return std::forward<T>(arg);
    }
}

// Wrapper functions to suppress warnings at the call site
template <typename... Args>
int snprintf_wrapper(char *buffer, size_t size, const char *format, Args &&...args)
{
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#endif
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // MSVC format security warning
#endif

    int result = std::snprintf(buffer, size, format, std::forward<Args>(args)...);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif

    return result;
}

template <typename... Args>
std::string formatString(const char *format, Args... args)
{
    // Convert arguments to C-compatible types
    auto convertedArgs = std::make_tuple(toCArg(args)...);

    // Calculate the size needed for the formatted string
    int size = std::apply([format](auto &&...convertedArgs)
                          { return snprintf_wrapper(nullptr, 0, format, convertedArgs...); }, convertedArgs) +
               1; // +1 for null terminator

    if (size <= 1)
    {
        return std::string(format); // Return original format string if formatting fails
    }

    // Create buffer and format the string
    std::unique_ptr<char[]> buf(new char[size]);
    std::apply([format, &buf, size](auto &&...convertedArgs)
               { snprintf_wrapper(buf.get(), size, format, convertedArgs...); }, convertedArgs);

    return std::string(buf.get(), buf.get() + size - 1); // -1 to exclude null terminator
}

// Format string variadic templates
template <typename... Args>
void logMessage(LogLevel level, const char *format, Args... args)
{
    extern Logger *g_Logger;
    if (g_Logger != nullptr)
    {
        g_Logger->log(formatString(format, args...), level);
    }
}

template <typename... Args>
void logInfo(const char *format, Args... args)
{
    logMessage(LogLevel::Info, format, args...);
}

template <typename... Args>
void logWarning(const char *format, Args... args)
{
    logMessage(LogLevel::Warning, format, args...);
}

template <typename... Args>
void logError(const char *format, Args... args)
{
    logMessage(LogLevel::Error, format, args...);
}

template <typename... Args>
void logDebug(const char *format, Args... args)
{
    logMessage(LogLevel::Debug, format, args...);
}

// Convenience hex logging functions
template <typename T>
void logInfoHex(const char *prefix, T value)
{
    logInfo("%s0x%X", prefix, value);
}

template <typename T>
void logWarningHex(const char *prefix, T value)
{
    logWarning("%s0x%X", prefix, value);
}

template <typename T>
void logErrorHex(const char *prefix, T value)
{
    logError("%s0x%X", prefix, value);
}

template <typename T>
void logDebugHex(const char *prefix, T value)
{
    logDebug("%s0x%X", prefix, value);
}

// Initialization functions
void initializeLogger();
void shutdownLogger();

// Global instance
extern Logger *g_Logger;
