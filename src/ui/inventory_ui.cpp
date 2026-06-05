#include "inventory_ui.h"
#include "world/item.h"
#include <string>

namespace FloraPhilosophia {
namespace UI {

InventoryUI::InventoryUI()
    : m_open(false)
{}

void InventoryUI::Toggle() {
    m_open = !m_open;
}

Rectangle InventoryUI::GetSlotRect(int slotIndex) const {
    // Items stack vertically in the panel
    return Rectangle{
        static_cast<float>(PANEL_X + ITEM_PADDING),
        static_cast<float>(PANEL_Y + 30 + slotIndex * (ITEM_SIZE + ITEM_PADDING)),
        static_cast<float>(ITEM_SIZE),
        static_cast<float>(ITEM_SIZE)
    };
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw
// Renders the inventory panel as a vertical list of item slots.
// Each slot shows a colored placeholder, the item name, and quantity.
// ─────────────────────────────────────────────────────────────────────────────
void InventoryUI::Draw(const World::Inventory& inventory) const {
    if (!m_open) {
        // When closed, just show the toggle hint
        DrawRectangle(PANEL_X, PANEL_Y, PANEL_WIDTH, 30, Fade(BLACK, 0.7f));
        DrawText("[I] Inventory", PANEL_X + 8, PANEL_Y + 8, 12, GOLD);
        return;
    }

    const auto& entries = inventory.GetAllEntries();
    int panelHeight = 36 + static_cast<int>(entries.size()) * (ITEM_SIZE + ITEM_PADDING) + ITEM_PADDING;

    // Panel background
    DrawRectangle(PANEL_X, PANEL_Y, PANEL_WIDTH, panelHeight, Fade(BLACK, 0.85f));
    DrawRectangleLines(PANEL_X, PANEL_Y, PANEL_WIDTH, panelHeight, Color{ 140, 100, 70, 255 });
    DrawText("[I] Inventory", PANEL_X + 8, PANEL_Y + 8, 12, GOLD);

    if (entries.empty()) {
        DrawText("(empty)", PANEL_X + 8, PANEL_Y + 36, 11, GRAY);
        return;
    }

    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        const auto& entry = entries[i];
        const World::ItemDefinition& def = World::GetItemDefinition(entry.type);
        Rectangle slot = GetSlotRect(i);

        // Slot background
        DrawRectangleRec(slot, Color{ 60, 50, 40, 255 });
        DrawRectangleLinesEx(slot, 1.5f, Color{ 100, 80, 60, 255 });

        // Item name (truncated to fit)
        std::string name = def.displayName;
        if (name.length() > 10) name = name.substr(0, 9) + ".";
        DrawText(name.c_str(), static_cast<int>(slot.x + 4), static_cast<int>(slot.y + 8),  10, RAYWHITE);

        // Quantity badge
        std::string qty = "x" + std::to_string(entry.quantity);
        DrawText(qty.c_str(),  static_cast<int>(slot.x + 4), static_cast<int>(slot.y + 42), 10, GOLD);

        // Hover highlight — not implemented yet (requires tracking hover state)
        // TODO: add hover detection to show full item description tooltip
    }

    // Placement mode hint
    DrawText("Click item to place", PANEL_X + 4, PANEL_Y + panelHeight - 16, 9, GRAY);
}

// ─────────────────────────────────────────────────────────────────────────────
// HandleClick
// Returns the ItemType of the clicked slot, or ItemType::COUNT if none.
// ─────────────────────────────────────────────────────────────────────────────
World::ItemType InventoryUI::HandleClick(Vector2 mouseScreenPos,
                                         const World::Inventory& inventory) const {
    if (!m_open) return World::ItemType::COUNT;

    const auto& entries = inventory.GetAllEntries();
    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
        if (CheckCollisionPointRec(mouseScreenPos, GetSlotRect(i))) {
            return entries[i].type;
        }
    }
    return World::ItemType::COUNT;
}

} // namespace UI
} // namespace FloraPhilosophia
