#ifndef FLORA_PHILOSOPHICA_WORLD_ITEM_H
#define FLORA_PHILOSOPHICA_WORLD_ITEM_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

enum PlantStage {
    STAGE_FRESH = 0,
    STAGE_DRIED = 1,
    STAGE_GROUND = 2,
    STAGE_SPENT = 3,
    STAGE_SPIRITS = 4,
    STAGE_SALT = 5,
    STAGE_TINCTURE = 6,
    STAGE_CALX_BLACK = 7,
    STAGE_CALX_BLACK_GROUND = 8,
    STAGE_CALX_GREY = 9,
    STAGE_CALX_GREY_GROUND = 10,
    STAGE_CALX_WHITE = 11,
    STAGE_SALT_PURIFIED = 12,
    STAGE_CALX_LIGHT_GREY = 13,
    STAGE_CALX_LIGHT_GREY_GROUND = 14,
    STAGE_TINCTURE_BASIC = 15,
    STAGE_TINCTURE_SPAGYRIC = 16,
    STAGE_ELIXIR = 17,
    STAGE_TINCTURE_MAGISTERY = 18,
    STAGE_TINCTURE_ENS = 19,
    STAGE_TINCTURE_PRIMUM_ENS = 20,
    STAGE_PLANT_STONE = 21,
};

enum HarvestQuality {
    QUALITY_CELESTIAL = 0,
    QUALITY_PRISTINE = 1,
    QUALITY_STANDARD = 2,
    QUALITY_STRESSED = 3,
    QUALITY_DEBASED = 4,
};

enum ItemType {
    ITEM_FIREPLACE = 0,
    ITEM_DRYING_RACK = 1,
    ITEM_MORTAR_AND_PESTLE = 2,
    ITEM_MACERATION_JAR = 3,
    ITEM_COMPOST_BIN = 4,
    ITEM_WORK_BENCH = 5,
    ITEM_COPPER_ALEMBIC = 6,
    ITEM_GLASS_FLASK = 7,
    ITEM_GLASSBLOWING_STATION = 8,
    ITEM_DISTILLATION_TRAIN = 9,
    ITEM_SOXHLET_EXTRACTOR = 10,
    ITEM_PELICAN_FLASK = 11,
    ITEM_RETORT_TRAIN = 12,
    ITEM_TERRARIUM = 13,
    ITEM_BOOKSHELF = 14,
    ITEM_STORAGE_CHEST = 15,
    ITEM_MAILBOX_POST = 16,
    ITEM_FURNACE = 17,
    ITEM_LEACHING_DISH = 18,
    ITEM_COUNT = 19
};

enum EquipmentTier {
    TIER_1_FORAGER = 1,
    TIER_2_HERBALIST = 2,
    TIER_3_PARACELSIAN = 3,
    TIER_4_ADEPT = 4
};

class HarvestItem : public RefCounted {
    GDCLASS(HarvestItem, RefCounted)

public:
    String plant_name;
    PlantStage stage;
    HarvestQuality quality;

    HarvestItem();
    ~HarvestItem();

    void set_plant_name(const String& p_name) { plant_name = p_name; }
    String get_plant_name() const { return plant_name; }

    void set_stage(PlantStage p_stage) { stage = p_stage; }
    PlantStage get_stage() const { return stage; }

    void set_quality(HarvestQuality p_quality) { quality = p_quality; }
    HarvestQuality get_quality() const { return quality; }

    String get_display_name() const;
    static String get_stage_name(PlantStage p_stage);

    Dictionary to_dict() const;
    void from_dict(const Dictionary& p_dict);

protected:
    static void _bind_methods();
};

class ItemDefinition : public RefCounted {
    GDCLASS(ItemDefinition, RefCounted)

public:
    ItemType type;
    String display_name;
    String description;
    EquipmentTier tier;
    int tile_width;
    int tile_height;
    bool can_place_indoors;
    bool          can_place_outdoors;

    ItemDefinition();
    ~ItemDefinition();

protected:
    static void _bind_methods();
};

class ItemDB : public Object {
    GDCLASS(ItemDB, Object)

public:
    static Ref<ItemDefinition> get_item_definition(ItemType p_type);

protected:
    static void _bind_methods();
};

} // namespace godot

VARIANT_ENUM_CAST(godot::PlantStage);
VARIANT_ENUM_CAST(godot::HarvestQuality);
VARIANT_ENUM_CAST(godot::ItemType);
VARIANT_ENUM_CAST(godot::EquipmentTier);

#endif // FLORA_PHILOSOPHICA_WORLD_ITEM_H
