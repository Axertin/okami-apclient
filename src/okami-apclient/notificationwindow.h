#pragma once

#include <string>

namespace notificationwindow
{

/**
 * @brief Cleanup Notification Window resources
 */
void shutdown();

/**
 * @brief Draw notification overlays
 *
 * Called by the GUI manager inside the ImGui frame.
 * Renders notification banners in the top-right corner.
 *
 * @param outerWidth Outer window width
 * @param outerHeight Outer window height
 */
void draw(int outerWidth, int outerHeight);

/**
 * @brief Queue a notification to be displayed
 *
 * Thread-safe: can be called from network thread.
 *
 * @param text The notification text to display
 * @param duration How long to display (seconds), default 5.0
 */
void queue(const std::string &text, float duration = 5.0f);

} // namespace notificationwindow
