#ifndef FLORA_PHILOSOPHICA_WORLD_INVENTORY_H
#define FLORA_PHILOSOPHICA_WORLD_INVENTORY_H

#include "item.h"
#include <vector>
#include <string>

namespace FloraPhilosophica {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// InventoryEntry
// One stack of a single item type in the player's unplaced inventory.
// quantity tracks how many of that item the player is holding.
// ─────────────────────────────────────────────────────────────────────────────
struct InventoryEntry {
    ItemType type;
    int      quantity;  // Number of unplaced instances of this item
};

// ─────────────────────────────────────────────────────────────────────────────
// Inventory
// The player's collection of unplaced items — things they own but haven't
// put down in a room yet. Placed items are tracked by the RoomManager instead.
//
// Items move through three states:
//   1. In Inventory (here)       — owned but not placed
//   2. Placed (in a Room)        — on the property grid, interactive
//   3. Consumed / destroyed      — removed from the game entirely
// ─────────────────────────────────────────────────────────────────────────────
class Inventory {
public:
    Inventory();

    // Add one or more items to the inventory
    void AddItem(ItemType type, int quantity = 1);

    // Remove one item of the given type. Returns false if none available.
    bool RemoveItem(ItemType type, int quantity = 1);

    // Returns how many of a given item are currently unplaced
    int GetQuantity(ItemType type) const;

    // Returns true if the player has at least one of this item unplaced
    bool HasItem(ItemType type) const;

    // Returns all inventory entries (for rendering the inventory UI)
    const std::vector<InventoryEntry>& GetAllEntries() const { return m_entries; }

    // Serialisation helpers — used by the save system
    // Returns a JSON-compatible string representation of the inventory
    std::string Serialise() const;

    // Loads inventory state from a previously serialised string
    void Deserialise(const std::string& json);

private:
    std::vector<InventoryEntry> m_entries;  // All item stacks the player holds

    // Finds the entry index for a given type, or -1 if not present
    int FindEntry(ItemType type) const;
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_INVENTORY_H
