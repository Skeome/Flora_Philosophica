#include "placed_item.h"
#include <algorithm>

namespace godot {

PlacedItem::PlacedItem() 
    : type(ITEM_COUNT), tile_x(0), tile_y(0), discovered(false), 
      autonomous_mode(MODE_CONTINUOUS), accumulated_quality(0.0f),
      occupied(false), process_start_utc(0), process_duration_sec(0) {}

PlacedItem::~PlacedItem() {}

void PlacedItem::init(ItemType p_type, int p_tile_x, int p_tile_y) {
    type = p_type;
    tile_x = p_tile_x;
    tile_y = p_tile_y;
}

void PlacedItem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("init", "type", "tile_x", "tile_y"), &PlacedItem::init);
    ClassDB::bind_method(D_METHOD("load_harvest_item", "item", "now_utc"), &PlacedItem::load_harvest_item);
    ClassDB::bind_method(D_METHOD("is_process_complete", "now_utc"), &PlacedItem::is_process_complete);
    ClassDB::bind_method(D_METHOD("unload_processed_item", "now_utc"), &PlacedItem::unload_processed_item);
    ClassDB::bind_method(D_METHOD("get_progress", "now_utc"), &PlacedItem::get_progress);
    ClassDB::bind_method(D_METHOD("interact"), &PlacedItem::interact);
    ClassDB::bind_method(D_METHOD("get_inspection_message"), &PlacedItem::get_inspection_message);
    
    ClassDB::bind_method(D_METHOD("get_item_type"), &PlacedItem::get_item_type);
    ClassDB::bind_method(D_METHOD("is_discovered"), &PlacedItem::is_discovered);
    ClassDB::bind_method(D_METHOD("set_discovered", "discovered"), &PlacedItem::set_discovered);

    ClassDB::bind_method(D_METHOD("get_autonomous_mode"), &PlacedItem::get_autonomous_mode);
    ClassDB::bind_method(D_METHOD("set_autonomous_mode", "mode"), &PlacedItem::set_autonomous_mode);
    ClassDB::bind_method(D_METHOD("get_accumulated_quality"), &PlacedItem::get_accumulated_quality);
    ClassDB::bind_method(D_METHOD("set_accumulated_quality", "quality"), &PlacedItem::set_accumulated_quality);
    ClassDB::bind_method(D_METHOD("award_quality", "amount"), &PlacedItem::award_quality);
    
    BIND_ENUM_CONSTANT(MODE_STRICT);
    BIND_ENUM_CONSTANT(MODE_SYNERGISTIC);
    BIND_ENUM_CONSTANT(MODE_CONTINUOUS);

    ClassDB::bind_method(D_METHOD("to_dict"), &PlacedItem::to_dict);
    ClassDB::bind_method(D_METHOD("from_dict", "dict"), &PlacedItem::from_dict);
}

bool PlacedItem::load_harvest_item(const Ref<HarvestItem>& p_item, int64_t p_now_utc) {
    if (occupied || !p_item.is_valid()) return false;

    switch (type) {
        case ITEM_DRYING_RACK:
            if (p_item->stage != STAGE_FRESH) return false;
            loaded_item = p_item;
            process_start_utc = p_now_utc;
            process_duration_sec = DRYING_DURATION_SEC;
            occupied = true;
            return true;

        case ITEM_MORTAR_AND_PESTLE:
            if (p_item->stage != STAGE_DRIED) return false;
            loaded_item = p_item;
            process_start_utc = p_now_utc;
            process_duration_sec = MORTAR_DURATION_SEC;
            occupied = true;
            return true;

        case ITEM_MACERATION_JAR:
            if (p_item->stage != STAGE_GROUND && p_item->stage != STAGE_DRIED) return false;
            loaded_item = p_item;
            process_start_utc = p_now_utc;
            process_duration_sec = MACERATION_DURATION_SEC;
            occupied = true;
            return true;

        default:
            return false;
    }
}

bool PlacedItem::is_process_complete(int64_t p_now_utc) const {
    if (!occupied) return false;
    if (process_duration_sec == 0) return true;
    return (p_now_utc - process_start_utc) >= process_duration_sec;
}

Ref<HarvestItem> PlacedItem::unload_processed_item(int64_t p_now_utc) {
    if (!occupied || !is_process_complete(p_now_utc)) return Ref<HarvestItem>();

    Ref<HarvestItem> out = loaded_item;
    
    if (type == ITEM_DRYING_RACK) {
        out->stage = STAGE_DRIED;
    } else if (type == ITEM_MACERATION_JAR) {
        out->stage = STAGE_TINCTURE;
    } else if (type == ITEM_MORTAR_AND_PESTLE) {
        out->stage = STAGE_GROUND;
    }

    occupied = false;
    loaded_item.unref();
    process_start_utc = 0;
    process_duration_sec = 0;
    return out;
}

float PlacedItem::get_progress(int64_t p_now_utc) const {
    if (!occupied) return -1.0f;
    if (process_duration_sec == 0) return 1.0f;
    float elapsed = (float)(p_now_utc - process_start_utc);
    return std::min(elapsed / (float)process_duration_sec, 1.0f);
}

InteractionResult PlacedItem::interact() {
    switch (type) {
        case ITEM_FIREPLACE:
            if (!discovered) {
                discovered = true;
                return INTERACT_INSPECT_DECORATION;
            }
            return INTERACT_OPEN_APPARATUS;

        case ITEM_DRYING_RACK:
        case ITEM_MORTAR_AND_PESTLE:
        case ITEM_MACERATION_JAR:
            return INTERACT_OPEN_APPARATUS;

        case ITEM_STORAGE_CHEST:
            return INTERACT_OPEN_STORAGE;

        case ITEM_MAILBOX_POST:
            return INTERACT_OPEN_MAILBOX;

        case ITEM_BOOKSHELF:
        case ITEM_WORK_BENCH:
            discovered = true;
            return INTERACT_INSPECT_DECORATION;

        default:
            return INTERACT_NONE;
    }
}

String PlacedItem::get_inspection_message() const {
    switch (type) {
        case ITEM_FIREPLACE:
            return "This fireplace... the heat is controllable. It's a furnace!";
        case ITEM_BOOKSHELF:
            return "The shelves are packed with dog-eared herbals. One reads: \"Begin with what the Sun gives freely.\"";
        default:
            return "Nothing of note.";
    }
}

Dictionary PlacedItem::to_dict() const {
    Dictionary d;
    d["type"] = (int)type;
    d["tile_x"] = tile_x;
    d["tile_y"] = tile_y;
    d["discovered"] = discovered;
    d["occupied"] = occupied;
    d["autonomous_mode"] = (int)autonomous_mode;
    d["accumulated_quality"] = accumulated_quality;
    d["process_start_utc"] = process_start_utc;
    d["process_duration_sec"] = process_duration_sec;
    if (occupied && loaded_item.is_valid()) {
        d["loaded_item"] = loaded_item->to_dict();
    }
    return d;
}

void PlacedItem::from_dict(const Dictionary& p_dict) {
    type = (ItemType)(int)p_dict.get("type", (int)ITEM_COUNT);
    tile_x = p_dict.get("tile_x", 0);
    tile_y = p_dict.get("tile_y", 0);
    discovered = p_dict.get("discovered", false);
    occupied = p_dict.get("occupied", false);
    autonomous_mode = (AutonomousMode)(int)p_dict.get("autonomous_mode", (int)MODE_CONTINUOUS);
    accumulated_quality = (float)p_dict.get("accumulated_quality", 0.0f);
    process_start_utc = p_dict.get("process_start_utc", 0);
    process_duration_sec = p_dict.get("process_duration_sec", 0);
    if (occupied && p_dict.has("loaded_item")) {
        loaded_item.instantiate();
        loaded_item->from_dict(p_dict["loaded_item"]);
    }
}

} // namespace godot
