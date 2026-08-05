#include "eventfix.hpp"

#include <span>

#include <wolf_framework.hpp>

#include "cave_of_nagi.hpp"
#include "common.hpp"
#include "hana_valley.hpp"
#include "kamiki_village.hpp"
#include "registry.hpp"

namespace eventfix
{

namespace
{

bool installBypass(const EventBypass &bypass)
{
    void *trampoline = nullptr;
    if (!wolf::hookFunction("main.dll", bypass.funcOffset, reinterpret_cast<void *>(bypass.stub), &trampoline))
    {
        wolf::logError("[eventfix] failed to hook %s at +0x%llx", bypass.name, static_cast<unsigned long long>(bypass.funcOffset));
        return false;
    }
    return true;
}

void installAll(std::span<const EventBypass> bypasses, int &installed)
{
    for (const auto &bypass : bypasses)
    {
        if (installBypass(bypass))
            ++installed;
    }
}

} // namespace

void initialize()
{
    if (!initializeCommon())
    {
        wolf::logError("[eventfix] main.dll base not found; bypasses not installed");
        return;
    }

    int installed = 0;
    installAll(cave_of_nagi::getBypasses(), installed);
    installAll(kamiki_village::getBypasses(), installed);
    installAll(hana_valley::getBypasses(), installed);

    wolf::logInfo("[eventfix] installed %d bypass hooks", installed);
}

} // namespace eventfix
