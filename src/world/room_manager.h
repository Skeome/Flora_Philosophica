#ifndef FLORA_PHILOSOPHICA_WORLD_ROOM_MANAGER_H
#define FLORA_PHILOSOPHICA_WORLD_ROOM_MANAGER_H

#include "placed_item.h"
#include <vector>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

enum RoomID {
    ROOM_EXTERIOR = 0,
    ROOM_CABIN_MAIN = 1,
    ROOM_CABIN_LOFT = 2,
    ROOM_GARDEN = 3,
    ROOM_COUNT = 4
};

class RoomManager : public RefCounted {
    GDCLASS(RoomManager, RefCounted)

public:
    RoomManager();
    ~RoomManager();

    void add_placed_item(RoomID p_room, const Ref<PlacedItem>& p_item);
    void remove_placed_item(RoomID p_room, const Ref<PlacedItem>& p_item);
    TypedArray<PlacedItem> get_placed_items(RoomID p_room) const;

    String serialise() const;
    void deserialise(const String& p_json);

protected:
    static void _bind_methods();

private:
    std::vector<TypedArray<PlacedItem>> m_rooms;
};

} // namespace godot

VARIANT_ENUM_CAST(godot::RoomID);

#endif // FLORA_PHILOSOPHICA_WORLD_ROOM_MANAGER_H
