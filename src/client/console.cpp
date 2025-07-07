#include "console.h"

#include <mutex>
#include <stdexcept>

#include <imgui.h>

#include "logger.h"

// Console Implementation - Handles UI rendering only

Console::Console() : Window("Console"), logger_(*g_Logger), autoScroll_(true), showTimestamps_(true)
{
    // Ensure logger is initialized
    if (g_Logger == nullptr)
    {
        throw std::runtime_error("Console created before Logger initialization. Call "
                                 "initializeLogger() first.");
    }

    // Initialize level filters - all enabled by default
    for (int i = 0; i < 4; ++i)
    {
        levelEnabled_[i] = true;
    }
}

Console::Console(Logger &logger) : Window("Console"), logger_(logger), autoScroll_(true), showTimestamps_(true)
{
    // Initialize level filters - all enabled by default
    for (int i = 0; i < 4; ++i)
    {
        levelEnabled_[i] = true;
    }
}

Console::~Console()
{
    // Logger cleanup is handled by Logger destructor
}

void Console::toggleVisibility()
{
    IsVisible = !IsVisible;
}

void Console::draw([[maybe_unused]] int OuterWidth, [[maybe_unused]] int OuterHeight, float UIScale)
{
    if (!IsVisible)
        return;

    ImGui::SetNextWindowSize(ImVec2(800 * UIScale, 400 * UIScale), ImGuiCond_FirstUseEver);

    if (ImGui::Begin(name.c_str(), &IsVisible))
    {
        // Simple toolbar
        if (ImGui::Button("Clear"))
        {
            logger_.clear();
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

void Console::drawLogOutput()
{
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        std::lock_guard<std::mutex> lock(logger_.getLogMutex());
        const auto &logEntries = logger_.getLogEntries();

        for (const auto &entry : logEntries)
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
