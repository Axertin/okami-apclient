#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

class ISocket;

namespace loginwindow
{

/**
 * @brief Initialize Login Window state (load saved connection data)
 *
 * Call this once during mod initialization.
 */
void initialize(ISocket &socket);

/**
 * @brief Cleanup Login Window resources
 */
void shutdown();

/**
 * @brief Draw Login Window content
 *
 * Called by the GUI manager inside the ImGui frame.
 * Shows connection form and status.
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
 * @brief Toggle login window visibility
 */
void toggle();

/**
 * @brief Get server connection string
 */
const char *getLoginServer();

/**
 * @brief Get slot name
 */
const char *getLoginSlot();

/**
 * @brief Get password
 */
const char *getLoginPassword();

} // namespace loginwindow
