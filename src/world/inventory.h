#ifndef FLORA_PHILOSOPHICA_WORLD_INVENTORY_H
#define FLORA_PHILOSOPHICA_WORLD_INVENTORY_H

#include "item.h"
#include <vector>
#include <string>

namespace FloraPhilosophica {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// InventorySlot
// A single cell in the unified inventory (46 slots total).
// Can hold either a stack of placeable items or a single HarvestItem herb.
// ─────────────────────────────────────────────────────────────────────────────
struct InventorySlot {
    bool occupied = false;
    bool isHerb   = false;
    
    // For Herbs
    HarvestItem herb;
    
    // For Placeables (Stations, furniture)
    ItemType    station  = ItemType::COUNT;
    int         quantity = 0;

    void Clear() {
        occupied = false;
        isHerb   = false;
        station  = ItemType::COUNT;
        quantity = 0;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Inventory
// A fixed-size (46 slot) container for all player items.
// Slots 0-9: Hotbar row
// Slots 10-45: 3x12 main grid
// ─────────────────────────────────────────────────────────────────────────────
class Inventory {
public:
    static constexpr int TOTAL_SLOTS = 46;

    Inventory();

    // ── Unified Slot Management ──────────────────────────────────────────
    const InventorySlot& GetSlot(int index) const;
    void SetSlot(int index, const InventorySlot& slot);
    void SwapSlots(int idxA, int idxB);
    void ClearSlot(int index);

    // ── Legacy API Compatibility ─────────────────────────────────────────
    // These now find the first available slot or stack
    void AddItem(ItemType type, int quantity = 1);
    bool RemoveItem(ItemType type, int quantity = 1);
    
    void AddHarvestItem(const std::string& plantName, HarvestQuality quality);
    void AddHarvestItem(const HarvestItem& item);
    bool RemoveHarvestItem(const std::string& plantName, PlantStage stage);

    // Helper for apparatus logic
    bool HasHarvestItem(const std::string& plantName, PlantStage stage) const;
    const HarvestItem* FindHarvestItem(const std::string& plantName, PlantStage stage) const;

    // ── Serialisation ─────────────────────────────────────────────────────
    std::string Serialise() const;
    void        Deserialise(const std::string& json);

private:
    std::vector<InventorySlot> m_slots;

    int FindFirstEmptySlot() const;
    int FindStack(ItemType type) const;
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_INVENTORY_H
