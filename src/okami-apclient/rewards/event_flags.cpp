#include "event_flags.hpp"

#include <wolf_framework.hpp>

#include "../gamestate_accessors.hpp"

namespace rewards::event_flags
{

std::expected<void, RewardError> grant(int64_t apItemId)
{
    uint32_t flagBit = getFlagBit(apItemId);
    apgame::keyItemsAcquired->Set(flagBit);
    return {};
}

} // namespace rewards::event_flags
