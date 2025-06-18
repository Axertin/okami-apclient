#pragma once
#include "pch.h"
#include "window.h"

// Log levels for better organization
enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

// Structure to hold log entries
struct LogEntry
{
    std::string message;
    LogLevel level;
    std::string timestamp;

    LogEntry(const std::string &msg, LogLevel lvl);

    ImVec4 getColor() const;
    const char *getLevelString() const;
};

// Custom streambuf to capture stdout/stderr
class StreamCapture : public std::streambuf
{
public:
    StreamCapture(std::ostream &stream, std::function<void(const std::string &, LogLevel)> callback, LogLevel level);
    ~StreamCapture();

protected:
    virtual int overflow(int c) override;

private:
    std::ostream &stream_;
    std::streambuf *old_buf_;
    std::string buffer_;
    std::function<void(const std::string &, LogLevel)> callback_;
    LogLevel level_;
};

class Console : public Window
{
public:
    Console();
    ~Console();

    void toggleVisibility() override;
    void draw(int OuterWidth, int OuterHeight, float UIScale) override;

    // Core logging methods
    void log(const std::string &message, LogLevel level = LogLevel::Info);
    void logInfo(const std::string &message);
    void logWarning(const std::string &message);
    void logError(const std::string &message);
    void logDebug(const std::string &message);

    // Clear all logs
    void clear();

private:
    std::vector<LogEntry> logEntries_;
    std::mutex logMutex_;
    std::ofstream logFile_;

    // Stream capture
    std::unique_ptr<StreamCapture> stdoutCapture_;
    std::unique_ptr<StreamCapture> stderrCapture_;

    // UI state
    bool autoScroll_;
    bool showTimestamps_;
    bool levelEnabled_[4]; // Info, Warning, Error, Debug

    void initializeLogFile();
    void setupStreamCapture();
    void cleanupStreamCapture();
    void addLogEntry(const std::string &message, LogLevel level);
    void drawLogOutput();
};

extern Console *g_Console;

// global logging functions
void consoleLog(const std::string &message, LogLevel level = LogLevel::Info);
void consoleLogInfo(const std::string &message);
void consoleLogWarning(const std::string &message);
void consoleLogError(const std::string &message);
void consoleLogDebug(const std::string &message);
