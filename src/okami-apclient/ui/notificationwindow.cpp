#include "notificationwindow.h"

#include <algorithm>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#ifdef _WIN32
#include "imgui.h"
#endif

namespace notificationwindow
{

namespace
{

struct Notification
{
    std::string text;
    float spawnTime;
    float duration;
};

// Thread-safe queue for incoming notifications
std::mutex g_queueMutex;
std::queue<Notification> g_pendingQueue;

// Active notifications being rendered (main thread only)
std::vector<Notification> g_activeNotifications;

} // namespace

void shutdown()
{
    // Clear any pending notifications
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        while (!g_pendingQueue.empty())
        {
            g_pendingQueue.pop();
        }
    }
    g_activeNotifications.clear();
}

void queue(const std::string &text, float duration)
{
    std::lock_guard<std::mutex> lock(g_queueMutex);
    g_pendingQueue.push(Notification{.text = text, .spawnTime = -1.0f, .duration = duration});
}

static void processQueue(float currentTime)
{
    std::lock_guard<std::mutex> lock(g_queueMutex);
    while (!g_pendingQueue.empty())
    {
        Notification notif = std::move(g_pendingQueue.front());
        g_pendingQueue.pop();
        notif.spawnTime = currentTime;
        g_activeNotifications.push_back(std::move(notif));
    }
}

static void removeExpired(float currentTime)
{
    g_activeNotifications.erase(std::remove_if(g_activeNotifications.begin(), g_activeNotifications.end(),
                                               [currentTime](const Notification &n) { return (currentTime - n.spawnTime) >= n.duration; }),
                                g_activeNotifications.end());
}

void draw(int outerWidth, int outerHeight)
{
#ifdef _WIN32
    float currentTime = static_cast<float>(ImGui::GetTime());

    // Move queued notifications to active list
    processQueue(currentTime);

    // Render active notifications
    float yOffset = 10.0f;
    int id = 0;

    for (auto &notif : g_activeNotifications)
    {
        float age = currentTime - notif.spawnTime;
        if (age >= notif.duration)
        {
            continue; // Skip expired (will be removed after loop)
        }

        // Calculate fade: full opacity for first 80% of duration, then fade out
        float fadeStart = notif.duration * 0.8f;
        float alpha = 1.0f;
        if (age > fadeStart)
        {
            alpha = 1.0f - ((age - fadeStart) / (notif.duration - fadeStart));
        }

        // Position in top-right corner
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(outerWidth) - 10.0f, yOffset), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.7f * alpha);

        std::string windowId = "##notif" + std::to_string(id++);
        ImGui::Begin(windowId.c_str(), nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);

        // Green text for item notifications
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, alpha), "%s", notif.text.c_str());

        // Get window height for stacking
        float windowHeight = ImGui::GetWindowSize().y;
        ImGui::End();

        yOffset += windowHeight + 5.0f; // Stack with 5px gap
    }

    // Remove expired notifications after rendering
    removeExpired(currentTime);
#endif
}

} // namespace notificationwindow
