#include "console.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// Global console instance
Console *g_Console = nullptr;

LogEntry::LogEntry(const std::string &msg, LogLevel lvl)
    : message(msg), level(lvl)
{
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    timestamp = oss.str();
}

ImVec4 LogEntry::getColor() const
{
    switch (level)
    {
    case LogLevel::Info:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // White
    case LogLevel::Warning:
        return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    case LogLevel::Error:
        return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
    case LogLevel::Debug:
        return ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Gray
    default:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

const char *LogEntry::getLevelString() const
{
    switch (level)
    {
    case LogLevel::Info:
        return "[INFO]";
    case LogLevel::Warning:
        return "[WARN]";
    case LogLevel::Error:
        return "[ERROR]";
    case LogLevel::Debug:
        return "[DEBUG]";
    default:
        return "[UNKNOWN]";
    }
}

StreamCapture::StreamCapture(std::ostream &stream, std::function<void(const std::string &, LogLevel)> callback, LogLevel level)
    : stream_(stream), callback_(callback), level_(level)
{
    old_buf_ = stream.rdbuf(this);
}

StreamCapture::~StreamCapture()
{
    stream_.rdbuf(old_buf_);
}

int StreamCapture::overflow(int c)
{
    if (c != EOF)
    {
        buffer_ += static_cast<char>(c);
        if (c == '\n')
        {
            if (!buffer_.empty() && buffer_.back() == '\n')
            {
                buffer_.pop_back(); // Remove newline for cleaner display
            }
            if (!buffer_.empty())
            {
                callback_(buffer_, level_);
                buffer_.clear();
            }
        }
    }
    return c;
}

Console::Console() : Window("Console"), autoScroll_(true), showTimestamps_(true)
{
    // Initialize level filters - all enabled by default
    for (int i = 0; i < 4; ++i)
    {
        levelEnabled_[i] = true;
    }

    initializeLogFile();
    setupStreamCapture();
}

Console::~Console()
{
    cleanupStreamCapture();
    if (logFile_.is_open())
    {
        logFile_.close();
    }
}

void Console::toggleVisibility()
{
    IsVisible = !IsVisible;
}

void Console::draw(int OuterWidth, int OuterHeight, float UIScale)
{
    if (!IsVisible)
        return;

    ImGui::SetNextWindowSize(ImVec2(800 * UIScale, 400 * UIScale), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(name.c_str(), &IsVisible))
    {
        // Simple toolbar
        if (ImGui::Button("Clear"))
        {
            clear();
        }

        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &autoScroll_);

        ImGui::SameLine();
        ImGui::Checkbox("Timestamps", &showTimestamps_);

        // Log level filters
        ImGui::SameLine();
        ImGui::Text("| Show: ");
        ImGui::SameLine();
        ImGui::Checkbox("Info", &levelEnabled_[0]);
        ImGui::SameLine();
        ImGui::Checkbox("Warning", &levelEnabled_[1]);
        ImGui::SameLine();
        ImGui::Checkbox("Error", &levelEnabled_[2]);
        ImGui::SameLine();
        ImGui::Checkbox("Debug", &levelEnabled_[3]);

        ImGui::Separator();

        drawLogOutput();
    }
    ImGui::End();
}

void Console::log(const std::string &message, LogLevel level)
{
    addLogEntry(message, level);
}

void Console::logInfo(const std::string &message)
{
    log(message, LogLevel::Info);
}

void Console::logWarning(const std::string &message)
{
    log(message, LogLevel::Warning);
}

void Console::logError(const std::string &message)
{
    log(message, LogLevel::Error);
}

void Console::logDebug(const std::string &message)
{
    log(message, LogLevel::Debug);
}

void Console::clear()
{
    std::lock_guard<std::mutex> lock(logMutex_);
    logEntries_.clear();
}

void Console::initializeLogFile()
{
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream filename;
    filename << "console_log_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".txt";

    logFile_.open(filename.str(), std::ios::out | std::ios::app);
    if (logFile_.is_open())
    {
        logFile_ << "=== Console Log Started ===" << std::endl;
    }
}

void Console::setupStreamCapture()
{
    auto captureCallback = [this](const std::string &msg, LogLevel level)
    {
        this->addLogEntry(msg, level);
    };

    stdoutCapture_ = std::make_unique<StreamCapture>(std::cout, captureCallback, LogLevel::Info);
    stderrCapture_ = std::make_unique<StreamCapture>(std::cerr, captureCallback, LogLevel::Error);
}

void Console::cleanupStreamCapture()
{
    stdoutCapture_.reset();
    stderrCapture_.reset();
}

void Console::addLogEntry(const std::string &message, LogLevel level)
{
    std::lock_guard<std::mutex> lock(logMutex_);

    logEntries_.emplace_back(message, level);

    // Write to file regardless of window visibility
    if (logFile_.is_open())
    {
        logFile_ << "[" << logEntries_.back().timestamp << "] "
                 << logEntries_.back().getLevelString() << " "
                 << message << std::endl;
        logFile_.flush();
    }

    // Keep log size manageable (trim old entries)
    if (logEntries_.size() > 10000)
    {
        logEntries_.erase(logEntries_.begin(), logEntries_.begin() + 1000);
    }
}

void Console::drawLogOutput()
{
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        std::lock_guard<std::mutex> lock(logMutex_);

        for (const auto &entry : logEntries_)
        {
            // Skip filtered out levels
            if (!levelEnabled_[static_cast<int>(entry.level)])
            {
                continue;
            }

            // Apply color based on log level
            ImGui::PushStyleColor(ImGuiCol_Text, entry.getColor());

            // Format display text
            std::string displayText;
            if (showTimestamps_)
            {
                displayText = "[" + entry.timestamp + "] " + entry.getLevelString() + " " + entry.message;
            }
            else
            {
                displayText = std::string(entry.getLevelString()) + " " + entry.message;
            }

            ImGui::TextUnformatted(displayText.c_str());
            ImGui::PopStyleColor();
        }

        // Auto-scroll to bottom if enabled and we're near the bottom
        if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
}

// Globals

void consoleLog(const std::string &message, LogLevel level)
{
    if (g_Console != nullptr)
    {
        g_Console->log(message, level);
    }
}

void consoleLogInfo(const std::string &message)
{
    if (g_Console != nullptr)
    {
        g_Console->logInfo(message);
    }
}

void consoleLogWarning(const std::string &message)
{
    if (g_Console != nullptr)
    {
        g_Console->logWarning(message);
    }
}

void consoleLogError(const std::string &message)
{
    if (g_Console != nullptr)
    {
        g_Console->logError(message);
    }
}

void consoleLogDebug(const std::string &message)
{
    if (g_Console != nullptr)
    {
        g_Console->logDebug(message);
    }
}
