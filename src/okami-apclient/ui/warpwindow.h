#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

namespace warpwindow
{

/**
 * @brief Initialize Warp Window state
 *
 * Call this once during mod initialization.
 */
void initialize();

/**
 * @brief Cleanup Warp Window resources
 */
void shutdown();

/**
 * @brief Draw Warp Window content
 *
 * Called by the GUI manager inside the ImGui frame.
 * Shows warp preset selection and manual coordinate entry.
 */
void draw();

/**
 * @brief Check if window is visible
 */
bool isVisible();

/**
 * @brief Set window visibility
 */
void setVisible(bool visible);

/**
 * @brief Toggle warp window visibility
 */
void toggle();

} // namespace warpwindow
