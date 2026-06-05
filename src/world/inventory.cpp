#include "inventory.h"
#include "nlohmann/json.hpp"
#include <stdexcept>

namespace FloraPhilosophica {
namespace World {

Inventory::Inventory() {}

// ─────────────────────────────────────────────────────────────────────────────
// FindEntry
// Linear search through entries for a given item type.
// Returns the index in m_entries, or -1 if not found.
// Fine for the number of distinct item types we have (~16).
// ─────────────────────────────────────────────────────────────────────────────
int Inventory::FindEntry(ItemType type) const {
    for (int i = 0; i < static_cast<int>(m_entries.size()); ++i) {
        if (m_entries[i].type == type) return i;
    }
    return -1;
}

// ─────────────────────────────────────────────────────────────────────────────
// AddItem
// Increases the stack count for an existing entry, or creates a new entry.
// ─────────────────────────────────────────────────────────────────────────────
void Inventory::AddItem(ItemType type, int quantity) {
    if (quantity <= 0) return;
    int index = FindEntry(type);
    if (index >= 0) {
        m_entries[index].quantity += quantity;
    } else {
        m_entries.push_back({ type, quantity });
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// RemoveItem
// Decrements the stack. Removes the entry entirely if quantity hits zero.
// Returns false if there weren't enough items.
// ─────────────────────────────────────────────────────────────────────────────
bool Inventory::RemoveItem(ItemType type, int quantity) {
    int index = FindEntry(type);
    if (index < 0 || m_entries[index].quantity < quantity) return false;

    m_entries[index].quantity -= quantity;
    if (m_entries[index].quantity == 0) {
        // Erase this entry — swap with back for O(1) removal
        m_entries[index] = m_entries.back();
        m_entries.pop_back();
    }
    return true;
}

int Inventory::GetQuantity(ItemType type) const {
    int index = FindEntry(type);
    return (index >= 0) ? m_entries[index].quantity : 0;
}

bool Inventory::HasItem(ItemType type) const {
    return GetQuantity(type) > 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Serialise
// Converts inventory to a JSON string for saving.
// Format: [{"type": 0, "quantity": 3}, ...]
// ─────────────────────────────────────────────────────────────────────────────
std::string Inventory::Serialise() const {
    nlohmann::json j = nlohmann::json::array();
    for (const auto& entry : m_entries) {
        j.push_back({
            { "type",     static_cast<int>(entry.type) },
            { "quantity", entry.quantity               }
        });
    }
    return j.dump();
}

// ─────────────────────────────────────────────────────────────────────────────
// Deserialise
// Restores inventory from a previously saved JSON string.
// Clears current inventory before loading.
// ─────────────────────────────────────────────────────────────────────────────
void Inventory::Deserialise(const std::string& jsonStr) {
    m_entries.clear();
    try {
        auto j = nlohmann::json::parse(jsonStr);
        for (const auto& entry : j) {
            int typeInt = entry.at("type").get<int>();
            int qty     = entry.at("quantity").get<int>();
            if (typeInt >= 0 && typeInt < static_cast<int>(ItemType::COUNT) && qty > 0) {
                m_entries.push_back({ static_cast<ItemType>(typeInt), qty });
            }
        }
    } catch (const nlohmann::json::exception& e) {
        // If the save is corrupt, start with empty inventory rather than crashing
        m_entries.clear();
    }
}

} // namespace World
} // namespace FloraPhilosophica
