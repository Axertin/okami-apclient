#pragma once

#include <span>

#include "registry.hpp"

namespace eventfix::hana_valley
{

// Bypass records for Hana Valley where Issun prevents you from leaving during Guardian Sapling sequence. See
// docs/event-triggers-runtime.md for the runtime model and chain.
std::span<const EventBypass> getBypasses();

} // namespace eventfix::hana_valley
