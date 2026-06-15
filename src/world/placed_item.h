#ifndef FLORA_PHILOSOPHICA_WORLD_PLACED_ITEM_H
#define FLORA_PHILOSOPHICA_WORLD_PLACED_ITEM_H

#include "item.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

enum InteractionResult {
    INTERACT_NONE,
    INTERACT_OPEN_APPARATUS,
    INTERACT_OPEN_STORAGE,
    INTERACT_OPEN_COMPOST,
    INTERACT_INSPECT_DECORATION,
    INTERACT_OPEN_MAILBOX
};

class PlacedItem : public RefCounted {
    GDCLASS(PlacedItem, RefCounted)

public:
    PlacedItem();
    ~PlacedItem();

    void init(ItemType p_type, int p_tile_x, int p_tile_y);

    // --- Apparatus Logic ---
    bool load_harvest_item(const Ref<HarvestItem>& p_item, int64_t p_now_utc);
    bool is_process_complete(int64_t p_now_utc) const;
    Ref<HarvestItem> unload_processed_item(int64_t p_now_utc);
    float get_progress(int64_t p_now_utc) const;
    
    InteractionResult interact();
    String get_inspection_message() const;

    // --- Getters/Setters ---
    ItemType get_item_type() const { return type; }
    int get_tile_x() const { return tile_x; }
    int get_tile_y() const { return tile_y; }
    bool is_discovered() const { return discovered; }
    void set_discovered(bool p_discovered) { discovered = p_discovered; }

    // --- Serialization ---
    Dictionary to_dict() const;
    void from_dict(const Dictionary& p_dict);

protected:
    static void _bind_methods();

private:
    ItemType type;
    int tile_x;
    int tile_y;
    bool discovered;

    // Apparatus state
    bool occupied;
    Ref<HarvestItem> loaded_item;
    int64_t process_start_utc;
    int64_t process_duration_sec;

    // Duration constants (seconds)
    static constexpr int64_t DRYING_DURATION_SEC     = 2 * 3600;
    static constexpr int64_t MACERATION_DURATION_SEC = 1 * 3600;
};

} // namespace godot

VARIANT_ENUM_CAST(godot::InteractionResult);

#endif // FLORA_PHILOSOPHICA_WORLD_PLACED_ITEM_H
