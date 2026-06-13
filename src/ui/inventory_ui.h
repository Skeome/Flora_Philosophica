#ifndef FLORA_PHILOSOPHICA_UI_INVENTORY_UI_H
#define FLORA_PHILOSOPHICA_UI_INVENTORY_UI_H

#include "world/inventory.h"
#include "world/room_manager.h"
#include "world/placed_item.h"
#include "raylib.h"

namespace FloraPhilosophica {
namespace UI {

// ─────────────────────────────────────────────────────────────────────────────
// InventoryUI
// Renders the player's unified 46-slot inventory.
// Supports drag-and-drop movement and hotbar selection.
// ─────────────────────────────────────────────────────────────────────────────
class InventoryUI {
public:
    InventoryUI();

    // Toggle panel open/closed (called on I keypress)
    void Toggle();
    bool IsOpen() const { return m_open; }

    // Logic and Input
    void UpdateDragging(Vector2 mousePos, World::Inventory& inventory);

    struct SlotClickResult {
        int index = -1; // 0-45
        bool isHerb = false;
        World::ItemType type = World::ItemType::COUNT;
    };
    SlotClickResult GetSlotAt(Vector2 mousePos) const;

    // Rendering
    void Draw(const World::Inventory& inventory) const;
    void DrawHotbar(const World::Inventory& inventory) const;

    // Station UIs
    void DrawDryingRackUI(World::Inventory& inventory);
    void DrawStorageUI(World::Inventory& inventory);

    World::PlacedItem* GetActiveDryingRack() const { return m_activeDryingRack; }
    void SetActiveDryingRack(World::PlacedItem* rack) { m_activeDryingRack = rack; }

    World::PlacedItem* GetActiveStorage() const { return m_activeStorage; }
    void SetActiveStorage(World::PlacedItem* storage) { m_activeStorage = storage; }

    bool IsAnyUIOpen() const {
        return m_open || m_activeDryingRack != nullptr || m_activeStorage != nullptr;
    }

    void CloseActiveStationUIs() {
        m_activeDryingRack = nullptr;
        m_activeStorage = nullptr;
    }

    // Hotbar selection controls
    int  GetSelectedSlot() const { return m_selectedSlot; }
    void SetSelectedSlot(int slot);

    // Layout Helpers
    Rectangle GetGridSlotRect(int row, int col) const;
    Rectangle GetHotbarSlotRectInside(int i) const;

private:
    bool m_open;
    int  m_selectedSlot; // Currently selected active slot (0-9)
    
    // Drag and Drop state
    int  m_draggedSlot = -1; // Index 0-45
    bool m_isDragging = false;

    World::PlacedItem* m_activeDryingRack;
    World::PlacedItem* m_activeStorage;

    // Panel dimensions and position
    static constexpr int GRID_COLS      = 12;
    static constexpr int GRID_ROWS      = 3;
    static constexpr int HOTBAR_COLS    = 10;
    static constexpr int SLOT_SIZE      = 48;
    static constexpr int SLOT_PADDING   = 6;
};

} // namespace UI
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_UI_INVENTORY_UI_H
