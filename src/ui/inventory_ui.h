#ifndef FLORA_PHILOSOPHICA_UI_INVENTORY_UI_H
#define FLORA_PHILOSOPHICA_UI_INVENTORY_UI_H

#include "world/inventory.h"
#include "world/room_manager.h"
#include "raylib.h"

namespace FloraPhilosophica {
namespace UI {

// ─────────────────────────────────────────────────────────────────────────────
// InventoryUI
// Renders the player's unplaced item inventory as a grid panel.
// Pressing I toggles visibility.
// Clicking an item in the panel enters placement mode for that item.
//
// Layout: a panel anchored to the right edge of the screen.
// Each item is a 64x64 tile with an icon placeholder and item name.
// ─────────────────────────────────────────────────────────────────────────────
class InventoryUI {
public:
    InventoryUI();

    // Toggle panel open/closed (called on I keypress)
    void Toggle();
    bool IsOpen() const { return m_open; }

    // Draw the inventory panel (screen-space, not world-space)
    void Draw(const World::Inventory& inventory) const;

    // Handle mouse click on the panel.
    // If the player clicks an item, returns its type for placement.
    // Returns ItemType::COUNT if no item was clicked.
    World::ItemType HandleClick(Vector2 mouseScreenPos,
                                const World::Inventory& inventory) const;

private:
    bool m_open;

    // Panel dimensions and position
    static constexpr int PANEL_X      = 1040; // right side of 1200px screen
    static constexpr int PANEL_Y      = 20;
    static constexpr int PANEL_WIDTH  = 140;
    static constexpr int ITEM_SIZE    = 60;   // each item cell in pixels
    static constexpr int ITEM_PADDING = 8;

    // Returns the screen rect for the nth item slot
    Rectangle GetSlotRect(int slotIndex) const;
};

} // namespace UI
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_UI_INVENTORY_UI_H
