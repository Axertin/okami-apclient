#pragma once

#include <cstdint>

namespace okami::custommodelpkg
{

/// Initialize all 6 custom model packages. Call once during itempatch::initialize().
void initialize();

/// Get the in-memory resource package pointer for an AP dummy entity type ID.
/// Returns nullptr if entityType is not an AP dummy.
void *getPackageForEntity(uint32_t entityTypeId);

/// Check if an entity type ID is an AP dummy item.
bool isApDummyEntity(uint32_t entityTypeId);

} // namespace okami::custommodelpkg
