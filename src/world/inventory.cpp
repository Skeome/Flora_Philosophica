#include "inventory.h"
#include "nlohmann/json.hpp"
#include <algorithm>

namespace FloraPhilosophica {
namespace World {

Inventory::Inventory() {
    m_slots.resize(TOTAL_SLOTS);
}

const InventorySlot& Inventory::GetSlot(int index) const {
    if (index < 0 || index >= TOTAL_SLOTS) {
        static InventorySlot empty;
        return empty;
    }
    return m_slots[index];
}

void Inventory::SetSlot(int index, const InventorySlot& slot) {
    if (index >= 0 && index < TOTAL_SLOTS) {
        m_slots[index] = slot;
    }
}

void Inventory::SwapSlots(int idxA, int idxB) {
    if (idxA >= 0 && idxA < TOTAL_SLOTS && idxB >= 0 && idxB < TOTAL_SLOTS) {
        std::swap(m_slots[idxA], m_slots[idxB]);
    }
}

void Inventory::ClearSlot(int index) {
    if (index >= 0 && index < TOTAL_SLOTS) {
        m_slots[index].Clear();
    }
}

int Inventory::FindFirstEmptySlot() const {
    for (int i = 0; i < TOTAL_SLOTS; ++i) {
        if (!m_slots[i].occupied) return i;
    }
    return -1;
}

int Inventory::FindStack(ItemType type) const {
    for (int i = 0; i < TOTAL_SLOTS; ++i) {
        if (m_slots[i].occupied && !m_slots[i].isHerb && m_slots[i].station == type) {
            return i;
        }
    }
    return -1;
}

void Inventory::AddItem(ItemType type, int quantity) {
    if (quantity <= 0) return;
    int idx = FindStack(type);
    if (idx != -1) {
        m_slots[idx].quantity += quantity;
    } else {
        int emptyIdx = FindFirstEmptySlot();
        if (emptyIdx != -1) {
            m_slots[emptyIdx].occupied = true;
            m_slots[emptyIdx].isHerb = false;
            m_slots[emptyIdx].station = type;
            m_slots[emptyIdx].quantity = quantity;
        }
    }
}

bool Inventory::RemoveItem(ItemType type, int quantity) {
    int idx = FindStack(type);
    if (idx != -1 && m_slots[idx].quantity >= quantity) {
        m_slots[idx].quantity -= quantity;
        if (m_slots[idx].quantity <= 0) {
            m_slots[idx].Clear();
        }
        return true;
    }
    return false;
}

void Inventory::AddHarvestItem(const std::string& plantName, HarvestQuality quality) {
    int emptyIdx = FindFirstEmptySlot();
    if (emptyIdx != -1) {
        m_slots[emptyIdx].occupied = true;
        m_slots[emptyIdx].isHerb = true;
        m_slots[emptyIdx].herb = { plantName, PlantStage::Fresh, quality };
    }
}

void Inventory::AddHarvestItem(const HarvestItem& item) {
    int emptyIdx = FindFirstEmptySlot();
    if (emptyIdx != -1) {
        m_slots[emptyIdx].occupied = true;
        m_slots[emptyIdx].isHerb = true;
        m_slots[emptyIdx].herb = item;
    }
}

bool Inventory::RemoveHarvestItem(const std::string& plantName, PlantStage stage) {
    for (int i = 0; i < TOTAL_SLOTS; ++i) {
        if (m_slots[i].occupied && m_slots[i].isHerb && m_slots[i].herb.plantName == plantName && m_slots[i].herb.stage == stage) {
            m_slots[i].Clear();
            return true;
        }
    }
    return false;
}

bool Inventory::HasHarvestItem(const std::string& plantName, PlantStage stage) const {
    return FindHarvestItem(plantName, stage) != nullptr;
}

const HarvestItem* Inventory::FindHarvestItem(const std::string& plantName, PlantStage stage) const {
    for (int i = 0; i < TOTAL_SLOTS; ++i) {
        if (m_slots[i].occupied && m_slots[i].isHerb && m_slots[i].herb.plantName == plantName && m_slots[i].herb.stage == stage) {
            return &m_slots[i].herb;
        }
    }
    return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Serialise / Deserialise
// ─────────────────────────────────────────────────────────────────────────────
std::string Inventory::Serialise() const {
    nlohmann::json j = nlohmann::json::array();
    for (int i = 0; i < TOTAL_SLOTS; ++i) {
        const auto& s = m_slots[i];
        nlohmann::json sj;
        sj["occ"] = s.occupied;
        if (s.occupied) {
            sj["herb"] = s.isHerb;
            if (s.isHerb) {
                sj["name"] = s.herb.plantName;
                sj["stg"]  = static_cast<int>(s.herb.stage);
                sj["ql"]   = static_cast<int>(s.herb.quality);
            } else {
                sj["type"] = static_cast<int>(s.station);
                sj["qty"]  = s.quantity;
            }
        }
        j.push_back(sj);
    }
    return j.dump();
}

void Inventory::Deserialise(const std::string& jsonStr) {
    for (auto& s : m_slots) s.Clear();
    try {
        auto j = nlohmann::json::parse(jsonStr);

        // ── NEW FORMAT: JSON Array of 46 slots ───────────────────────────
        if (j.is_array()) {
            for (size_t i = 0; i < j.size() && i < (size_t)TOTAL_SLOTS; ++i) {
                const auto& sj = j[i];
                if (sj.contains("occ") && sj.at("occ").get<bool>()) {
                    m_slots[i].occupied = true;
                    m_slots[i].isHerb = sj.at("herb").get<bool>();
                    if (m_slots[i].isHerb) {
                        m_slots[i].herb.plantName = sj.at("name").get<std::string>();
                        m_slots[i].herb.stage = static_cast<PlantStage>(sj.at("stg").get<int>());
                        m_slots[i].herb.quality = static_cast<HarvestQuality>(sj.at("ql").get<int>());
                    } else {
                        m_slots[i].station = static_cast<ItemType>(sj.at("type").get<int>());
                        m_slots[i].quantity = sj.at("qty").get<int>();
                    }
                }
            }
        }
        // ── OLD FORMAT: Object with "items" and "herbs" keys ──────────────
        else if (j.is_object()) {
            if (j.contains("items")) {
                for (const auto& e : j["items"]) {
                    AddItem(static_cast<ItemType>(e.at("type").get<int>()), e.at("quantity").get<int>());
                }
            }
            if (j.contains("herbs")) {
                for (const auto& h : j["herbs"]) {
                    HarvestItem item;
                    item.plantName = h.at("plant").get<std::string>();
                    item.stage     = static_cast<PlantStage>(h.at("stage").get<int>());
                    item.quality   = static_cast<HarvestQuality>(h.at("quality").get<int>());
                    AddHarvestItem(item);
                }
            }
        }
    } catch (...) {
        for (auto& s : m_slots) s.Clear();
    }
}

} // namespace World
} // namespace FloraPhilosophica
