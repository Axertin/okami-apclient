#include <memory>

#include <imgui.h>

#include <wolf_framework.hpp>

#include "archipelagosocket.h"
#include "checkman.h"
#include "gamestate_accessors.hpp"
#include "loginwindow.h"
#include "rewardman.h"
#include "version.h"

// Global manager instances (explicit construction, not singletons)
static std::unique_ptr<RewardMan> g_rewardMan;
static std::unique_ptr<CheckMan> g_checkMan;

class APClientMod
{
  public:
    static void earlyGameInit()
    {
    }

    static void lateGameInit()
    {
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
        loginwindow::initialize(ArchipelagoSocket::instance());

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
        loginwindow::shutdown();
    }

    static const char *getName()
    {
        return "Archipelago Client";
    }

    static const char *getVersion()
    {
        return version::string();
    }
};

WOLF_MOD_ENTRY_CLASS(APClientMod)
