#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

class ISocket;

namespace loginwindow
{

/**
 * @brief Initialize Login Window GUI system
 *
 * Sets up ImGui integration and registers GUI callback with WOLF framework.
 * Call this once during mod initialization.
 */
void initialize(ISocket &socket);

/**
 * @brief Cleanup Login Window resources
 */
void shutdown();

/**
 * @brief Render Login Window
 *
 * Called automatically by WOLF's GUI system each frame.
 * Shows connection form and status.
 *
 * @param width Outer window width
 * @param height Outer window height
 * @param scale UI scale factor
 */
void render(int width, int height, float scale);

/**
 * @brief Show the login window
 */
void show();

/**
 * @brief Hide the login window
 */
void hide();

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
