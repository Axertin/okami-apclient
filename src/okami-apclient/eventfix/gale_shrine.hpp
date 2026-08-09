#pragma once

#include <span>

#include "registry.hpp"

namespace eventfix::gale_shrine
{

// Bypass records for Gale Shrine forced-tutorial softlock. See
// docs/event-triggers-runtime.md for the runtime model and chain.
std::span<const EventBypass> getBypasses();

} // namespace eventfix::gale_shrine
