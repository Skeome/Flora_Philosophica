#include "inventory.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace godot {

// --- InventorySlot ---

InventorySlot::InventorySlot() {}
InventorySlot::~InventorySlot() {}

void InventorySlot::clear() {
    occupied = false;
    is_herb = false;
    herb.unref();
    station = ITEM_COUNT;
    quantity = 0;
}

Dictionary InventorySlot::to_dict() const {
    Dictionary d;
    d["occupied"] = occupied;
    d["is_herb"] = is_herb;
    if (is_herb && herb.is_valid()) {
        d["herb"] = herb->to_dict();
    }
    d["station"] = (int)station;
    d["quantity"] = quantity;
    return d;
}

void InventorySlot::from_dict(const Dictionary& p_dict) {
    occupied = p_dict.get("occupied", false);
    is_herb = p_dict.get("is_herb", false);
    if (is_herb && p_dict.has("herb")) {
        herb.instantiate();
        herb->from_dict(p_dict["herb"]);
    }
    station = (ItemType)(int)p_dict.get("station", (int)ITEM_COUNT);
    quantity = p_dict.get("quantity", 0);
}

void InventorySlot::_bind_methods() {
    ClassDB::bind_method(D_METHOD("clear"), &InventorySlot::clear);
    ClassDB::bind_method(D_METHOD("to_dict"), &InventorySlot::to_dict);
    ClassDB::bind_method(D_METHOD("from_dict", "dict"), &InventorySlot::from_dict);

    ClassDB::bind_method(D_METHOD("set_occupied", "occupied"), &InventorySlot::set_occupied);
    ClassDB::bind_method(D_METHOD("get_occupied"), &InventorySlot::get_occupied);
    ClassDB::bind_method(D_METHOD("set_is_herb", "is_herb"), &InventorySlot::set_is_herb);
    ClassDB::bind_method(D_METHOD("get_is_herb"), &InventorySlot::get_is_herb);
    ClassDB::bind_method(D_METHOD("set_herb", "herb"), &InventorySlot::set_herb);
    ClassDB::bind_method(D_METHOD("get_herb"), &InventorySlot::get_herb);
    ClassDB::bind_method(D_METHOD("set_station", "station"), &InventorySlot::set_station);
    ClassDB::bind_method(D_METHOD("get_station"), &InventorySlot::get_station);
    ClassDB::bind_method(D_METHOD("set_quantity", "quantity"), &InventorySlot::set_quantity);
    ClassDB::bind_method(D_METHOD("get_quantity"), &InventorySlot::get_quantity);
    
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "occupied"), "set_occupied", "get_occupied");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_herb"), "set_is_herb", "get_is_herb");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "herb", PROPERTY_HINT_RESOURCE_TYPE, "HarvestItem"), "set_herb", "get_herb");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "station", PROPERTY_HINT_ENUM, "Fireplace,DryingRack,Mortar,MacerationJar,CompostBin,WorkBench,Alembic,Flask,Glassblowing,DistillTrain,Soxhlet,Pelican,Retort,Terrarium,Bookshelf,Chest,Mailbox"), "set_station", "get_station");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "quantity"), "set_quantity", "get_quantity");
}

// --- Inventory ---

Inventory::Inventory() {
    m_slots.resize(TOTAL_SLOTS);
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        m_slots[i].instantiate();
    }
}

Inventory::~Inventory() {}

Ref<InventorySlot> Inventory::get_slot(int p_index) const {
    ERR_FAIL_INDEX_V(p_index, TOTAL_SLOTS, Ref<InventorySlot>());
    return m_slots[p_index];
}

void Inventory::set_slot(int p_index, const Ref<InventorySlot>& p_slot) {
    ERR_FAIL_INDEX(p_index, TOTAL_SLOTS);
    m_slots[p_index] = p_slot;
}

void Inventory::swap_slots(int p_idx_a, int p_idx_b) {
    ERR_FAIL_INDEX(p_idx_a, TOTAL_SLOTS);
    ERR_FAIL_INDEX(p_idx_b, TOTAL_SLOTS);
    Ref<InventorySlot> temp = m_slots[p_idx_a];
    m_slots[p_idx_a] = m_slots[p_idx_b];
    m_slots[p_idx_b] = temp;
}

void Inventory::clear_slot(int p_index) {
    ERR_FAIL_INDEX(p_index, TOTAL_SLOTS);
    m_slots[p_index]->clear();
}

int Inventory::find_first_empty_slot() const {
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (!m_slots[i]->occupied) return i;
    }
    return -1;
}

int Inventory::find_stack(ItemType p_type) const {
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (m_slots[i]->occupied && !m_slots[i]->is_herb && m_slots[i]->station == p_type) {
            return i;
        }
    }
    return -1;
}

void Inventory::add_item(ItemType p_type, int p_quantity) {
    int idx = find_stack(p_type);
    if (idx != -1) {
        m_slots[idx]->quantity += p_quantity;
    } else {
        idx = find_first_empty_slot();
        if (idx != -1) {
            m_slots[idx]->occupied = true;
            m_slots[idx]->is_herb = false;
            m_slots[idx]->station = p_type;
            m_slots[idx]->quantity = p_quantity;
        }
    }
}

bool Inventory::remove_item(ItemType p_type, int p_quantity) {
    int idx = find_stack(p_type);
    if (idx != -1 && m_slots[idx]->quantity >= p_quantity) {
        m_slots[idx]->quantity -= p_quantity;
        if (m_slots[idx]->quantity <= 0) {
            m_slots[idx]->clear();
        }
        return true;
    }
    return false;
}

void Inventory::add_harvest_item(const Ref<HarvestItem>& p_item) {
    int idx = find_first_empty_slot();
    if (idx != -1) {
        m_slots[idx]->occupied = true;
        m_slots[idx]->is_herb = true;
        m_slots[idx]->herb = p_item;
    }
}

bool Inventory::remove_harvest_item(const String& p_plant_name, PlantStage p_stage) {
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        if (m_slots[i]->occupied && m_slots[i]->is_herb) {
            if (m_slots[i]->herb->plant_name == p_plant_name && m_slots[i]->herb->stage == p_stage) {
                m_slots[i]->clear();
                return true;
            }
        }
    }
    return false;
}

String Inventory::serialise() const {
    json j = json::array();
    for (int i = 0; i < TOTAL_SLOTS; i++) {
        json slot;
        slot["occupied"] = m_slots[i]->occupied;
        slot["isHerb"] = m_slots[i]->is_herb;
        if (m_slots[i]->is_herb && m_slots[i]->herb.is_valid()) {
            slot["plant"] = m_slots[i]->herb->plant_name.utf8().get_data();
            slot["stage"] = (int)m_slots[i]->herb->stage;
            slot["quality"] = (int)m_slots[i]->herb->quality;
        } else {
            slot["station"] = (int)m_slots[i]->station;
            slot["quantity"] = m_slots[i]->quantity;
        }
        j.push_back(slot);
    }
    return String(j.dump().c_str());
}

void Inventory::deserialise(const String& p_json) {
    try {
        json j = json::parse(p_json.utf8().get_data());
        for (int i = 0; i < TOTAL_SLOTS && i < (int)j.size(); i++) {
            m_slots[i]->clear();
            const auto& slot = j[i];
            m_slots[i]->occupied = slot.value("occupied", false);
            m_slots[i]->is_herb = slot.value("isHerb", false);
            if (m_slots[i]->is_herb) {
                m_slots[i]->herb.instantiate();
                m_slots[i]->herb->plant_name = String(slot.value("plant", "").c_str());
                m_slots[i]->herb->stage = (PlantStage)slot.value("stage", 0);
                m_slots[i]->herb->quality = (HarvestQuality)slot.value("quality", 1);
            } else {
                m_slots[i]->station = (ItemType)slot.value("station", (int)ITEM_COUNT);
                m_slots[i]->quantity = slot.value("quantity", 0);
            }
        }
    } catch (...) {
        UtilityFunctions::printerr("Failed to deserialise inventory.");
    }
}

void Inventory::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_slot", "index"), &Inventory::get_slot);
    ClassDB::bind_method(D_METHOD("swap_slots", "idx_a", "idx_b"), &Inventory::swap_slots);
    ClassDB::bind_method(D_METHOD("add_item", "type", "quantity"), &Inventory::add_item, DEFVAL(1));
    ClassDB::bind_method(D_METHOD("remove_item", "type", "quantity"), &Inventory::remove_item, DEFVAL(1));
    ClassDB::bind_method(D_METHOD("add_harvest_item", "item"), &Inventory::add_harvest_item);
    ClassDB::bind_method(D_METHOD("serialise"), &Inventory::serialise);
    ClassDB::bind_method(D_METHOD("deserialise", "json"), &Inventory::deserialise);
}

} // namespace godot
