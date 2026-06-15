#include "plant_db.h"

namespace godot {

PlantData::PlantData() : ruler(PlanetaryHourCalculator::SUN) {}
PlantData::~PlantData() {}

void PlantData::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_name", "name"), &PlantData::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &PlantData::get_name);
    ClassDB::bind_method(D_METHOD("set_ruler", "ruler"), &PlantData::set_ruler);
    ClassDB::bind_method(D_METHOD("get_ruler"), &PlantData::get_ruler);
    ClassDB::bind_method(D_METHOD("set_element", "element"), &PlantData::set_element);
    ClassDB::bind_method(D_METHOD("get_element"), &PlantData::get_element);
    ClassDB::bind_method(D_METHOD("set_properties", "properties"), &PlantData::set_properties);
    ClassDB::bind_method(D_METHOD("get_properties"), &PlantData::get_properties);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "ruler", PROPERTY_HINT_ENUM, "Saturn,Jupiter,Mars,Sun,Venus,Mercury,Moon"), "set_ruler", "get_ruler");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "element"), "set_element", "get_element");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "properties"), "set_properties", "get_properties");
}

namespace {
    static TypedArray<PlantData> get_botanical_entries() {
        static TypedArray<PlantData> entries;
        if (entries.size() > 0) return entries;

        struct RawData {
            String name;
            PlanetaryHourCalculator::Planet ruler;
            String element;
            String props;
        };

        RawData raw[] = {
            {"St. John's Wort", PlanetaryHourCalculator::SUN, "Fire", "Under the Celestial influence of the Sun, and Element of Fire. It is a singular wound herb, clearing melancholy, healing burns, and driving away evil spirits."},
            {"Mugwort", PlanetaryHourCalculator::MOON, "Water", "An herb of the Moon. Placed under the Water element. It is excellent for clearing obstructions, inducing vivid dreams, and aiding female complaints."},
            {"Nettle", PlanetaryHourCalculator::MARS, "Fire", "An herb of Mars. Placed under the Fire element. It is hot, dry, and stinging. Excellent for purifying the blood, easing joint pain, and stimulating vitality."},
            {"Yarrow", PlanetaryHourCalculator::VENUS, "Earth", "Under the dominion of Venus. Placed under the Earth element. An excellent styptic wound herb, it stops bleeding and opens pores to relieve fevers."},
            {"Lavender", PlanetaryHourCalculator::MERCURY, "Air", "Ruled by Mercury. Placed under the Air element. It is highly aromatic, calming the brain, easing headaches, and restoring nervous energy."},
            {"Dandelion", PlanetaryHourCalculator::JUPITER, "Air", "An herb of Jupiter. Placed under the Air element. Singularly powerful for cleansing the liver and kidneys, acting as a gentle physical purifier."},
            {"Comfrey", PlanetaryHourCalculator::SATURN, "Earth", "Under the heavy dominion of Saturn. Placed under the Earth element. Commonly known as knit-bone; it excels at mending fractures, healing deep wounds, and strengthening bones."}
        };

        for (const auto& r : raw) {
            Ref<PlantData> p;
            p.instantiate();
            p->name = r.name;
            p->ruler = r.ruler;
            p->element = r.element;
            p->properties = r.props;
            entries.append(p);
        }

        return entries;
    }
}

void PlantDatabase::_bind_methods() {
    ClassDB::bind_static_method("PlantDatabase", D_METHOD("get_plant", "name"), &PlantDatabase::get_plant);
    ClassDB::bind_static_method("PlantDatabase", D_METHOD("get_all_plants"), &PlantDatabase::get_all_plants);
}

Ref<PlantData> PlantDatabase::get_plant(const String& p_name) {
    TypedArray<PlantData> all = get_botanical_entries();
    for (int i = 0; i < all.size(); i++) {
        Ref<PlantData> p = all[i];
        if (p->name == p_name) return p;
    }
    return Ref<PlantData>();
}

TypedArray<PlantData> PlantDatabase::get_all_plants() {
    return get_botanical_entries();
}

} // namespace godot
