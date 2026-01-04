#include <memory>

#include <imgui.h>

#include <wolf_framework.hpp>

#include "aplocationmonitor.h"
#include "archipelagosocket.h"
#include "containermanager.h"
#include "gamestate_accessors.hpp"
#include "item_handlers.hpp"
#include "loginwindow.h"
#include "version.h"

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

        item_handlers::initialize();
        loginwindow::initialize(ArchipelagoSocket::instance());

        APLocationMonitor::instance().initialize();
        APLocationMonitor::instance().setSocket(&ArchipelagoSocket::instance());
        container_manager::setSocket(&ArchipelagoSocket::instance());

        // Initialize container-based item randomization
        if (!container_manager::initialize())
        {
            wolf::logWarning("[APClient] Container manager failed to initialize, container randomization disabled");
        }

        wolf::onGameTick(
            []()
            {
                ArchipelagoSocket::instance().processMainThreadTasks();
                ArchipelagoSocket::instance().poll();
                item_handlers::handleAPItems();
                container_manager::pollForPickups();
            });
    }

    static void shutdown()
    {
        container_manager::shutdown();
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
