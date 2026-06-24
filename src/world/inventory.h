#ifndef FLORA_PHILOSOPHICA_WORLD_INVENTORY_H
#define FLORA_PHILOSOPHICA_WORLD_INVENTORY_H

#include "item.h"
#include <vector>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

class InventorySlot : public RefCounted {
    GDCLASS(InventorySlot, RefCounted)

public:
    bool occupied = false;
    bool is_herb = false;
    Ref<HarvestItem> herb;
    ItemType station = ITEM_COUNT;
    int quantity = 0;

    InventorySlot();
    ~InventorySlot();

    void set_occupied(bool p_occupied) { occupied = p_occupied; }
    bool get_occupied() const { return occupied; }

    void set_is_herb(bool p_is_herb) { is_herb = p_is_herb; }
    bool get_is_herb() const { return is_herb; }

    void set_herb(const Ref<HarvestItem>& p_herb) { herb = p_herb; }
    Ref<HarvestItem> get_herb() const { return herb; }

    void set_station(ItemType p_station) { station = p_station; }
    ItemType get_station() const { return station; }

    void set_quantity(int p_quantity) { quantity = p_quantity; }
    int get_quantity() const { return quantity; }

    void clear();
    Dictionary to_dict() const;
    void from_dict(const Dictionary& p_dict);

protected:
    static void _bind_methods();
};

class Inventory : public RefCounted {
    GDCLASS(Inventory, RefCounted)

public:
    static constexpr int TOTAL_SLOTS = 46;

    Inventory();
    ~Inventory();

    Ref<InventorySlot> get_slot(int p_index) const;
    void set_slot(int p_index, const Ref<InventorySlot>& p_slot);
    void swap_slots(int p_idx_a, int p_idx_b);
    void clear_slot(int p_index);

    void add_item(ItemType p_type, int p_quantity = 1);
    bool remove_item(ItemType p_type, int p_quantity = 1);
    
    void add_harvest_item(const Ref<HarvestItem>& p_item);
    bool remove_harvest_item(const String& p_plant_name, PlantStage p_stage);

    String serialise() const;
    void deserialise(const String& p_json);

protected:
    static void _bind_methods();

private:
    std::vector<Ref<InventorySlot>> m_slots;

    int find_first_empty_slot() const;
    int find_stack(ItemType p_type) const;
};

} // namespace godot

#endif // FLORA_PHILOSOPHICA_WORLD_INVENTORY_H
