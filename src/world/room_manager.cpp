#include "room_manager.h"
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

RoomManager::RoomManager() {
    m_rooms.resize(ROOM_COUNT);
}

RoomManager::~RoomManager() {}

void RoomManager::add_placed_item(RoomID p_room, const Ref<PlacedItem>& p_item) {
    ERR_FAIL_INDEX(p_room, ROOM_COUNT);
    m_rooms[p_room].append(p_item);
}

void RoomManager::remove_placed_item(RoomID p_room, const Ref<PlacedItem>& p_item) {
    ERR_FAIL_INDEX(p_room, ROOM_COUNT);
    int idx = m_rooms[p_room].find(p_item);
    if (idx != -1) {
        m_rooms[p_room].remove_at(idx);
    }
}

TypedArray<PlacedItem> RoomManager::get_placed_items(RoomID p_room) const {
    ERR_FAIL_INDEX_V(p_room, ROOM_COUNT, TypedArray<PlacedItem>());
    return m_rooms[p_room];
}

String RoomManager::serialise() const {
    Dictionary data;
    Array rooms_array;

    for (int i = 0; i < ROOM_COUNT; i++) {
        Array room_items;
        for (int k = 0; k < m_rooms[i].size(); k++) {
            Ref<PlacedItem> item = m_rooms[i][k];
            if (item.is_valid()) {
                room_items.append(item->to_dict());
            }
        }
        rooms_array.append(room_items);
    }
    data["rooms"] = rooms_array;
    return JSON::stringify(data);
}

void RoomManager::deserialise(const String& p_json) {
    Variant v = JSON::parse_string(p_json);
    if (v.get_type() != Variant::DICTIONARY) {
        UtilityFunctions::printerr("Failed to deserialise RoomManager: Invalid JSON.");
        return;
    }

    Dictionary data = v;
    if (data.has("rooms")) {
        Array rooms_array = data["rooms"];
        for (int i = 0; i < ROOM_COUNT && i < rooms_array.size(); i++) {
            m_rooms[i].clear();
            Array items_array = rooms_array[i];
            for (int k = 0; k < items_array.size(); k++) {
                Ref<PlacedItem> item;
                item.instantiate();
                item->from_dict(items_array[k]);
                m_rooms[i].append(item);
            }
        }
    }
}

void RoomManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add_placed_item", "room", "item"), &RoomManager::add_placed_item);
    ClassDB::bind_method(D_METHOD("remove_placed_item", "room", "item"), &RoomManager::remove_placed_item);
    ClassDB::bind_method(D_METHOD("get_placed_items", "room"), &RoomManager::get_placed_items);
    ClassDB::bind_method(D_METHOD("serialise"), &RoomManager::serialise);
    ClassDB::bind_method(D_METHOD("deserialise", "json"), &RoomManager::deserialise);

    BIND_ENUM_CONSTANT(ROOM_EXTERIOR);
    BIND_ENUM_CONSTANT(ROOM_CABIN_MAIN);
    BIND_ENUM_CONSTANT(ROOM_CABIN_LOFT);
    BIND_ENUM_CONSTANT(ROOM_GARDEN);
}

} // namespace godot
