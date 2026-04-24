#include <memory>

#include <wolf_framework.hpp>

#include "archipelagosocket.h"
#include "checkman.h"
#include "gamestate_accessors.hpp"
#include "itempatch.hpp"
#include "rewardman.h"
#include "saveman.h"
#include "ui/loginwindow.h"
#include "ui/notificationwindow.h"
#include "ui/warpwindow.h"
#include "version.h"

#include <okami/maps.hpp>
#include <okami/offsets.hpp>

#ifdef _WIN32
#include <d3d11.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#endif

// Global manager instances
static std::unique_ptr<RewardMan> g_rewardMan;
static std::unique_ptr<CheckMan> g_checkMan;
static std::unique_ptr<SaveMan> g_saveMan;

// GUI window name for wolf registration
static const char *g_guiWindowName = "APClient GUI";

// Main GUI render function
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
        itempatch::initializeEarly();
    }

    static void lateGameInit()
    {
        wolf::logDebug("main.dll is at 0x%llX", wolf::getModuleBase("main.dll"));
        // Initialize game state accessors first
        apgame::initialize();

        // Install hooks (before patchItemParams, so hooks intercept game events)
        itempatch::initialize();

        // Patch ItemParam array to prevent shop crashes
        itempatch::patchItemParams();

        // Create save manager and install save/load hooks
        g_saveMan = std::make_unique<SaveMan>(ArchipelagoSocket::instance());
        g_saveMan->initialize();
        g_saveMan->installHooks();

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
        loginwindow::setSaveMan(g_saveMan.get());
        warpwindow::initialize();

        // Initialize check manager (sets up monitors and container hooks)
        g_checkMan->initialize();

        // Wire auto-save: queue save after each check is sent
        g_checkMan->setOnCheckSentCallback([]() {
            if (g_saveMan)
                g_saveMan->queueAutoSave();
        });

        // AP mode lifecycle: activate on play start (if connected). The Steam
        // redirect and the FUN_18014f580 / FUN_18014e100 hooks handle data
        // injection; we just track "we're in an AP gameplay session".
        wolf::onPlayStart([]() {
            if (g_saveMan && ArchipelagoSocket::instance().isConnected())
                g_saveMan->setApModeActive(true);
        });
        wolf::onReturnToMenu([]() {
            if (g_saveMan)
            {
                g_saveMan->deactivateRedirect();
                g_saveMan->setApModeActive(false);
            }
        });

        // Game tick handler
        wolf::onGameTick(
            []()
            {
                ArchipelagoSocket::instance().processMainThreadTasks();
                ArchipelagoSocket::instance().poll();
                g_rewardMan->processQueuedRewards();
                g_checkMan->poll();
                // Activate Steam redirect when connected and path is available.
                // If the user connects after the title menu was already populated with
                // vanilla save state, warn: the currently-displayed menu won't reflect
                // the AP save until the game re-reads save status (return-to-menu).
                if (g_saveMan && g_saveMan->isConnected() && !g_saveMan->isRedirectActive())
                {
                    std::string path = g_saveMan->getSavePath();
                    if (!path.empty())
                    {
                        g_saveMan->activateRedirect(path);

                        uintptr_t base = wolf::getModuleBase("main.dll");
                        if (base != 0)
                        {
                            auto mapId = *reinterpret_cast<const uint16_t *>(
                                base + okami::main::exteriorMapID);
                            if (mapId == static_cast<uint16_t>(MapID::TitleScreen) ||
                                mapId == static_cast<uint16_t>(MapID::TitleScreenDemoCutscene))
                            {
                                wolf::logWarning("[APClient] Steam redirect activated while on title screen "
                                                 "(mapId=0x%X). The title menu was already populated with "
                                                 "vanilla save state and will not reflect the AP save until "
                                                 "the game re-reads save status. Return to menu to re-read.",
                                                 mapId);
                            }
                        }
                    }
                }
                if (g_saveMan)
                    g_saveMan->processAutoSave();
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
        g_saveMan.reset();
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
