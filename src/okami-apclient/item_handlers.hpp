#pragma once

namespace item_handlers
{

void initialize();
void reset();

bool receiveAPItem(int ItemID);
bool handleAPItems();
bool handleItem(int itemID);

} // namespace item_handlers
