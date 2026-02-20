/**
 * @file wolf_framework.hpp
 * @brief WOLF Okami Loader Framework - Complete Single-Header Distribution
 *
 * This file contains the complete Wolf API flattened into a single header
 * for easy distribution and integration with mod projects.
 *
 * Generated automatically - do not edit directly.
 * Edit the source templates in src/api/ instead.
 *
 * Build: Debug (Clang 20.1.0)
 *
 * MIT License
 *
 * Copyright (c) 2025 WOLF Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

// Core runtime API (uses function table)

// === Expanded from wolf_core.hpp ===
#include <atomic>
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#if defined(_MSVC_LANG)
#if _MSVC_LANG < 201703L
#error "This header requires C++17 or later"
#endif
#elif __cplusplus < 201703L
#error "This header requires C++17 or later"
#endif

#define WOLF_VERSION_MAJOR 0
#define WOLF_VERSION_MINOR 2
#define WOLF_VERSION_PATCH 0
#define WOLF_VERSION_PRERELEASE "beta.2"
#define WOLF_VERSION_IS_PRERELEASE TRUE
#define WOLF_VERSION_BUILD ""
#define WOLF_VERSION_STRING "0.2.0-beta.2"

#define WOLF_VERSION_INT ((0U << 24) | (2U << 16) | 0U)

#define WOLF_BUILD_TYPE "Debug"
#define WOLF_COMPILER "Clang 20.1.0"

//==============================================================================
// C RUNTIME API DECLARATIONS
// These function pointers are provided by the runtime via function table injection
//==============================================================================

// === Expanded from wolf_function_table.h ===

// === Expanded from wolf_types.h ===
#include <stddef.h>
#include <stdint.h>

extern "C"
{
    //==============================================================================
    // FUNDAMENTAL TYPES
    //==============================================================================

    /**
     * @brief Unique identifier for loaded mods
     *
     * Each mod receives a unique WolfModId when successfully registered with the
     * Wolf runtime. This ID is used to track mod ownership of resources, callbacks,
     * and other runtime objects.
     *
     * @note ID 0 is reserved and indicates an invalid or uninitialized mod ID
     */
    typedef int WolfModId;

    /**
     * @brief Handle for bitfield monitoring objects
     *
     * Opaque handle used to reference bitfield monitors created through the
     * Wolf runtime API. Used for tracking memory changes in game data structures.
     */
    typedef struct WolfBitfieldMonitor *WolfBitfieldMonitorHandle;

    //==============================================================================
    // LOGGING SYSTEM
    //==============================================================================

    /**
     * @brief Log message severity levels
     *
     * Defines the severity levels available for logging messages through the
     * Wolf framework. Higher values indicate more severe messages.
     */
    typedef enum WolfLogLevel
    {
        WOLF_LOG_DEBUG = 3,   /**< Debug information, lowest priority */
        WOLF_LOG_INFO = 0,    /**< General information messages */
        WOLF_LOG_WARNING = 1, /**< Warning messages for potential issues */
        WOLF_LOG_ERROR = 2    /**< Error messages for serious problems */
    } WolfLogLevel;

    //==============================================================================
    // CALLBACK FUNCTION TYPES
    //==============================================================================

    /**
     * @brief Generic game event callback function
     *
     * Called when specific game events occur (game start, stop, tick, etc.).
     *
     * @param userdata User-provided data passed during callback registration
     */
    typedef void(__cdecl *WolfGameEventCallback)(void *userdata);

    /**
     * @brief Item pickup event callback function
     *
     * Called when the player picks up an item in the game.
     *
     * @param item_id Unique identifier of the picked up item
     * @param count Number of items picked up
     * @param userdata User-provided data passed during callback registration
     */
    typedef void(__cdecl *WolfItemPickupCallback)(int item_id, int count, void *userdata);

    /**
     * @brief Blocking item pickup callback function
     *
     * Called when the player picks up items. Return 1 to block the pickup, 0 to allow.
     *
     * @param item_id Unique identifier of the picked up item
     * @param count Number of items picked up
     * @param userdata User-provided data passed during callback registration
     * @return 1 to block the pickup, 0 to allow
     */
    typedef int(__cdecl *WolfItemPickupBlockingCallback)(int item_id, int count, void *userdata);

    /**
     * @brief Brush edit callback function
     *
     * Called when a brush is edited. Return 1 to block the edit, 0 to allow.
     *
     * @param bit_index Index of the bit being edited
     * @param operation Edit operation being performed
     * @param userdata User-provided data passed during callback registration
     * @return 1 to block the edit, 0 to allow
     */
    typedef int(__cdecl *WolfBrushEditCallback)(int bit_index, int operation, void *userdata);

    /**
     * @brief Memory pattern search callback function
     *
     * Called for each match found during memory pattern searching.
     *
     * @param address Memory address where the pattern was found
     * @param userdata User-provided data passed during pattern search
     */
    typedef void(__cdecl *WolfPatternCallback)(uintptr_t address, void *userdata);

    /**
     * @brief Memory watch callback function
     *
     * Called when monitored memory region changes.
     *
     * @param address Base address of the changed memory region
     * @param old_data Pointer to the old data (size determined by watch region)
     * @param new_data Pointer to the new data (size determined by watch region)
     * @param size Number of bytes that changed
     * @param userdata User-provided data passed during watch registration
     */
    typedef void(__cdecl *WolfMemoryWatchCallback)(uintptr_t address, const void *old_data, const void *new_data, size_t size, void *userdata);

    /**
     * @brief Console command callback function
     *
     * Called when a registered console command is executed.
     *
     * @param argc Number of arguments (including command name)
     * @param argv Array of argument strings
     * @param userdata User-provided data passed during command registration
     */
    typedef void(__cdecl *WolfConsoleCommandCallback)(int argc, const char **argv, void *userdata);

    /**
     * @brief Resource provider callback function
     *
     * Called to provide custom resource data when intercepting game resources.
     *
     * @param original_path Path to the original resource file
     * @param userdata User-provided data passed during interception registration
     * @return Path to replacement resource, or NULL to use original
     */
    typedef const char *(__cdecl *WolfResourceProvider)(const char *original_path, void *userdata);

    /**
     * @brief Bitfield change callback function
     *
     * Called when monitored bitfield data changes.
     *
     * @param bit_index Index of the bit that changed (0-based)
     * @param old_value Previous value of the bit (0 or 1)
     * @param new_value New value of the bit (0 or 1)
     * @param userdata User-provided data passed during monitor creation
     */
    typedef void(__cdecl *WolfBitfieldChangeCallback)(unsigned int bit_index, int old_value, int new_value, void *userdata);

    /**
     * @brief GUI window rendering callback function
     *
     * Called each frame to render a mod's GUI window. ImGui context is automatically
     * available during callback execution.
     *
     * @param outer_width Current window width in pixels
     * @param outer_height Current window height in pixels
     * @param ui_scale UI scaling factor (1.0 = normal, higher = larger)
     * @param userdata User-provided data passed during window registration
     */
    typedef void(__cdecl *WolfGuiWindowCallback)(int outer_width, int outer_height, float ui_scale, void *userdata);

    //==============================================================================
    // MOD INTERFACE STRUCTURE
    //==============================================================================

    /**
     * @brief Mod interface structure (ABI-stable)
     *
     * Structure that mods must implement to be loaded by the Wolf runtime.
     * Field order is critical - matches macro expectations.
     */
    typedef struct WolfModInterface
    {
        /**
         * @brief Early game initialization callback (optional)
         *
         * Called during early mod loading phase. Can be NULL if not needed.
         */
        void(__cdecl *earlyGameInit)(void);

        /**
         * @brief Late game initialization callback (optional)
         *
         * Called after GUI initialization is complete. Useful for mods that
         * need to register GUI windows. Can be NULL if not needed.
         */
        void(__cdecl *lateGameInit)(void);

        /**
         * @brief Mod shutdown callback (required)
         *
         * Called when the mod is being unloaded. Should clean up all resources.
         * Runtime API may not be available during this call.
         */
        void(__cdecl *shutdown)(void);

        /**
         * @brief Get mod name callback (required)
         *
         * Must return a static string containing the mod's name.
         *
         * @return Null-terminated mod name string
         */
        const char *(__cdecl *getName)(void);

        /**
         * @brief Get mod version callback (optional)
         *
         * Returns the mod's version string. Can be NULL if not implemented.
         *
         * @return Null-terminated version string, or NULL
         */
        const char *(__cdecl *getVersion)(void);

        /**
         * @brief Framework version this mod was compiled against
         *
         * Used for compatibility checking. Should match the framework version
         * the mod was developed with (WOLF_VERSION_INT).
         */
        unsigned int frameworkVersionInt;

        /**
         * @brief ImGui version integer
         *
         * Used for ImGui compatibility checking. Should match the ImGui version
         * the mod was compiled with (IMGUI_VERSION_NUM).
         */
        unsigned int imguiVersionInt;
    } WolfModInterface;

    /**
     * @brief Callback function type for Win32 window procedure hooks
     *
     * @param hwnd Window handle
     * @param msg Window message
     * @param wParam Message parameter
     * @param lParam Message parameter
     * @param userData User data provided during registration
     * @return Non-zero if the message was handled and should not be processed further
     */
    typedef int(__cdecl *WolfWndProcCallback)(void *hwnd, unsigned int msg, uintptr_t wParam, intptr_t lParam, void *userData);
}

// === End of wolf_types.h ===

extern "C"
{
    //==============================================================================
    // WOLF RUNTIME API STRUCTURE
    //==============================================================================

    /**
     * @brief Complete Wolf Runtime API Function Table
     *
     * This structure contains function pointers to all services provided by the
     * Wolf runtime. It is passed to mods during initialization and provides the
     * complete interface for mod-runtime interaction.
     *
     * @attention Insertions to or reordering of this struct constitutes a BREAKING CHANGE. Unless this is your intention, always append to this struct.
     *
     * @note Function pointers should never be NULL when the API is properly initialized
     * @note All functions use __cdecl calling convention for cross-compiler compatibility
     */
    typedef struct WolfRuntimeAPI
    {
        //==========================================================================
        // VERSION INFORMATION SYSTEM
        //==========================================================================

        /**
         * @brief Get Wolf runtime version string
         *
         * Returns the version of the Wolf runtime currently loaded.
         *
         * @return Null-terminated version string (e.g., "1.0.0")
         */
        const char *(__cdecl *getRuntimeVersion)(void);

        /**
         * @brief Get Wolf runtime build information
         *
         * Returns detailed build information including commit hash, build date, etc.
         *
         * @return Null-terminated build information string
         */
        const char *(__cdecl *getRuntimeBuildInfo)(void);

        //==========================================================================
        // MOD LIFECYCLE MANAGEMENT
        //==========================================================================

        /**
         * @brief Get current mod ID for the calling thread
         *
         * Returns the WolfModId of the mod that is currently executing on this thread.
         * Useful for debugging and context tracking.
         *
         * @return Current mod's unique identifier, or 0 if no mod context
         */
        WolfModId(__cdecl *getCurrentModId)(void);

        /**
         * @brief Register a new mod with the runtime
         *
         * Called during mod loading to register the mod interface and obtain a unique ID.
         * This function is typically called automatically during mod loading.
         *
         * @param modInterface Pointer to the mod's interface structure
         * @return Unique mod ID on success, 0 on failure
         */
        WolfModId(__cdecl *registerMod)(const WolfModInterface *modInterface);

        //==========================================================================
        // LOGGING SYSTEM
        //==========================================================================

        /**
         * @brief Log a message with specified severity level
         *
         * Outputs a message to the Wolf logging system with the specified severity.
         * Messages are automatically prefixed with the mod's identifier.
         *
         * @param mod_id ID of the mod sending the message
         * @param level Severity level of the message
         * @param message Null-terminated message string
         */
        void(__cdecl *log)(WolfModId mod_id, WolfLogLevel level, const char *message);

        /**
         * @brief Set custom log prefix for a mod
         *
         * Allows mods to customize their log message prefix instead of using
         * the default "[ModName]" format.
         *
         * @param mod_id ID of the mod setting the prefix
         * @param prefix New prefix string to use for log messages
         */
        void(__cdecl *setLogPrefix)(WolfModId mod_id, const char *prefix);

        //==========================================================================
        // MEMORY ACCESS AND PATTERN SEARCHING
        //==========================================================================

        /**
         * @brief Get base address of a loaded module
         *
         * Retrieves the base memory address where the specified module is loaded.
         * Useful for calculating offsets and hooking functions.
         *
         * @param module_name Name of the module (e.g., "game.exe")
         * @return Base address of the module, or 0 if not found
         */
        uintptr_t(__cdecl *getModuleBase)(const char *module_name);

        /**
         * @brief Check if a memory address is valid for access
         *
         * Verifies that the given address can be safely read from or written to.
         *
         * @param address Memory address to check
         * @return 1 if address is valid, 0 otherwise
         */
        int(__cdecl *isValidAddress)(uintptr_t address);

        /**
         * @brief Read memory from the target process
         *
         * Safely reads memory from the specified address into the provided buffer.
         *
         * @param address Source memory address
         * @param buffer Destination buffer to store read data
         * @param size Number of bytes to read
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *readMemory)(uintptr_t address, void *buffer, size_t size);

        /**
         * @brief Write memory to the target process
         *
         * Safely writes data from the provided buffer to the specified memory address.
         *
         * @param address Destination memory address
         * @param buffer Source buffer containing data to write
         * @param size Number of bytes to write
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *writeMemory)(uintptr_t address, const void *buffer, size_t size);

        /**
         * @brief Search for byte patterns in memory
         *
         * Searches for the specified byte pattern within a module's memory space.
         * The callback is invoked for each match found.
         *
         * @param pattern Byte pattern to search for (hex string format)
         * @param mask Mask string specifying which bytes to match ('x' = match, '?' = wildcard)
         * @param module_name Name of module to search in, or NULL for all modules
         * @param callback Function called for each pattern match
         * @param userdata User data passed to the callback
         */
        void(__cdecl *findPattern)(const char *pattern, const char *mask, const char *module_name, WolfPatternCallback callback, void *userdata);

        /**
         * @brief Monitor memory region for changes
         *
         * Sets up monitoring for a memory region, calling the callback whenever
         * the memory content changes.
         *
         * @param mod_id ID of the mod setting up the watch
         * @param start Starting address of the memory region
         * @param size Size of the memory region in bytes
         * @param callback Function called when memory changes
         * @param userdata User data passed to the callback
         * @param description Human-readable description of what's being watched
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *watchMemory)(WolfModId mod_id, uintptr_t start, size_t size, WolfMemoryWatchCallback callback, void *userdata, const char *description);

        /**
         * @brief Stop monitoring a memory region
         *
         * Removes memory monitoring set up by a previous watchMemory call.
         *
         * @param mod_id ID of the mod that set up the watch
         * @param start Starting address of the watched region
         * @return 1 on success, 0 if no watch was found
         */
        int(__cdecl *unwatchMemory)(WolfModId mod_id, uintptr_t start);

        //==========================================================================
        // GAME HOOKS AND EVENT CALLBACKS
        //==========================================================================

        /**
         * @brief Register callback for game tick events
         *
         * Called every game frame/tick. Useful for continuous monitoring or updates.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call on each game tick
         * @param userdata User data passed to the callback
         */
        void(__cdecl *registerGameTick)(WolfModId mod_id, WolfGameEventCallback callback, void *userdata);

        /**
         * @brief Register callback for game start events
         *
         * Called when the game begins (e.g., new game, load game).
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when game starts
         * @param userdata User data passed to the callback
         */
        void(__cdecl *registerGameStart)(WolfModId mod_id, WolfGameEventCallback callback, void *userdata);

        /**
         * @brief Register callback for game stop events
         *
         * Called when the game ends or is paused.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when game stops
         * @param userdata User data passed to the callback
         */
        void(__cdecl *registerGameStop)(WolfModId mod_id, WolfGameEventCallback callback, void *userdata);

        /**
         * @brief Register callback for play start events
         *
         * Called when active gameplay begins.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when play starts
         * @param userdata User data passed to the callback
         */
        void(__cdecl *registerPlayStart)(WolfModId mod_id, WolfGameEventCallback callback, void *userdata);

        /**
         * @brief Register callback for return to menu events
         *
         * Called when the player returns to the main menu.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when returning to menu
         * @param userdata User data passed to the callback
         */
        void(__cdecl *registerReturnToMenu)(WolfModId mod_id, WolfGameEventCallback callback, void *userdata);

        /**
         * @brief Register callback for item pickup events
         *
         * Called when the player picks up an item in the game.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when item is picked up
         * @param userdata User data passed to the callback
         */
        void(__cdecl *registerItemPickup)(WolfModId mod_id, WolfItemPickupCallback callback, void *userdata);

        /**
         * @brief Register blocking callback for item pickup events
         *
         * Called when the player picks up an item in the game. Return 1 to block the pickup.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when item is picked up (return 1 to block)
         * @param userdata User data passed to the callback
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *registerItemPickupBlocking)(WolfModId mod_id, WolfItemPickupBlockingCallback callback, void *userdata);

        /**
         * @brief Register callback for brush edit events
         *
         * Called when a brush is edited. Return 1 to block the edit.
         *
         * @param mod_id ID of the mod registering the callback
         * @param callback Function to call when brush is edited (return 1 to block)
         * @param userdata User data passed to the callback
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *registerBrushEdit)(WolfModId mod_id, WolfBrushEditCallback callback, void *userdata);

        /**
         * @brief Install function hook
         *
         * Installs a hook at the specified address, redirecting calls to the detour function.
         *
         * @param address Address of the function to hook
         * @param detour Address of the replacement function
         * @param original Pointer to store the original function address
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *hookFunction)(uintptr_t address, void *detour, void **original);

        //==========================================================================
        // CONSOLE COMMAND SYSTEM
        //==========================================================================

        /**
         * @brief Register a new console command
         *
         * Adds a new command to the developer console that mods can implement.
         *
         * @param mod_id ID of the mod registering the command
         * @param name Command name (without prefix)
         * @param callback Function to call when command is executed
         * @param userdata User data passed to the callback
         * @param description Help text describing the command
         */
        void(__cdecl *addCommand)(WolfModId mod_id, const char *name, WolfConsoleCommandCallback callback, void *userdata, const char *description);

        /**
         * @brief Remove a console command
         *
         * Removes a previously registered console command.
         *
         * @param mod_id ID of the mod that registered the command
         * @param name Name of the command to remove
         */
        void(__cdecl *removeCommand)(WolfModId mod_id, const char *name);

        /**
         * @brief Execute a console command
         *
         * Programmatically executes a console command as if it was typed by the user.
         *
         * @param command_line Complete command line to execute
         */
        void(__cdecl *executeCommand)(const char *command_line);

        /**
         * @brief Print message to console
         *
         * Displays a message in the developer console.
         *
         * @param message Message to display in the console
         */
        void(__cdecl *consolePrint)(const char *message);

        /**
         * @brief Check if console is currently visible
         *
         * Determines whether the developer console is currently open and visible.
         *
         * @return 1 if console is visible, 0 otherwise
         */
        int(__cdecl *isConsoleVisible)(void);

        //==========================================================================
        // RESOURCE INTERCEPTION SYSTEM
        //==========================================================================

        /**
         * @brief Intercept resource file loading
         *
         * Registers a provider to supply custom data when the game requests a specific resource file.
         *
         * @param mod_id ID of the mod registering the interception
         * @param filename Name of the resource file to intercept
         * @param provider Function that provides the resource data
         * @param userdata User data passed to the provider function
         */
        void(__cdecl *interceptResource)(WolfModId mod_id, const char *filename, WolfResourceProvider provider, void *userdata);

        /**
         * @brief Remove resource interception
         *
         * Removes a previously registered resource interception.
         *
         * @param mod_id ID of the mod that registered the interception
         * @param filename Name of the resource file to stop intercepting
         */
        void(__cdecl *removeResourceInterception)(WolfModId mod_id, const char *filename);

        /**
         * @brief Intercept resource files matching a pattern
         *
         * Registers a provider for multiple resource files matching a wildcard pattern.
         *
         * @param mod_id ID of the mod registering the interception
         * @param pattern Wildcard pattern for filenames to intercept
         * @param provider Function that provides the resource data
         * @param userdata User data passed to the provider function
         */
        void(__cdecl *interceptResourcePattern)(WolfModId mod_id, const char *pattern, WolfResourceProvider provider, void *userdata);

        //==========================================================================
        // BITFIELD MONITORING SYSTEM
        //==========================================================================

        /**
         * @brief Create bitfield monitor for absolute address
         *
         * Sets up monitoring of bitfield data at a specific memory address.
         *
         * @param mod_id ID of the mod creating the monitor
         * @param address Absolute memory address to monitor
         * @param size_in_bytes Size of the bitfield data in bytes
         * @param callback Function called when bitfield changes
         * @param userdata User data passed to the callback
         * @param description Human-readable description of the bitfield
         * @return Handle to the monitor, or NULL on failure
         */
        WolfBitfieldMonitorHandle(__cdecl *createBitfieldMonitor)(WolfModId mod_id, uintptr_t address, size_t size_in_bytes,
                                                                  WolfBitfieldChangeCallback callback, void *userdata, const char *description);

        /**
         * @brief Create bitfield monitor for module-relative address
         *
         * Sets up monitoring of bitfield data at an offset within a specific module.
         *
         * @param mod_id ID of the mod creating the monitor
         * @param module_name Name of the module containing the bitfield
         * @param offset Offset from the module base address
         * @param size_in_bytes Size of the bitfield data in bytes
         * @param callback Function called when bitfield changes
         * @param userdata User data passed to the callback
         * @param description Human-readable description of the bitfield
         * @return Handle to the monitor, or NULL on failure
         */
        WolfBitfieldMonitorHandle(__cdecl *createBitfieldMonitorModule)(WolfModId mod_id, const char *module_name, uintptr_t offset, size_t size_in_bytes,
                                                                        WolfBitfieldChangeCallback callback, void *userdata, const char *description);

        /**
         * @brief Destroy a bitfield monitor
         *
         * Removes monitoring and frees resources associated with the monitor handle.
         *
         * @param monitor Handle to the monitor to destroy
         */
        void(__cdecl *destroyBitfieldMonitor)(WolfBitfieldMonitorHandle monitor);

        /**
         * @brief Manually update a bitfield monitor
         *
         * Forces a check for changes in the monitored bitfield data.
         *
         * @param monitor Handle to the monitor to update
         * @return 1 if changes were detected, 0 otherwise
         */
        int(__cdecl *updateBitfieldMonitor)(WolfBitfieldMonitorHandle monitor);

        /**
         * @brief Reset bitfield monitor baseline
         *
         * Updates the monitor's baseline value to the current bitfield state.
         *
         * @param monitor Handle to the monitor to reset
         * @return 1 on success, 0 on failure
         */
        int(__cdecl *resetBitfieldMonitor)(WolfBitfieldMonitorHandle monitor);

        //==========================================================================
        // GUI WINDOW MANAGEMENT
        //==========================================================================

        /**
         * @brief Register a GUI window for rendering
         *
         * Creates a new ImGui window that will be rendered each frame. The callback
         * is executed with proper ImGui context automatically available.
         *
         * @param mod_id ID of the mod registering the window
         * @param window_name Unique name for the window
         * @param callback Function called each frame to render the window
         * @param userdata User data passed to the callback
         * @param initially_visible Whether the window should start visible (1) or hidden (0)
         * @return 1 on success, 0 on failure (e.g., name already registered)
         */
        int(__cdecl *registerGuiWindow)(WolfModId mod_id, const char *window_name, WolfGuiWindowCallback callback, void *userdata, int initially_visible);

        /**
         * @brief Unregister a GUI window
         *
         * Removes a previously registered GUI window and stops rendering it.
         *
         * @param mod_id ID of the mod that registered the window
         * @param window_name Name of the window to unregister
         * @return 1 on success, 0 if window not found
         */
        int(__cdecl *unregisterGuiWindow)(WolfModId mod_id, const char *window_name);

        /**
         * @brief Toggle GUI window visibility
         *
         * Toggles the visibility state of a registered GUI window.
         *
         * @param mod_id ID of the mod that owns the window
         * @param window_name Name of the window to toggle
         * @return 1 on success, 0 if window not found
         */
        int(__cdecl *toggleGuiWindow)(WolfModId mod_id, const char *window_name);

        /**
         * @brief Set GUI window visibility
         *
         * Sets the visibility state of a registered GUI window.
         *
         * @param mod_id ID of the mod that owns the window
         * @param window_name Name of the window to modify
         * @param visible 1 to show the window, 0 to hide it
         * @return 1 on success, 0 if window not found
         */
        int(__cdecl *setGuiWindowVisible)(WolfModId mod_id, const char *window_name, int visible);

        /**
         * @brief Execute function within ImGui context
         *
         * Executes the provided function with the Wolf runtime's ImGui context active.
         * This ensures that ImGui functions called within the callback have access to
         * a valid ImGui context. Primarily useful for advanced scenarios where ImGui
         * code needs to run outside of registered window callbacks.
         *
         * @param mod_id ID of the calling mod
         * @param renderFunc Function to execute with ImGui context active
         * @param userdata User data passed to the render function
         * @return 1 on success, 0 on failure (invalid context or function)
         */
        int(__cdecl *executeInImGuiContext)(WolfModId mod_id, void(__cdecl *renderFunc)(void *userdata), void *userdata);

        /**
         * @brief Get Wolf runtime's ImGui context pointer
         *
         * Returns the Wolf runtime's ImGui context pointer that mods can use to set
         * their own ImGui context. This is needed because each DLL has its own ImGui
         * library instance with separate global state.
         *
         * @return ImGui context pointer, or NULL if ImGui is not initialized
         *
         * @note Mods should call ImGui::SetCurrentContext() with this pointer in their
         *       GUI callbacks to access the Wolf runtime's ImGui context
         */
        void *(__cdecl *getImGuiContext)(void);

        /**
         * @brief Get Wolf's ImGui memory allocator function
         *
         * Returns the memory allocation function used by Wolf's ImGui instance.
         * Mods must call ImGui::SetAllocatorFunctions() with this allocator to avoid
         * heap corruption when sharing ImGui contexts across DLL boundaries.
         *
         * @return Memory allocator function pointer
         */
        void *(__cdecl *getImGuiAllocFunc)(void);

        /**
         * @brief Get Wolf's ImGui memory free function
         *
         * Returns the memory free function used by Wolf's ImGui instance.
         * Mods must call ImGui::SetAllocatorFunctions() with this free function to avoid
         * heap corruption when sharing ImGui contexts across DLL boundaries.
         *
         * @return Memory free function pointer
         */
        void *(__cdecl *getImGuiFreeFunc)(void);

        /**
         * @brief Get Wolf's ImGui allocator user data
         *
         * Returns the user data pointer used by Wolf's ImGui allocator functions.
         *
         * @return User data pointer for allocator functions
         */
        void *(__cdecl *getImGuiAllocUserData)(void);

        /**
         * @brief Get Wolf's ImGui font atlas
         *
         * Returns Wolf's shared ImFontAtlas instance. Mods must use this shared
         * font atlas to prevent GPU/rendering conflicts during styled text rendering.
         * Without shared font atlas, styled text operations (like ImGui::PushStyleColor() + Text())
         * can cause crashes due to separate font texture data.
         *
         * @return Pointer to Wolf's ImFontAtlas, or NULL if not available
         */
        void *(__cdecl *getImGuiFontAtlas)(void);

        /**
         * @brief Get Wolf runtime's ImGui IO configuration
         *
         * Returns Wolf's ImGuiIO configuration that mods should copy when creating their own
         * ImGui contexts. This is critical for preventing assertion failures in ImGui.
         * Mods must copy essential fields like DisplaySize, DeltaTime, etc. to their own context.
         *
         * @return Pointer to Wolf's ImGuiIO, or NULL if not available
         */
        void *(__cdecl *getImGuiIO)(void);

        /**
         * @brief Get Wolf runtime's D3D11 device for mod backend initialization
         *
         * Returns Wolf's ID3D11Device that mods should use when initializing their own
         * ImGui D3D11 backends. This allows multiple backends to share the same device.
         *
         * @return Pointer to Wolf's ID3D11Device, or NULL if not available
         */
        void *(__cdecl *getD3D11Device)(void);

        /**
         * @brief Get Wolf runtime's D3D11 device context for mod backend initialization
         *
         * Returns Wolf's ID3D11DeviceContext that mods should use when initializing their own
         * ImGui D3D11 backends. This allows multiple backends to share the same device context.
         *
         * @return Pointer to Wolf's ID3D11DeviceContext, or NULL if not available
         */
        void *(__cdecl *getD3D11DeviceContext)(void);

        /**
         * @brief Register mod's ImGui draw data for centralized rendering
         *
         * Mods call this after ImGui::Render() to register their draw data with Wolf.
         * Wolf will collect all mod draw data and render it centrally.
         *
         * @param mod_id Mod ID
         * @param draw_data Pointer to ImDrawData from ImGui::GetDrawData()
         */
        void(__cdecl *registerModDrawData)(WolfModId mod_id, void *draw_data);

        /**
         * @brief Register mod's ImGui context for input event forwarding
         *
         * Called by mods to register their ImGui context for input forwarding.
         * Wolf will forward all input events to registered mod contexts.
         *
         * @param mod_id Mod ID
         * @param context Pointer to ImGuiContext
         */
        void(__cdecl *registerModContext)(WolfModId mod_id, void *context);

        /**
         * @brief Unregister mod's ImGui context from input event forwarding
         *
         * Called by mods during cleanup to unregister their ImGui context.
         *
         * @param mod_id Mod ID
         * @param context Pointer to ImGuiContext
         */
        void(__cdecl *unregisterModContext)(WolfModId mod_id, void *context);

        //==========================================================================
        // WIN32 INPUT SYSTEM
        //==========================================================================

        /**
         * @brief Register a Win32 window procedure hook for input handling
         *
         * Allows mods to receive Win32 messages for custom input handling.
         * Callbacks are called in registration order until one returns true.
         *
         * @param mod_id Mod ID
         * @param callback Callback function to handle Win32 messages
         * @param userData User data passed to the callback
         */
        void(__cdecl *registerWndProcHook)(WolfModId mod_id, WolfWndProcCallback callback, void *userData);

        /**
         * @brief Unregister a Win32 window procedure hook
         *
         * @param mod_id Mod ID
         */
        void(__cdecl *unregisterWndProcHook)(WolfModId mod_id);

        /**
         * @brief Give the player one or more items
         *
         * @param okamiItemID ID of the item to give
         * @param quantity number of items to give (negative to remove)
         */
        void(__cdecl *giveInvItem)(int okamiItemID, int quantity);

    } WolfRuntimeAPI;
}

// === End of wolf_function_table.h ===

// All C types are now defined in wolf_function_table.h and wolf_types.h

//==============================================================================
// C++ FRAMEWORK IMPLEMENTATION
//==============================================================================

namespace wolf
{

//==============================================================================
// MOD INTERFACE & REGISTRATION
//==============================================================================

namespace detail
{
/**
 * @brief Simple global runtime pointer - direct access for game modding
 */
inline WolfRuntimeAPI *g_runtime = nullptr;

/**
 * @brief Initialize runtime pointer
 * @param runtime Runtime API pointer to store
 */
inline void initializeRuntime(WolfRuntimeAPI *runtime) noexcept
{
    g_runtime = runtime;
}

/**
 * @brief Get runtime API pointer
 * @return Runtime API pointer or nullptr if not initialized
 */
inline WolfRuntimeAPI *getRuntime() noexcept
{
    return g_runtime;
}

// Simple mod ID storage for automatic mod identification
inline WolfModId current_mod_id = -1;

/**
 * @brief Get current mod ID
 * @return Current mod ID or -1 if not available
 */
inline WolfModId getCurrentModId() noexcept
{
    if (current_mod_id == -1 && g_runtime)
    {
        current_mod_id = g_runtime->getCurrentModId();
    }
    return current_mod_id;
}

/**
 * @brief Simple cleanup handler storage
 */
inline std::vector<std::function<void()>> cleanup_handlers;

/**
 * @brief Execute all cleanup handlers during mod shutdown
 */
inline void executeCleanup() noexcept
{
    // Execute cleanup handlers in reverse order (LIFO)
    for (auto it = cleanup_handlers.rbegin(); it != cleanup_handlers.rend(); ++it)
    {
        (*it)();
    }
    cleanup_handlers.clear();
}

} // namespace detail

/**
 * @brief Register a cleanup handler for automatic resource management
 * @param handler Function to call during mod shutdown
 *
 * Handlers are executed in LIFO order (last registered, first executed).
 */
inline void registerCleanupHandler(std::function<void()> handler)
{
    detail::cleanup_handlers.push_back(std::move(handler));
}

/**
 * @brief Execute all registered cleanup handlers (internal use only)
 * @warning Should only be called during mod shutdown
 */
inline void executeModCleanup() noexcept
{
    detail::executeCleanup();
}

//==============================================================================
// RUNTIME VERSION INFORMATION
//==============================================================================

/**
 * @brief Get the runtime version string
 * @return Version string (e.g., "0.1.0") or "Unknown" if runtime not available
 */
inline const char *getRuntimeVersion() noexcept
{
    if (detail::g_runtime)
    {
        return detail::g_runtime->getRuntimeVersion();
    }
    return "Unknown";
}

/**
 * @brief Get the runtime build information
 * @return Build info string (e.g., "Debug (Clang 20.1.0)") or "Unknown" if runtime not available
 */
inline const char *getRuntimeBuildInfo() noexcept
{
    if (detail::g_runtime)
    {
        return detail::g_runtime->getRuntimeBuildInfo();
    }
    return "Unknown";
}

//==============================================================================
// ENTRY MACROS
//==============================================================================

/**
 * @brief Register a C-style mod interface with the WOLF framework (recommended)
 * @param earlyInit Function for early initialization (or nullptr)
 * @param lateInit Function for late initialization (or nullptr)
 * @param shutdownFunc Function for cleanup (required)
 * @param nameFunc Function returning mod name (required)
 * @param versionFunc Function returning version (or nullptr for default)
 *
 * Usage:
 * @code
 * void myEarlyInit() { ... }
 * void myLateInit() { ... }
 * void myShutdown() { ... }
 * const char* myGetName() { return "MyMod"; }
 * const char* myGetVersion() { return "1.0.0"; }
 *
 * WOLF_MOD_ENTRY(myEarlyInit, myLateInit, myShutdown, myGetName, myGetVersion)
 * @endcode
 */
#define WOLF_MOD_ENTRY(earlyInit, lateInit, shutdownFunc, nameFunc, versionFunc)                                                                               \
    static void __cdecl wolfModShutdownWrapper(void) noexcept                                                                                                  \
    {                                                                                                                                                          \
        wolf::executeModCleanup(); /* Execute automatic cleanup first */                                                                                       \
        if ((shutdownFunc))                                                                                                                                    \
        {                                                                                                                                                      \
            (shutdownFunc)();                                                                                                                                  \
        } /* Then call user shutdown */                                                                                                                        \
    }                                                                                                                                                          \
    extern "C" __declspec(dllexport) WolfModInterface __cdecl wolfGetModInterface(WolfRuntimeAPI *runtime)                                                     \
    {                                                                                                                                                          \
        wolf::detail::initializeRuntime(runtime);                                                                                                              \
        WolfModInterface modInterface = {(earlyInit), (lateInit), wolfModShutdownWrapper, (nameFunc), (versionFunc), WOLF_VERSION_INT, IMGUI_VERSION_NUM};     \
        wolf::detail::current_mod_id = runtime->registerMod(&modInterface);                                                                                    \
        return modInterface;                                                                                                                                   \
    }

#define WOLF_MOD_ENTRY_NO_IMGUI(earlyInit, lateInit, shutdownFunc, nameFunc, versionFunc)                                                                      \
    static void __cdecl wolfModShutdownWrapper(void) noexcept                                                                                                  \
    {                                                                                                                                                          \
        wolf::executeModCleanup(); /* Execute automatic cleanup first */                                                                                       \
        if ((shutdownFunc))                                                                                                                                    \
        {                                                                                                                                                      \
            (shutdownFunc)();                                                                                                                                  \
        } /* Then call user shutdown */                                                                                                                        \
    }                                                                                                                                                          \
    extern "C" __declspec(dllexport) WolfModInterface __cdecl wolfGetModInterface(WolfRuntimeAPI *runtime)                                                     \
    {                                                                                                                                                          \
        wolf::detail::initializeRuntime(runtime);                                                                                                              \
        WolfModInterface modInterface = {(earlyInit), (lateInit), wolfModShutdownWrapper, (nameFunc), (versionFunc), WOLF_VERSION_INT, 0};                     \
        wolf::detail::current_mod_id = runtime->registerMod(&modInterface);                                                                                    \
        return modInterface;                                                                                                                                   \
    }

/**
 * @brief Simplified macro for common mod pattern
 * @param ModClass A class with static methods: earlyGameInit(), lateGameInit(), shutdown(), getName(), getVersion()
 *
 * Usage:
 * @code
 * class MyMod {
 * public:
 *     static void earlyGameInit() { ... }
 *     static void lateGameInit() { ... }
 *     static void shutdown() { ... }
 *     static const char* getName() { return "MyMod"; }
 *     static const char* getVersion() { return "1.0.0"; }
 * };
 * WOLF_MOD_ENTRY_CLASS(MyMod)
 * @endcode
 */
#define WOLF_MOD_ENTRY_CLASS(ModClass)                                                                                                                         \
    static void __cdecl wolfModClassShutdownWrapper(void) noexcept                                                                                             \
    {                                                                                                                                                          \
        wolf::executeModCleanup(); /* Execute automatic cleanup first */                                                                                       \
        ModClass::shutdown();      /* Then call class shutdown */                                                                                              \
    }                                                                                                                                                          \
    extern "C" __declspec(dllexport) WolfModInterface __cdecl wolfGetModInterface(WolfRuntimeAPI *runtime)                                                     \
    {                                                                                                                                                          \
        wolf::detail::initializeRuntime(runtime);                                                                                                              \
        WolfModInterface modInterface = {                                                                                                                      \
            ModClass::earlyGameInit, ModClass::lateGameInit, wolfModClassShutdownWrapper, ModClass::getName, ModClass::getVersion,                             \
            WOLF_VERSION_INT,        IMGUI_VERSION_NUM};                                                                                                       \
        wolf::detail::current_mod_id = runtime->registerMod(&modInterface);                                                                                    \
        return modInterface;                                                                                                                                   \
    }

#define WOLF_MOD_ENTRY_CLASS_NO_IMGUI(ModClass)                                                                                                                \
    static void __cdecl wolfModClassShutdownWrapper(void) noexcept                                                                                             \
    {                                                                                                                                                          \
        wolf::executeModCleanup(); /* Execute automatic cleanup first */                                                                                       \
        ModClass::shutdown();      /* Then call class shutdown */                                                                                              \
    }                                                                                                                                                          \
    extern "C" __declspec(dllexport) WolfModInterface __cdecl wolfGetModInterface(WolfRuntimeAPI *runtime)                                                     \
    {                                                                                                                                                          \
        wolf::detail::initializeRuntime(runtime);                                                                                                              \
        WolfModInterface modInterface = {                                                                                                                      \
            ModClass::earlyGameInit, ModClass::lateGameInit, wolfModClassShutdownWrapper, ModClass::getName, ModClass::getVersion, WOLF_VERSION_INT, 0};       \
        wolf::detail::current_mod_id = runtime->registerMod(&modInterface);                                                                                    \
        return modInterface;                                                                                                                                   \
    }

} // namespace wolf

// === End of wolf_core.hpp ===

// Specialized API modules (order doesn't matter for these)

// === Expanded from wolf_bitfield.hpp ===
//==============================================================================
// BITFIELD MONITORING SYSTEM
//==============================================================================

namespace wolf
{

/**
 * @brief Handle for a bitfield monitor instance
 */
using BitfieldMonitorHandle = WolfBitfieldMonitorHandle;

/**
 * @brief Callback function for bitfield changes
 * @param bitIndex Index of the bit that changed (0-based)
 * @param oldValue Previous value of the bit (true/false)
 * @param newValue New value of the bit (true/false)
 */
using BitfieldChangeCallback = std::function<void(unsigned int bitIndex, bool oldValue, bool newValue)>;

namespace detail
{
/**
 * @brief Simple storage for bitfield monitor callbacks
 */
inline std::vector<std::unique_ptr<BitfieldChangeCallback>> bitfield_callbacks;

/**
 * @brief Add a bitfield callback
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline BitfieldChangeCallback *addBitfieldCallback(BitfieldChangeCallback &&callback)
{
    auto stored_callback = std::make_unique<BitfieldChangeCallback>(std::move(callback));
    BitfieldChangeCallback *callback_ptr = stored_callback.get();
    bitfield_callbacks.push_back(std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Clear all bitfield callbacks during shutdown
 */
inline void clearBitfieldCallbacks()
{
    bitfield_callbacks.clear();
}

} // namespace detail

/**
 * @brief Create a bitfield monitor for a memory location
 * @param address Memory address of the bitfield
 * @param sizeInBytes Size of the bitfield in bytes
 * @param callback Function called when bits change
 * @param description Optional description for debugging
 * @return Handle to bitfield monitor, or nullptr on failure
 */
inline BitfieldMonitorHandle createBitfieldMonitor(uintptr_t address, size_t sizeInBytes, BitfieldChangeCallback callback,
                                                   const char *description = "") noexcept
{
    BitfieldChangeCallback *callback_ptr = detail::addBitfieldCallback(std::move(callback));

    // Register cleanup handler to ensure callback storage is cleaned up
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearBitfieldCallbacks(); });
        cleanup_registered = true;
    }

    if (!detail::g_runtime)
        return nullptr;

    BitfieldMonitorHandle handle = detail::g_runtime->createBitfieldMonitor(
        detail::getCurrentModId(), address, sizeInBytes,
        [](unsigned int bit_index, int old_value, int new_value, void *userdata) noexcept
        {
            auto *cb = static_cast<BitfieldChangeCallback *>(userdata);
            (*cb)(bit_index, old_value != 0, new_value != 0);
        },
        callback_ptr, description);

    return handle;
}

/**
 * @brief Create a bitfield monitor for module + offset
 * @param module Module name (e.g., "main.dll")
 * @param offset Offset from module base
 * @param sizeInBytes Size of the bitfield in bytes
 * @param callback Function called when bits change
 * @param description Optional description for debugging
 * @return Handle to bitfield monitor, or nullptr on failure
 */
inline BitfieldMonitorHandle createBitfieldMonitor(const char *module, uintptr_t offset, size_t sizeInBytes, BitfieldChangeCallback callback,
                                                   const char *description = "") noexcept
{
    BitfieldChangeCallback *callback_ptr = detail::addBitfieldCallback(std::move(callback));

    if (!detail::g_runtime)
        return nullptr;

    BitfieldMonitorHandle handle = detail::g_runtime->createBitfieldMonitorModule(
        detail::getCurrentModId(), module, offset, sizeInBytes,
        [](unsigned int bit_index, int old_value, int new_value, void *userdata) noexcept
        {
            auto *cb = static_cast<BitfieldChangeCallback *>(userdata);
            (*cb)(bit_index, old_value != 0, new_value != 0);
        },
        callback_ptr, description);

    return handle;
}

/**
 * @brief Destroy a bitfield monitor and stop monitoring
 * @param monitor Bitfield monitor handle to destroy
 * @return True if monitor was successfully destroyed
 */
inline bool destroyBitfieldMonitor(BitfieldMonitorHandle monitor) noexcept
{
    if (!monitor)
        return false;

    if (!detail::g_runtime)
        return false;
    detail::g_runtime->destroyBitfieldMonitor(monitor);
    return true;
}

/**
 * @brief Manually update a bitfield monitor (called automatically during game ticks)
 * @param monitor Bitfield monitor handle
 * @return True on success, false on failure
 */
inline bool updateBitfieldMonitor(BitfieldMonitorHandle monitor) noexcept
{
    if (!monitor)
        return false;

    if (detail::g_runtime)
    {
        return detail::g_runtime->updateBitfieldMonitor(monitor) != 0;
    }
    return false;
}

/**
 * @brief Reset a bitfield monitor to reinitialize baseline state
 * @param monitor Bitfield monitor handle
 * @return True on success, false on failure
 */
inline bool resetBitfieldMonitor(BitfieldMonitorHandle monitor) noexcept
{
    if (!monitor)
        return false;

    if (detail::g_runtime)
    {
        return detail::g_runtime->resetBitfieldMonitor(monitor) != 0;
    }
    return false;
}

} // namespace wolf

// === End of wolf_bitfield.hpp ===

// === Expanded from wolf_console.hpp ===
#include <cstdarg>

// === Expanded from wolf_logging.hpp ===
#include <cstdarg>

//==============================================================================
// LOGGING
//==============================================================================

namespace wolf
{

/**
 * @brief Log message severity levels
 */
enum class LogLevel
{
    Info = 0,    ///< General information
    Warning = 1, ///< Warning conditions
    Error = 2,   ///< Error conditions
    Debug = 3    ///< Debug information
};

namespace detail
{
// Convert C++ log level to C enum - use the constants directly to avoid enum conflicts
inline int toCLogLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Info:
        return 0; // WOLF_LOG_INFO
    case LogLevel::Warning:
        return 1; // WOLF_LOG_WARNING
    case LogLevel::Error:
        return 2; // WOLF_LOG_ERROR
    case LogLevel::Debug:
        return 3; // WOLF_LOG_DEBUG
    default:
        return 0; // WOLF_LOG_INFO
    }
}

// Helper for printf-style formatting (used by logging system)
inline std::string formatLogString(const char *format, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int buffer_size = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (buffer_size <= 0)
        return std::string(format);

    std::string result(buffer_size, '\0');
    vsnprintf(&result[0], buffer_size + 1, format, args);
    return result;
}
} // namespace detail

/**
 * @brief Log an info message with optional printf-style formatting
 * @param format Format string (can be a simple message or printf-style format)
 * @param ... Format arguments (optional)
 */
inline void logInfo(const char *format, ...) noexcept
{
    if (!detail::g_runtime)
        return;

    va_list args;
    va_start(args, format);
    std::string message = detail::formatLogString(format, args);
    va_end(args);

    detail::g_runtime->log(detail::getCurrentModId(), static_cast<WolfLogLevel>(0), message.c_str());
}

/**
 * @brief Log a warning message with optional printf-style formatting
 * @param format Format string (can be a simple message or printf-style format)
 * @param ... Format arguments (optional)
 */
inline void logWarning(const char *format, ...) noexcept
{
    if (!detail::g_runtime)
        return;

    va_list args;
    va_start(args, format);
    std::string message = detail::formatLogString(format, args);
    va_end(args);

    detail::g_runtime->log(detail::getCurrentModId(), static_cast<WolfLogLevel>(1), message.c_str());
}

/**
 * @brief Log an error message with optional printf-style formatting
 * @param format Format string (can be a simple message or printf-style format)
 * @param ... Format arguments (optional)
 */
inline void logError(const char *format, ...) noexcept
{
    if (!detail::g_runtime)
        return;

    va_list args;
    va_start(args, format);
    std::string message = detail::formatLogString(format, args);
    va_end(args);

    detail::g_runtime->log(detail::getCurrentModId(), static_cast<WolfLogLevel>(2), message.c_str());
}

/**
 * @brief Log a debug message with optional printf-style formatting
 * @param format Format string (can be a simple message or printf-style format)
 * @param ... Format arguments (optional)
 */
inline void logDebug(const char *format, ...) noexcept
{
    if (!detail::g_runtime)
        return;

    va_list args;
    va_start(args, format);
    std::string message = detail::formatLogString(format, args);
    va_end(args);

    detail::g_runtime->log(detail::getCurrentModId(), static_cast<WolfLogLevel>(3), message.c_str());
}

/**
 * @brief Set a prefix for all log messages from this mod
 * @param prefix Prefix string (e.g., "[MyMod]")
 * @return True if prefix was successfully set
 */
inline bool setLogPrefix(const char *prefix) noexcept
{
    if (!detail::g_runtime)
        return false;
    detail::g_runtime->setLogPrefix(detail::getCurrentModId(), prefix);
    return true;
}

} // namespace wolf

// === End of wolf_logging.hpp ===

//==============================================================================
// CONSOLE SYSTEM
//==============================================================================

namespace wolf
{

/**
 * @brief Function signature for console command handlers
 * @param args Vector of command arguments (including command name at [0])
 */
using CommandHandler = std::function<void(const std::vector<std::string> &args)>;

namespace detail
{
/**
 * @brief Simple storage for console command callbacks
 */
inline std::vector<std::unique_ptr<CommandHandler>> command_callbacks;

/**
 * @brief Add a command handler
 * @param handler Handler to store
 * @return Pointer to stored handler for runtime registration
 */
inline CommandHandler *addCommandHandler(CommandHandler &&handler)
{
    auto stored_handler = std::make_unique<CommandHandler>(std::move(handler));
    CommandHandler *handler_ptr = stored_handler.get();
    command_callbacks.push_back(std::move(stored_handler));
    return handler_ptr;
}

/**
 * @brief Clear all command callbacks during shutdown
 */
inline void clearCommandCallbacks()
{
    command_callbacks.clear();
}

// C-style callback wrapper for commands
inline void __cdecl commandCallbackWrapper(int argc, const char **argv, void *userdata) noexcept
{
    auto *cb = static_cast<CommandHandler *>(userdata);
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.emplace_back(argv[i]);
    }
    (*cb)(args);
}
} // namespace detail

/**
 * @brief Register a console command
 * @param name Command name (case-sensitive)
 * @param handler Function to call when command is executed
 * @param description Help text for the command (optional)
 * @return True if command was successfully registered
 */
inline bool addCommand(const char *name, CommandHandler handler, const char *description = "") noexcept
{
    CommandHandler *callback_ptr = detail::addCommandHandler(std::move(handler));

    // Register cleanup handler to ensure callback storage is cleaned up
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearCommandCallbacks(); });
        cleanup_registered = true;
    }

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->addCommand(detail::getCurrentModId(), name, detail::commandCallbackWrapper, callback_ptr, description);
    return true;
}

/**
 * @brief Unregister a console command
 * @param name Command name to remove
 * @return True if command was successfully removed
 */
inline bool removeCommand(const char *name) noexcept
{
    if (!detail::g_runtime)
        return false;
    detail::g_runtime->removeCommand(detail::getCurrentModId(), name);
    return true;
}

/**
 * @brief Execute a command line string
 * @param commandLine Full command with arguments
 * @return True if command was successfully executed
 */
inline bool executeCommand(const std::string &commandLine) noexcept
{
    if (!detail::g_runtime)
        return false;
    detail::g_runtime->executeCommand(commandLine.c_str());
    return true;
}

/**
 * @brief Print a message to the console
 * @param message Message to display
 * @return True if message was successfully printed
 */
inline bool consolePrint(const char *message) noexcept
{
    if (!detail::g_runtime)
        return false;
    detail::g_runtime->consolePrint(message);
    return true;
}

/**
 * @brief Print a formatted message to the console
 * @param format Printf-style format string
 * @param ... Format arguments
 * @return True if message was successfully printed
 */
inline bool consolePrintf(const char *format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    // Format string for console output
    va_list args_copy;
    va_copy(args_copy, args);
    int buffer_size = vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    if (buffer_size <= 0)
    {
        va_end(args);
        return false;
    }

    std::string message(buffer_size, '\0');
    vsnprintf(&message[0], buffer_size + 1, format, args);
    va_end(args);

    if (!detail::g_runtime)
        return false;
    detail::g_runtime->consolePrint(message.c_str());
    return true;
}

/**
 * @brief Check if console window is visible
 * @return True if console is visible
 */
inline bool isConsoleVisible() noexcept
{
    if (detail::g_runtime)
    {
        return detail::g_runtime->isConsoleVisible() != 0;
    }
    return false;
}

} // namespace wolf

// === End of wolf_console.hpp ===

// === Expanded from wolf_gui.hpp ===
#ifndef NOIMGUI

#ifndef IMGUI_VERSION
// Forward declarations for ImGui types and functions
struct ImGuiContext;
struct ImFontAtlas;
typedef void *(*ImGuiMemAllocFunc)(size_t sz, void *user_data);
typedef void (*ImGuiMemFreeFunc)(void *ptr, void *user_data);

namespace ImGui
{
void SetCurrentContext(ImGuiContext *ctx);
void SetAllocatorFunctions(ImGuiMemAllocFunc alloc_func, ImGuiMemFreeFunc free_func, void *user_data);
ImGuiContext *CreateContext(ImFontAtlas *shared_font_atlas);
void DestroyContext(ImGuiContext *ctx);
void NewFrame();
void EndFrame();
void Render();
} // namespace ImGui

#endif

//==============================================================================
// GUI SYSTEM
//==============================================================================

namespace wolf
{

// Storage for mod-specific ImGui context and frame state
namespace detail
{
static ImGuiContext *g_modContext = nullptr;
static bool g_modFrameActive = false;
} // namespace detail

/**
 * @brief Setup mod-specific ImGui context with Wolf's shared resources
 * @return True if context was successfully created and configured
 *
 * This function creates a mod-specific ImGui context using Wolf's shared font atlas
 * and allocators. This approach allows each mod to have its own ImGui context while
 * sharing critical resources to prevent crashes and ensure text rendering works.
 *
 * This function should be called BEFORE any other ImGui operations in the mod.
 * Typically this would be done in the mod's early initialization.
 *
 * Usage:
 * @code
 * // In mod initialization:
 * if (!wolf::setupSharedImGuiAllocators()) {
 *     logError("Failed to setup mod ImGui context!");
 *     return false;
 * }
 *
 * // Now it's safe to use ImGui including text rendering
 * wolf::registerGuiWindow("My Window", myCallback);
 * @endcode
 */
inline bool setupSharedImGuiAllocators() noexcept
{
    if (!detail::g_runtime)
    {
        logError("No runtime available");
        return false;
    }

    // Don't create context twice
    if (detail::g_modContext)
    {
        logDebug("Mod context already exists");
        return true;
    }

    // Get Wolf's allocator functions
    void *allocFuncPtr = detail::g_runtime->getImGuiAllocFunc();
    void *freeFuncPtr = detail::g_runtime->getImGuiFreeFunc();
    void *userData = detail::g_runtime->getImGuiAllocUserData();

    if (!allocFuncPtr || !freeFuncPtr)
    {
        logError("Invalid allocator functions");
        return false;
    }

    // Cast to proper function pointers
    ImGuiMemAllocFunc allocFunc = reinterpret_cast<ImGuiMemAllocFunc>(allocFuncPtr);
    ImGuiMemFreeFunc freeFunc = reinterpret_cast<ImGuiMemFreeFunc>(freeFuncPtr);

    // Configure this mod's ImGui to use Wolf's allocators
    ImGui::SetAllocatorFunctions(allocFunc, freeFunc, userData);

    // Get Wolf's shared font atlas
    void *fontAtlasPtr = detail::g_runtime->getImGuiFontAtlas();
    if (!fontAtlasPtr)
    {
        logError("No font atlas available");
        return false;
    }

    ImFontAtlas *sharedFontAtlas = static_cast<ImFontAtlas *>(fontAtlasPtr);

    // Create mod-specific context with Wolf's shared font atlas
    // Each mod gets its own context but shares Wolf's font atlas
    detail::g_modContext = ImGui::CreateContext(sharedFontAtlas);
    if (!detail::g_modContext)
    {
        logError("Failed to create mod context");
        return false;
    }

    // Register this context with Wolf for input event forwarding
    detail::g_runtime->registerModContext(detail::getCurrentModId(), detail::g_modContext);

    return true;
}

/**
 * @brief Cleanup mod's ImGui context
 *
 * This function should be called during mod shutdown to properly cleanup
 * the mod-specific ImGui context. This prevents resource leaks.
 */
inline void cleanupImGuiContext() noexcept
{
    if (detail::g_modContext)
    {
        // Unregister context from Wolf's input forwarding
        if (detail::g_runtime)
        {
            detail::g_runtime->unregisterModContext(detail::getCurrentModId(), detail::g_modContext);
        }

        ImGui::DestroyContext(detail::g_modContext);
        detail::g_modContext = nullptr;
    }
}

/**
 * @brief Get Wolf's shared ImGui font atlas
 * @return Pointer to Wolf's ImFontAtlas, or nullptr if not available
 *
 * This function returns Wolf's shared font atlas that must be used by mods
 * to prevent GPU/rendering conflicts. Without shared font atlas, styled text
 * operations (like ImGui::PushStyleColor() + Text()) can crash due to
 * separate font texture data.
 *
 * Usage:
 * @code
 * // If creating your own context (advanced usage):
 * ImFontAtlas* sharedAtlas = wolf::getSharedFontAtlas();
 * ImGuiContext* context = ImGui::CreateContext(sharedAtlas);
 * @endcode
 */
inline ImFontAtlas *getSharedFontAtlas() noexcept
{
    if (!detail::g_runtime)
        return nullptr;

    void *atlasPtr = detail::g_runtime->getImGuiFontAtlas();
    return static_cast<ImFontAtlas *>(atlasPtr);
}

/**
 * @brief Function signature for GUI window callbacks
 * @param outerWidth Window width
 * @param outerHeight Window height
 * @param uiScale UI scaling factor
 */
using GuiWindowCallback = std::function<void(int outerWidth, int outerHeight, float uiScale)>;

namespace detail
{
/**
 * @brief Simple storage for GUI window callbacks
 */
inline std::vector<std::unique_ptr<GuiWindowCallback>> gui_window_callbacks;

/**
 * @brief Add a GUI window callback
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline GuiWindowCallback *addGuiWindowCallback(GuiWindowCallback &&callback)
{
    auto stored_callback = std::make_unique<GuiWindowCallback>(std::move(callback));
    GuiWindowCallback *callback_ptr = stored_callback.get();
    gui_window_callbacks.push_back(std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Clear all GUI window callbacks during shutdown
 */
inline void clearGuiWindowCallbacks()
{
    gui_window_callbacks.clear();
}

} // namespace detail

/**
 * @brief Unregister a custom GUI window
 * @param windowName Window name to remove
 * @return True if window was successfully unregistered
 */
inline bool unregisterGuiWindow(const char *windowName) noexcept
{
    if (!detail::g_runtime)
        return false;
    return detail::g_runtime->unregisterGuiWindow(detail::getCurrentModId(), windowName) != 0;
}

/**
 * @brief Toggle visibility of a GUI window
 * @param windowName Window name to toggle
 * @return True if window was found and toggled
 */
inline bool toggleGuiWindow(const char *windowName) noexcept
{
    if (!detail::g_runtime)
        return false;
    return detail::g_runtime->toggleGuiWindow(detail::getCurrentModId(), windowName) != 0;
}

/**
 * @brief Set visibility of a GUI window
 * @param windowName Window name
 * @param visible True to show, false to hide
 * @return True if window was found and visibility was set
 */
inline bool setGuiWindowVisible(const char *windowName, bool visible) noexcept
{
    if (!detail::g_runtime)
        return false;
    return detail::g_runtime->setGuiWindowVisible(detail::getCurrentModId(), windowName, visible ? 1 : 0) != 0;
}

/**
 * @brief Ensure mod ImGui context is set up and ready
 * @return True if context is available for use
 */
inline bool ensureModContext() noexcept
{
    if (!detail::g_runtime)
    {
        logError("ensureModContext: No runtime");
        return false;
    }

    // CRITICAL: Set up mod context with shared resources first
    static bool contextSetup = false;
    if (!contextSetup)
    {
        if (!setupSharedImGuiAllocators())
        {
            logError("ensureModContext: setupSharedImGuiAllocators failed");
            return false;
        }
        contextSetup = true;
    }

    return detail::g_modContext != nullptr;
}

//==============================================================================
// IMGUI MACROS FOR MODS
//==============================================================================

/**
 * @brief Initialize D3D11 backend for mod's ImGui context
 *
 * Call this once after setupSharedImGuiAllocators() and before using other ImGui macros.
 * Requires imgui_impl_dx11.h to be included.
 *
 * Usage:
 * @code
 * wolf::setupSharedImGuiAllocators();
 * WOLF_IMGUI_INIT_BACKEND();
 * @endcode
 */
#define WOLF_IMGUI_INIT_BACKEND()                                                                                                                              \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if (!wolf::detail::g_modContext)                                                                                                                       \
        {                                                                                                                                                      \
            wolf::logError("No mod context available");                                                                                                        \
            break;                                                                                                                                             \
        }                                                                                                                                                      \
        void *d3d11Device = wolf::detail::g_runtime->getD3D11Device();                                                                                         \
        void *d3d11DeviceContext = wolf::detail::g_runtime->getD3D11DeviceContext();                                                                           \
        if (d3d11Device && d3d11DeviceContext)                                                                                                                 \
        {                                                                                                                                                      \
            ImGui::SetCurrentContext(wolf::detail::g_modContext);                                                                                              \
            bool result = ImGui_ImplDX11_Init(static_cast<ID3D11Device *>(d3d11Device), static_cast<ID3D11DeviceContext *>(d3d11DeviceContext));               \
        }                                                                                                                                                      \
        else                                                                                                                                                   \
        {                                                                                                                                                      \
            wolf::logError("Invalid D3D11 device or context");                                                                                                 \
        }                                                                                                                                                      \
    } while (0)

/**
 * @brief Begin ImGui frame with proper context and DisplaySize setup
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @param scale UI scale factor
 *
 * Usage:
 * @code
 * wolf::registerGuiWindow("My Window", [](int w, int h, float scale) {
 *     WOLF_IMGUI_BEGIN(w, h, scale);
 *
 *     ImGui::Begin("My Window");
 *     ImGui::Text("Hello World!");
 *     ImGui::End();
 *
 *     WOLF_IMGUI_END();
 * });
 * @endcode
 */
#define WOLF_IMGUI_BEGIN(width, height, scale)                                                                                                                 \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if (!wolf::ensureModContext())                                                                                                                         \
        {                                                                                                                                                      \
            wolf::logError("No valid mod context");                                                                                                            \
            break;                                                                                                                                             \
        }                                                                                                                                                      \
        if (wolf::detail::g_modFrameActive)                                                                                                                    \
        {                                                                                                                                                      \
            wolf::logError("Frame already active");                                                                                                            \
            break;                                                                                                                                             \
        }                                                                                                                                                      \
        ImGui::SetCurrentContext(wolf::detail::g_modContext);                                                                                                  \
        ImGuiIO &io = ImGui::GetIO();                                                                                                                          \
        if (!io.Fonts)                                                                                                                                         \
        {                                                                                                                                                      \
            wolf::logError("No font atlas in IO");                                                                                                             \
            break;                                                                                                                                             \
        }                                                                                                                                                      \
        if (!io.Fonts->IsBuilt())                                                                                                                              \
        {                                                                                                                                                      \
            /* Try to get Wolf's current font atlas and use it */                                                                                              \
            ImFontAtlas *wolfAtlas = wolf::getSharedFontAtlas();                                                                                               \
            if (wolfAtlas && wolfAtlas->IsBuilt())                                                                                                             \
            {                                                                                                                                                  \
                io.Fonts = wolfAtlas;                                                                                                                          \
                wolf::logInfo("Resynced font atlas with Wolf (recovered from invalid state)");                                                                 \
            }                                                                                                                                                  \
            else                                                                                                                                               \
            {                                                                                                                                                  \
                /* Wolf's font atlas is also invalid, skip rendering this frame */                                                                             \
                wolf::logDebug("Font atlas rebuilding in progress, skipping mod render this frame");                                                           \
                break;                                                                                                                                         \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
        io.DisplaySize.x = static_cast<float>(width);                                                                                                          \
        io.DisplaySize.y = static_cast<float>(height);                                                                                                         \
        /* Font atlas is now rasterized at the correct size, no need for FontGlobalScale */                                                                    \
        /* Input events are forwarded globally by Wolf to all registered mod contexts */                                                                       \
        ImGui::NewFrame();                                                                                                                                     \
        wolf::detail::g_modFrameActive = true;

/**
 * @brief End ImGui frame and generate draw data
 *
 * Must be paired with WOLF_IMGUI_BEGIN().
 */
#define WOLF_IMGUI_END()                                                                                                                                       \
    ImGui::EndFrame();                                                                                                                                         \
    ImGui::Render();                                                                                                                                           \
    ImDrawData *drawData = ImGui::GetDrawData();                                                                                                               \
    if (drawData)                                                                                                                                              \
    {                                                                                                                                                          \
        try                                                                                                                                                    \
        {                                                                                                                                                      \
            if (drawData->Valid && drawData->CmdListsCount > 0)                                                                                                \
            {                                                                                                                                                  \
                wolf::detail::g_runtime->registerModDrawData(wolf::detail::getCurrentModId(), drawData);                                                       \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
        catch (...)                                                                                                                                            \
        {                                                                                                                                                      \
            wolf::logError("Exception registering draw data");                                                                                                 \
        }                                                                                                                                                      \
    }                                                                                                                                                          \
    wolf::detail::g_modFrameActive = false;                                                                                                                    \
    }                                                                                                                                                          \
    while (0)

/**
 * @brief Register a Win32 WndProc hook for input handling
 * @param callback Function to handle Win32 messages
 * @param userData User data passed to the callback
 * @return True if hook was successfully registered
 *
 * This allows mods to receive raw Win32 input messages for custom handling,
 * including ImGui input forwarding. The callback should return true if it
 * handled the message and wants to prevent further processing.
 *
 * Usage for ImGui input forwarding:
 * @code
 * wolf::registerWndProcHook([](void* hwnd, unsigned int msg, uintptr_t wParam, intptr_t lParam, void* userData) -> int {
 *     ImGui::SetCurrentContext(myModContext);
 *     return ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(hwnd), msg, wParam, lParam) ? 1 : 0;
 * }, nullptr);
 * @endcode
 */
inline bool registerWndProcHook(std::function<int(void *, unsigned int, uintptr_t, intptr_t, void *)> callback, void *userData = nullptr) noexcept
{
    if (!detail::g_runtime || !callback)
        return false;

    // Store the callback in a way that can be called from C
    static std::vector<std::unique_ptr<std::function<int(void *, unsigned int, uintptr_t, intptr_t, void *)>>> s_callbacks;
    auto stored_callback = std::make_unique<std::function<int(void *, unsigned int, uintptr_t, intptr_t, void *)>>(std::move(callback));
    auto *callback_ptr = stored_callback.get();
    s_callbacks.push_back(std::move(stored_callback));

    // Create C-compatible wrapper
    auto c_wrapper = [](void *hwnd, unsigned int msg, uintptr_t wParam, intptr_t lParam, void *userData) -> int
    {
        auto *cpp_callback = static_cast<std::function<int(void *, unsigned int, uintptr_t, intptr_t, void *)> *>(userData);
        return (*cpp_callback)(hwnd, msg, wParam, lParam, nullptr);
    };

    detail::g_runtime->registerWndProcHook(detail::getCurrentModId(), c_wrapper, callback_ptr);
    return true;
}

/**
 * @brief Unregister the mod's Win32 WndProc hook
 */
inline void unregisterWndProcHook() noexcept
{
    if (detail::g_runtime)
    {
        detail::g_runtime->unregisterWndProcHook(detail::getCurrentModId());
    }
}

/**
 * @brief Register a custom GUI window
 * @param windowName Window name/title
 * @param callback Function called to draw the window
 * @param initiallyVisible Whether window starts visible (default: false)
 * @return True if window was successfully registered
 */
inline bool registerGuiWindow(const char *windowName, GuiWindowCallback callback, bool initiallyVisible = false) noexcept
{
    GuiWindowCallback *callback_ptr = detail::addGuiWindowCallback(std::move(callback));

    // Register cleanup handler to ensure callback storage is cleaned up
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearGuiWindowCallbacks(); });
        cleanup_registered = true;
    }

    if (!detail::g_runtime)
        return false;

    return detail::g_runtime->registerGuiWindow(
               detail::getCurrentModId(), windowName,
               [](int outer_width, int outer_height, float ui_scale, void *userdata) noexcept
               {
                   // Execute the mod's GUI callback - mod handles frame lifecycle with macros
                   auto *cb = static_cast<GuiWindowCallback *>(userdata);
                   (*cb)(outer_width, outer_height, ui_scale);
               },
               callback_ptr, initiallyVisible ? 1 : 0) != 0;
}

/**
 * @brief Execute a function within the proper ImGui context
 * @param renderFunc Function to execute with ImGui context active
 * @return True if the function was executed successfully
 *
 * This function ensures that ImGui::GetCurrentContext() returns a valid context
 * when the renderFunc is called, allowing safe use of ImGui functions.
 *
 * Note: This is typically not needed for GUI window callbacks registered with
 * registerGuiWindow(), as they automatically have the correct ImGui context.
 * This function is primarily for advanced use cases where you need to execute
 * ImGui code outside of registered window callbacks.
 */
inline bool executeInImGuiContext(std::function<void()> renderFunc) noexcept
{
    if (!detail::g_runtime || !renderFunc)
        return false;

    return detail::g_runtime->executeInImGuiContext(
               detail::getCurrentModId(),
               [](void *userdata) noexcept
               {
                   auto *func = static_cast<std::function<void()> *>(userdata);
                   (*func)();
               },
               &renderFunc) != 0;
}

} // namespace wolf

#endif // NOIMGUI

// === End of wolf_gui.hpp ===

// === Expanded from wolf_hooks.hpp ===

// === Expanded from wolf_memory.hpp ===
#include <unordered_map>

//==============================================================================
// MEMORY ACCESS
//==============================================================================

namespace wolf
{

namespace detail
{
/**
 * @brief Simple module base address cache for performance
 */
inline std::unordered_map<std::string, uintptr_t> module_cache;

/**
 * @brief Get cached module base address
 * @param module_name Module name to look up
 * @return Module base address or 0 if not found/cached
 */
inline uintptr_t getCachedModuleBase(const char *module_name) noexcept
{
    if (!module_name)
        return 0;

    auto it = module_cache.find(module_name);
    if (it != module_cache.end())
    {
        return it->second;
    }

    // Cache miss - look up via runtime and cache the result
    if (g_runtime)
    {
        uintptr_t base = g_runtime->getModuleBase(module_name);
        module_cache[module_name] = base;
        return base;
    }

    return 0;
}

/**
 * @brief Clear module cache during shutdown
 */
inline void clearModuleCache() noexcept
{
    module_cache.clear();
}
} // namespace detail

/**
 * @brief Get the base address of a loaded module with caching
 * @param moduleName Module name (e.g., "main.dll", "flower_kernel.dll")
 * @return Base address of the module, or 0 if not found
 *
 * Performance Notes:
 * - First access per module hits the runtime API (slow)
 * - Subsequent accesses use cached values (fast)
 */
inline uintptr_t getModuleBase(const char *moduleName) noexcept
{
    if (!moduleName)
        return 0;

    // Register cache cleanup on first use
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearModuleCache(); });
        cleanup_registered = true;
    }

    return detail::getCachedModuleBase(moduleName);
}

/**
 * @brief Check if an address is valid and accessible
 * @param address Address to validate
 * @return True if address is safe to access
 */
inline bool isValidAddress(uintptr_t address) noexcept
{
    if (detail::g_runtime)
    {
        return detail::g_runtime->isValidAddress(address) != 0;
    }
    return false;
}

/**
 * @brief Type-safe memory accessor for direct game memory access
 * @tparam T Type of data at the memory location
 *
 * Provides direct access to game memory with runtime address binding.
 * Optimized for performance, no validation overhead or memory shadows.
 */
template <typename T> class MemoryAccessor
{
    static_assert(std::is_trivially_copyable_v<T>, "MemoryAccessor requires trivially copyable types");

  private:
    uintptr_t address = 0; ///< Memory address being accessed

  public:
    /**
     * @brief Default constructor - creates unbound accessor
     */
    MemoryAccessor() = default;

    /**
     * @brief Construct an accessor for a specific address
     * @param addr Memory address to access
     */
    explicit MemoryAccessor(uintptr_t addr) : address(addr)
    {
    }

    /**
     * @brief Construct an accessor for a specific relative address
     * @param module Module to be relative to
     * @param offset offset from Base
     */
    explicit MemoryAccessor(const char *module, uintptr_t offset) : address(getModuleBase(module) + offset)
    {
    }

    /**
     * @brief Bind to an address (useful for runtime address resolution)
     * @param addr Memory address to access
     */
    void bind(uintptr_t addr)
    {
        address = addr;
    }

    /**
     * @brief Bind to an offset relative to a module
     * @param module Module to be relative to
     * @param offset offset from Base
     */
    void bind(const char *module, uintptr_t offset)
    {
        address = getModuleBase(module) + offset;
    }

    /**
     * @brief Read value directly from memory
     * @return Value at memory location
     * @warning Direct access - no validation or bounds checking performed
     * @warning Not thread-safe - concurrent access may cause data races
     */
    T get() const
    {
        return *reinterpret_cast<volatile T *>(address);
    }

    /**
     * @brief Write value directly to memory
     * @param value Value to write
     * @warning Direct access - no validation or bounds checking performed
     * @warning Not thread-safe - concurrent access may cause data races
     * @warning May crash game if writing to protected memory
     */
    void set(const T &value)
    {
        *reinterpret_cast<volatile T *>(address) = value;
    }

    /**
     * @brief Get raw pointer for direct access
     * @return Pointer to memory location
     * @warning Direct access - no validation performed
     */
    T *get_ptr() const
    {
        return reinterpret_cast<T *>(address);
    }

    /**
     * @brief Get the raw memory address
     * @return Memory address
     */
    uintptr_t raw() const
    {
        return address;
    }

    /**
     * @brief Check if accessor is bound to a valid address
     * @return True if address is non-zero (basic validation only)
     * @note This only checks if the address is non-zero, not if it's valid/accessible
     */
    bool is_bound() const noexcept
    {
        return address != 0;
    }

    /**
     * @brief Compare-and-swap operation (thread-safe atomic operation)
     * @param expected Expected value to compare against
     * @param desired New value to set if comparison succeeds
     * @return True if swap succeeded, false otherwise
     * @note Only available for types that fit in an atomic word size
     */
    template <typename U = T>
    typename std::enable_if_t<sizeof(U) <= sizeof(void *) && std::is_trivially_copyable_v<U>, bool> compare_exchange(T &expected, const T &desired) noexcept
    {
        static_assert(std::is_same_v<U, T>, "Template parameter must match T");
        auto *atomic_ptr = reinterpret_cast<std::atomic<T> *>(address);
        return atomic_ptr->compare_exchange_strong(expected, desired, std::memory_order_acq_rel);
    }

    /**
     * @brief Atomic load operation
     * @return Current value at memory location
     * @note Only available for types that fit in an atomic word size
     */
    template <typename U = T> typename std::enable_if_t<sizeof(U) <= sizeof(void *) && std::is_trivially_copyable_v<U>, T> atomic_load() const noexcept
    {
        static_assert(std::is_same_v<U, T>, "Template parameter must match T");
        auto *atomic_ptr = reinterpret_cast<std::atomic<T> *>(address);
        return atomic_ptr->load(std::memory_order_acquire);
    }

    /**
     * @brief Atomic store operation
     * @param value Value to store
     * @note Only available for types that fit in an atomic word size
     */
    template <typename U = T>
    typename std::enable_if_t<sizeof(U) <= sizeof(void *) && std::is_trivially_copyable_v<U>, void> atomic_store(const T &value) noexcept
    {
        static_assert(std::is_same_v<U, T>, "Template parameter must match T");
        auto *atomic_ptr = reinterpret_cast<std::atomic<T> *>(address);
        atomic_ptr->store(value, std::memory_order_release);
    }

    // ===== CONVENIENCE OPERATORS ====

    /**
     * @brief Implicit conversion to T via get()
     * @return Value at memory location
     */
    operator T() const
    {
        return get();
    }

    /**
     * @brief Assignment operator to write a value
     * @param value Value to write
     * @return Reference to this accessor
     */
    MemoryAccessor &operator=(const T &value)
    {
        set(value);
        return *this;
    }

    /**
     * @brief Access members directly (for struct/class types)
     * @return Pointer to memory location
     */
    T *operator->() const
    {
        return get_ptr();
    }

    /**
     * @brief Dereference operator for reference-style access
     * @return Reference to memory location
     */
    T &operator*() const
    {
        return *get_ptr();
    }
};

/**
 * @brief Create a typed accessor for module + offset
 * @tparam T Type of data to access
 * @param module Module name (e.g., "main.dll")
 * @param offset Offset from module base
 * @return Typed accessor for the memory location
 */
template <typename T> inline MemoryAccessor<T> getMemoryAccessor(const char *module, uintptr_t offset)
{
    return MemoryAccessor<T>(getModuleBase(module) + offset);
}

/**
 * @brief Create a typed accessor for absolute address
 * @tparam T Type of data to access
 * @param absoluteAddress Absolute memory address
 * @return Typed accessor for the memory location
 */
template <typename T> inline MemoryAccessor<T> getMemoryAccessor(uintptr_t absoluteAddress)
{
    return MemoryAccessor<T>(absoluteAddress);
}

/**
 * @brief Read raw bytes from memory
 * @param address Source address
 * @param buffer Destination buffer
 * @param size Number of bytes to read
 * @return True if read succeeded
 */
inline bool readMemory(uintptr_t address, void *buffer, size_t size) noexcept
{
    if (detail::g_runtime && buffer && size > 0)
    {
        return detail::g_runtime->readMemory(address, buffer, size) != 0;
    }
    return false;
}

/**
 * @brief Write raw bytes to memory
 * @param address Destination address
 * @param buffer Source buffer
 * @param size Number of bytes to write
 * @return True if write succeeded
 */
inline bool writeMemory(uintptr_t address, const void *buffer, size_t size) noexcept
{
    if (detail::g_runtime && buffer && size > 0)
    {
        return detail::g_runtime->writeMemory(address, buffer, size) != 0;
    }
    return false;
}

/**
 * @brief Search for byte patterns in memory with Optional Caching
 * @param pattern Byte pattern to search for
 * @param mask Pattern mask (? for wildcards, x for exact match)
 * @param module Module to search in (nullptr for all modules)
 * @return Vector of addresses where pattern was found
 *
 * Performance Notes:
 * - Pattern searches are expensive operations
 * - Consider caching results if searching for the same pattern repeatedly
 * - Use getMemoryAccessor() to create accessors from found addresses
 */
inline std::vector<uintptr_t> findPattern(const char *pattern, const char *mask, const char *module = nullptr) noexcept
{
    std::vector<uintptr_t> results;

    struct CallbackData
    {
        std::vector<uintptr_t> *results;
    } data = {&results};

    if (detail::g_runtime && pattern && mask)
    {
        detail::g_runtime->findPattern(
            pattern, mask, module, [](uintptr_t address, void *userdata) noexcept { static_cast<CallbackData *>(userdata)->results->push_back(address); },
            &data);
    }

    return results;
}

/**
 * @brief Find first occurrence of a pattern
 * @param pattern Byte pattern to search for
 * @param mask Pattern mask (? for wildcards, x for exact match)
 * @param module Module to search in (nullptr for all modules)
 * @return Address of first match, or 0 if not found
 *
 * Performance optimized version that stops after finding the first match.
 */
inline uintptr_t findFirstPattern(const char *pattern, const char *mask, const char *module = nullptr) noexcept
{
    uintptr_t result = 0;
    bool found = false;

    struct CallbackData
    {
        uintptr_t *result;
        bool *found;
    } data = {&result, &found};

    if (detail::g_runtime && pattern && mask)
    {
        detail::g_runtime->findPattern(
            pattern, mask, module,
            [](uintptr_t address, void *userdata) noexcept
            {
                auto *cb_data = static_cast<CallbackData *>(userdata);
                if (!*(cb_data->found))
                {
                    *(cb_data->result) = address;
                    *(cb_data->found) = true;
                }
            },
            &data);
    }

    return result;
}

/**
 * @brief Callback function for memory change notifications
 * @param address Address where change occurred
 * @param oldData Previous data at the address
 * @param newData New data at the address
 * @param size Size of the changed data
 */
using MemoryWatchCallback = std::function<void(uintptr_t address, const void *oldData, const void *newData, size_t size)>;

namespace detail
{
/**
 * @brief Simple storage for memory watch callbacks
 */
inline std::vector<std::pair<uintptr_t, std::unique_ptr<MemoryWatchCallback>>> memory_watch_callbacks;

/**
 * @brief Add a memory watch callback
 * @param address Address being watched
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline MemoryWatchCallback *addMemoryWatchCallback(uintptr_t address, MemoryWatchCallback &&callback)
{
    auto stored_callback = std::make_unique<MemoryWatchCallback>(std::move(callback));
    MemoryWatchCallback *callback_ptr = stored_callback.get();
    memory_watch_callbacks.emplace_back(address, std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Remove a memory watch callback
 * @param address Address to stop watching
 * @return True if callback was found and removed
 */
inline bool removeMemoryWatchCallback(uintptr_t address)
{
    auto it = std::find_if(memory_watch_callbacks.begin(), memory_watch_callbacks.end(), [address](const auto &pair) { return pair.first == address; });
    if (it != memory_watch_callbacks.end())
    {
        memory_watch_callbacks.erase(it);
        return true;
    }
    return false;
}

/**
 * @brief Clear all memory watch callbacks
 */
inline void clearMemoryWatchCallbacks()
{
    memory_watch_callbacks.clear();
}
} // namespace detail

/**
 * @brief Watch a memory region for changes
 * @param start Starting address of region
 * @param size Size of region to watch
 * @param callback Function called when changes are detected
 * @param description Optional description for debugging
 * @return True if watch was successfully set up
 */
inline bool watchMemory(uintptr_t start, size_t size, MemoryWatchCallback callback, const char *description = "") noexcept
{
    auto *callback_ptr = detail::addMemoryWatchCallback(start, std::move(callback));

    // Register cleanup handler to ensure callback storage is cleaned up
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearMemoryWatchCallbacks(); });
        cleanup_registered = true;
    }

    if (!detail::g_runtime)
        return false;

    bool result = detail::g_runtime->watchMemory(
                      detail::getCurrentModId(), start, size,
                      [](uintptr_t address, const void *old_data, const void *new_data, size_t change_size, void *userdata) noexcept
                      {
                          auto *cb = static_cast<MemoryWatchCallback *>(userdata);
                          (*cb)(address, old_data, new_data, change_size);
                      },
                      callback_ptr, description ? description : "") != 0;

    if (!result)
    {
        // Remove callback if registration failed
        detail::removeMemoryWatchCallback(start);
    }

    return result;
}

/**
 * @brief Stop watching a memory region
 * @param start Starting address of region to unwatch
 * @return True if watch was successfully removed
 */
inline bool unwatchMemory(uintptr_t start) noexcept
{
    // Remove from local storage first
    bool callback_removed = detail::removeMemoryWatchCallback(start);

    if (detail::g_runtime)
    {
        return detail::g_runtime->unwatchMemory(detail::getCurrentModId(), start) != 0;
    }

    return callback_removed; // Return true if we at least cleaned up our callback
}

} // namespace wolf

// === End of wolf_memory.hpp ===

//==============================================================================
// GAME HOOKS & CALLBACKS
//==============================================================================

namespace wolf
{

namespace detail
{
/**
 * @brief Simple storage for game event callbacks
 */
inline std::vector<std::unique_ptr<std::function<void()>>> game_event_callbacks;

/**
 * @brief Simple storage for item pickup callbacks
 */
inline std::vector<std::unique_ptr<std::function<void(int, int)>>> item_pickup_callbacks;

/**
 * @brief Simple storage for blocking item pickup callbacks
 */
inline std::vector<std::unique_ptr<std::function<bool(int, int)>>> item_pickup_blocking_callbacks;

/**
 * @brief Simple storage for brush edit callbacks
 */
inline std::vector<std::unique_ptr<std::function<bool(int, int)>>> brush_edit_callbacks;

/**
 * @brief Add a game event callback
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline std::function<void()> *addGameEventCallback(std::function<void()> &&callback)
{
    auto stored_callback = std::make_unique<std::function<void()>>(std::move(callback));
    std::function<void()> *callback_ptr = stored_callback.get();
    game_event_callbacks.push_back(std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Add an item pickup callback
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline std::function<void(int, int)> *addItemPickupCallback(std::function<void(int, int)> &&callback)
{
    auto stored_callback = std::make_unique<std::function<void(int, int)>>(std::move(callback));
    std::function<void(int, int)> *callback_ptr = stored_callback.get();
    item_pickup_callbacks.push_back(std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Add a blocking item pickup callback
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline std::function<bool(int, int)> *addItemPickupBlockingCallback(std::function<bool(int, int)> &&callback)
{
    auto stored_callback = std::make_unique<std::function<bool(int, int)>>(std::move(callback));
    std::function<bool(int, int)> *callback_ptr = stored_callback.get();
    item_pickup_blocking_callbacks.push_back(std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Add a brush edit callback
 * @param callback Callback to store
 * @return Pointer to stored callback for runtime registration
 */
inline std::function<bool(int, int)> *addBrushEditCallback(std::function<bool(int, int)> &&callback)
{
    auto stored_callback = std::make_unique<std::function<bool(int, int)>>(std::move(callback));
    std::function<bool(int, int)> *callback_ptr = stored_callback.get();
    brush_edit_callbacks.push_back(std::move(stored_callback));
    return callback_ptr;
}

/**
 * @brief Clear all hook callbacks during shutdown
 */
inline void clearHookCallbacks()
{
    game_event_callbacks.clear();
    item_pickup_callbacks.clear();
    item_pickup_blocking_callbacks.clear();
    brush_edit_callbacks.clear();
}

} // namespace detail

/**
 * @brief Register callback for game tick (called every frame)
 * @param callback Function to call each frame
 * @return True if registration succeeded, false otherwise
 */
inline bool onGameTick(std::function<void()> callback)
{
    auto *callback_ptr = detail::addGameEventCallback(std::move(callback));

    // Register cleanup handler to ensure callback storage is cleaned up
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearHookCallbacks(); });
        cleanup_registered = true;
    }

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->registerGameTick(
        detail::getCurrentModId(),
        [](void *userdata) noexcept
        {
            auto *cb = static_cast<std::function<void()> *>(userdata);
            (*cb)();
        },
        callback_ptr);
    return true;
}

/**
 * @brief Register callback for game start
 * @param callback Function to call when game starts
 * @return True if registration succeeded, false otherwise
 */
inline bool onGameStart(std::function<void()> callback)
{
    auto *callback_ptr = detail::addGameEventCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->registerGameStart(
        detail::getCurrentModId(),
        [](void *userdata) noexcept
        {
            auto *cb = static_cast<std::function<void()> *>(userdata);
            (*cb)();
        },
        callback_ptr);
    return true;
}

/**
 * @brief Register callback for game stop
 * @param callback Function to call when game stops
 * @return True if registration succeeded, false otherwise
 */
inline bool onGameStop(std::function<void()> callback)
{
    auto *callback_ptr = detail::addGameEventCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->registerGameStop(
        detail::getCurrentModId(),
        [](void *userdata) noexcept
        {
            auto *cb = static_cast<std::function<void()> *>(userdata);
            (*cb)();
        },
        callback_ptr);
    return true;
}

/**
 * @brief Register callback for gameplay start (i.e, not in main menu and after a save file loads)
 * @param callback Function to call when gameplay starts
 * @return True if registration succeeded, false otherwise
 */
inline bool onPlayStart(std::function<void()> callback)
{
    auto *callback_ptr = detail::addGameEventCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->registerPlayStart(
        detail::getCurrentModId(),
        [](void *userdata) noexcept
        {
            auto *cb = static_cast<std::function<void()> *>(userdata);
            (*cb)();
        },
        callback_ptr);
    return true;
}

/**
 * @brief Register callback for return to menu
 * @param callback Function to call when returning to menu
 * @return True if registration succeeded, false otherwise
 */
inline bool onReturnToMenu(std::function<void()> callback)
{
    auto *callback_ptr = detail::addGameEventCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->registerReturnToMenu(
        detail::getCurrentModId(),
        [](void *userdata) noexcept
        {
            auto *cb = static_cast<std::function<void()> *>(userdata);
            (*cb)();
        },
        callback_ptr);
    return true;
}

/**
 * @brief Register callback for item pickup events
 * @param callback Function to call when player picks up items
 * @return True if registration succeeded, false otherwise
 */
inline bool onItemPickup(std::function<void(int itemId, int count)> callback)
{
    auto *callback_ptr = detail::addItemPickupCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->registerItemPickup(
        detail::getCurrentModId(),
        [](int item_id, int count, void *userdata) noexcept
        {
            auto *cb = static_cast<std::function<void(int, int)> *>(userdata);
            (*cb)(item_id, count);
        },
        callback_ptr);
    return true;
}

/**
 * @brief Hook a function at absolute address
 * @tparam FuncSig Function signature type
 * @param address Absolute memory address of function
 * @param detour Replacement function
 * @param original Pointer to store original function (optional)
 * @return True if hook was successfully created
 */
template <typename FuncSig> inline bool hookFunction(uintptr_t address, FuncSig detour, FuncSig *original = nullptr) noexcept
{
    void **orig_ptr = original ? reinterpret_cast<void **>(original) : nullptr;

    if (!detail::g_runtime)
        return false;
    return detail::g_runtime->hookFunction(address, reinterpret_cast<void *>(detour), orig_ptr) != 0;
}

/**
 * @brief Hook a function at module + offset
 * @tparam FuncSig Function signature type
 * @param module Module name (e.g., "main.dll")
 * @param offset Offset from module base
 * @param detour Replacement function
 * @param original Pointer to store original function (optional)
 * @return True if hook was successfully created
 */
template <typename FuncSig> inline bool hookFunction(const char *module, uintptr_t offset, FuncSig detour, FuncSig *original = nullptr) noexcept
{
    uintptr_t module_base = getModuleBase(module);
    if (module_base == 0)
        return false;

    return hookFunction(module_base + offset, detour, original);
}

/**
 * @brief Replace a function completely (no original call)
 * @tparam FuncSig Function signature type
 * @param address Absolute memory address of function
 * @param replacement Replacement function
 * @return True if replacement was successful
 */
template <typename FuncSig> inline bool replaceFunction(uintptr_t address, FuncSig replacement) noexcept
{
    return hookFunction(address, replacement, static_cast<FuncSig *>(nullptr));
}

/**
 * @brief Replace a function at module + offset
 * @tparam FuncSig Function signature type
 * @param module Module name (e.g., "main.dll")
 * @param offset Offset from module base
 * @param replacement Replacement function
 * @return True if replacement was successful
 */
template <typename FuncSig> inline bool replaceFunction(const char *module, uintptr_t offset, FuncSig replacement) noexcept
{
    uintptr_t module_base = getModuleBase(module);
    if (module_base == 0)
        return false;

    return replaceFunction(module_base + offset, replacement);
}

/**
 * @brief Register blocking callback for item pickup events
 * @param callback Function to call when player picks up items (return true to block)
 * @return True if registration succeeded, false otherwise
 */
inline bool onItemPickupBlocking(std::function<bool(int itemId, int count)> callback)
{
    auto *callback_ptr = detail::addItemPickupBlockingCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    return detail::g_runtime->registerItemPickupBlocking(
               detail::getCurrentModId(),
               [](int item_id, int count, void *userdata) noexcept -> int
               {
                   auto *cb = static_cast<std::function<bool(int, int)> *>(userdata);
                   try
                   {
                       return (*cb)(item_id, count) ? 1 : 0;
                   }
                   catch (...)
                   {
                       return 0; // Default to not blocking on exception
                   }
               },
               callback_ptr) != 0;
}

/**
 * @brief Register callback for brush edit events
 * @param callback Function to call when brush is edited (return true to block)
 * @return True if registration succeeded, false otherwise
 */
inline bool onBrushEdit(std::function<bool(int bitIndex, int operation)> callback)
{
    auto *callback_ptr = detail::addBrushEditCallback(std::move(callback));

    if (!detail::g_runtime)
        return false;

    return detail::g_runtime->registerBrushEdit(
               detail::getCurrentModId(),
               [](int bit_index, int operation, void *userdata) noexcept -> int
               {
                   auto *cb = static_cast<std::function<bool(int, int)> *>(userdata);
                   try
                   {
                       return (*cb)(bit_index, operation) ? 1 : 0;
                   }
                   catch (...)
                   {
                       return 0; // Default to not blocking on exception
                   }
               },
               callback_ptr) != 0;
}

} // namespace wolf

// === End of wolf_hooks.hpp ===

// === Expanded from wolf_misc.hpp ===
namespace wolf
{
inline void giveItem(int okamiItemID, int quantity)
{
    detail::g_runtime->giveInvItem(okamiItemID, quantity);
}
} // namespace wolf

// === End of wolf_misc.hpp ===

// === Expanded from wolf_resources.hpp ===
//==============================================================================
// RESOURCE SYSTEM
//==============================================================================

namespace wolf
{

/**
 * @brief Function signature for resource providers
 * @param originalPath The original resource path requested by game
 * @return Path to replacement resource, or nullptr to use original
 */
using ResourceProvider = std::function<const char *(const char *originalPath)>;

namespace detail
{
/**
 * @brief Simple storage for resource provider callbacks
 */
inline std::vector<std::unique_ptr<ResourceProvider>> resource_provider_callbacks;

/**
 * @brief Add a resource provider callback
 * @param provider Provider to store
 * @return Pointer to stored provider for runtime registration
 */
inline ResourceProvider *addResourceProvider(ResourceProvider &&provider)
{
    auto stored_provider = std::make_unique<ResourceProvider>(std::move(provider));
    ResourceProvider *provider_ptr = stored_provider.get();
    resource_provider_callbacks.push_back(std::move(stored_provider));
    return provider_ptr;
}

/**
 * @brief Clear all resource provider callbacks during shutdown
 */
inline void clearResourceProviderCallbacks()
{
    resource_provider_callbacks.clear();
}

} // namespace detail

/**
 * @brief Intercept loading of a specific resource file
 * @param filename Exact filename to intercept (e.g., "id/ItemShopBuyIcon.dat")
 * @param provider Function that returns replacement path
 * @return True if interception was successfully set up
 */
inline bool interceptResource(const char *filename, ResourceProvider provider) noexcept
{
    ResourceProvider *provider_ptr = detail::addResourceProvider(std::move(provider));

    // Register cleanup handler to ensure callback storage is cleaned up
    static bool cleanup_registered = false;
    if (!cleanup_registered)
    {
        registerCleanupHandler([]() { detail::clearResourceProviderCallbacks(); });
        cleanup_registered = true;
    }

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->interceptResource(
        detail::getCurrentModId(), filename,
        [](const char *original_path, void *userdata) noexcept -> const char *
        {
            auto *prov = static_cast<ResourceProvider *>(userdata);
            return (*prov)(original_path);
        },
        provider_ptr);
    return true;
}

/**
 * @brief Remove resource interception
 * @param filename Filename to stop intercepting
 * @return True if interception was successfully removed
 */
inline bool removeResourceInterception(const char *filename) noexcept
{
    if (!detail::g_runtime)
        return false;
    detail::g_runtime->removeResourceInterception(detail::getCurrentModId(), filename);
    return true;
}

/**
 * @brief Intercept resources matching a pattern
 * @param pattern Wildcard pattern (e.g., "*.dat", "textures/...")
 * @param provider Function that returns replacement path
 * @return True if pattern interception was successfully set up
 */
inline bool interceptResourcePattern(const char *pattern, ResourceProvider provider) noexcept
{
    ResourceProvider *provider_ptr = detail::addResourceProvider(std::move(provider));

    if (!detail::g_runtime)
        return false;

    detail::g_runtime->interceptResourcePattern(
        detail::getCurrentModId(), pattern,
        [](const char *original_path, void *userdata) noexcept -> const char *
        {
            auto *prov = static_cast<ResourceProvider *>(userdata);
            return (*prov)(original_path);
        },
        provider_ptr);
    return true;
}

} // namespace wolf

// === End of wolf_resources.hpp ===
