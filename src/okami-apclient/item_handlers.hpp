#pragma once

#include "apitems_generated.hpp"

namespace item_handlers
{

void initialize();
void reset(); // For testing - clears queue and resets safeToReceive

bool receiveAPItem(int ItemID);
bool handleAPItems();

// Handler functions for each item type
// Returns true on success, false on failure

bool handle_brush(const apitems::APItemDef *item);
bool handle_progressive_brush(const apitems::APItemDef *item);
bool handle_inventory(const apitems::APItemDef *item);
bool handle_event_flags(const apitems::APItemDef *item);
bool handle_progressive(const apitems::APItemDef *item);

// Main dispatch function
bool handleItem(int itemID);

} // namespace item_handlers
