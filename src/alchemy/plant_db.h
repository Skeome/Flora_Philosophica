#ifndef FLORA_PHILOSOPHICA_ALCHEMY_PLANT_DB_H
#define FLORA_PHILOSOPHICA_ALCHEMY_PLANT_DB_H

#include "core/clock.h"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

namespace godot {

class PlantData : public RefCounted {
    GDCLASS(PlantData, RefCounted)

public:
    String name;
    PlanetaryHourCalculator::Planet ruler;
    String element;
    String properties;

    PlantData();
    ~PlantData();

    void set_name(const String& p_name) { name = p_name; }
    String get_name() const { return name; }

    void set_ruler(PlanetaryHourCalculator::Planet p_ruler) { ruler = p_ruler; }
    PlanetaryHourCalculator::Planet get_ruler() const { return ruler; }

    void set_element(const String& p_element) { element = p_element; }
    String get_element() const { return element; }

    void set_properties(const String& p_properties) { properties = p_properties; }
    String get_properties() const { return properties; }

protected:
    static void _bind_methods();
};

class PlantDatabase : public Object {
    GDCLASS(PlantDatabase, Object)

public:
    static Ref<PlantData> get_plant(const String& p_name);
    static TypedArray<PlantData> get_all_plants();

protected:
    static void _bind_methods();
};

} // namespace godot

#endif // FLORA_PHILOSOPHICA_ALCHEMY_PLANT_DB_H
