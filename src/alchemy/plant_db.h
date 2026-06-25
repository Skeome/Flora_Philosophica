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
    String sprite_path;
    int choler;
    int sanguine;
    int phlegm;
    int melancholy;

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

    void set_sprite_path(const String& p_path) { sprite_path = p_path; }
    String get_sprite_path() const { return sprite_path; }

    void set_choler(int p_val) { choler = p_val; }
    int get_choler() const { return choler; }

    void set_sanguine(int p_val) { sanguine = p_val; }
    int get_sanguine() const { return sanguine; }

    void set_phlegm(int p_val) { phlegm = p_val; }
    int get_phlegm() const { return phlegm; }

    void set_melancholy(int p_val) { melancholy = p_val; }
    int get_melancholy() const { return melancholy; }

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
