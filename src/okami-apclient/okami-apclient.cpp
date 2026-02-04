#include <memory>

#include <wolf_framework.hpp>

#include "archipelagosocket.h"
#include "checkman.h"
#include "gamestate_accessors.hpp"
#include "loginwindow.h"
#include "notificationwindow.h"
#include "rewardman.h"
#include "version.h"
#include "warpwindow.h"

#ifdef _WIN32
#include <d3d11.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#endif

// Global manager instances
static std::unique_ptr<RewardMan> g_rewardMan;
static std::unique_ptr<CheckMan> g_checkMan;

// GUI window name for wolf registration
static const char *g_guiWindowName = "APClient GUI";

// Main GUI render function - calls all internal windows
static void renderGui(int outerWidth, int outerHeight, [[maybe_unused]] float uiScale)
{
#ifdef _WIN32
    WOLF_IMGUI_BEGIN(outerWidth, outerHeight, uiScale);

    // Draw all windows
    loginwindow::draw();
    warpwindow::draw();
    notificationwindow::draw(outerWidth, outerHeight);

    WOLF_IMGUI_END();
#endif
}

static void initializeGui()
{
    if (!wolf::setupSharedImGuiAllocators())
    {
        wolf::logError("[GUI] Failed to setup mod ImGui context!");
        return;
    }

    WOLF_IMGUI_INIT_BACKEND();

    if (!wolf::registerGuiWindow(g_guiWindowName, renderGui, true))
    {
        wolf::logError("[GUI] Failed to register GUI window");
        return;
    }

    wolf::logInfo("[GUI] Initialized");
}

static void shutdownGui()
{
    wolf::unregisterGuiWindow(g_guiWindowName);
    wolf::logInfo("[GUI] Shutdown complete");
}

class APClientMod
{
  public:
    static void earlyGameInit()
    {
    }

    static void lateGameInit()
    {
        wolf::logDebug("main.dll is at 0x%llX", wolf::getModuleBase("main.dll"));
        // Initialize game state accessors first
        apgame::initialize();

        // Create managers with explicit dependencies
        g_checkMan = std::make_unique<CheckMan>(ArchipelagoSocket::instance());

        // RewardMan callback to disable check sending during granting
        g_rewardMan = std::make_unique<RewardMan>(
            [](bool enabled)
            {
                if (g_checkMan)
                {
                    g_checkMan->enableSending(enabled);
                }
            });

        // Inject managers into socket
        ArchipelagoSocket::instance().setRewardMan(g_rewardMan.get());
        ArchipelagoSocket::instance().setCheckMan(g_checkMan.get());

        // Initialize UI
        initializeGui();
        loginwindow::initialize(ArchipelagoSocket::instance());
        warpwindow::initialize();

        // Initialize check manager (sets up monitors and container hooks)
        g_checkMan->initialize();

        // Game tick handler
        wolf::onGameTick(
            []()
            {
                ArchipelagoSocket::instance().processMainThreadTasks();
                ArchipelagoSocket::instance().poll();
                g_rewardMan->processQueuedRewards();
                g_checkMan->poll();
            });
    }

    static void shutdown()
    {
        if (g_checkMan)
        {
            g_checkMan->shutdown();
        }
        g_checkMan.reset();
        g_rewardMan.reset();
        warpwindow::shutdown();
        notificationwindow::shutdown();
        loginwindow::shutdown();
        shutdownGui();
    }

    static const char *getName()
    {
        return "Archipelago Client";
    }

    static const char *getVersion()
    {
        return version::string.data();
    }
};

WOLF_MOD_ENTRY_CLASS(APClientMod)
