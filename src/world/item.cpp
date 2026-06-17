#include "item.h"

namespace godot {

HarvestItem::HarvestItem() : stage(STAGE_FRESH), quality(QUALITY_STANDARD) {}
HarvestItem::~HarvestItem() {}

void HarvestItem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_display_name"), &HarvestItem::get_display_name);
    ClassDB::bind_static_method("HarvestItem", D_METHOD("get_stage_name", "stage"), &HarvestItem::get_stage_name);
    
    ClassDB::bind_method(D_METHOD("to_dict"), &HarvestItem::to_dict);
    ClassDB::bind_method(D_METHOD("from_dict", "dict"), &HarvestItem::from_dict);

    ClassDB::bind_method(D_METHOD("set_plant_name", "name"), &HarvestItem::set_plant_name);
    ClassDB::bind_method(D_METHOD("get_plant_name"), &HarvestItem::get_plant_name);
    ClassDB::bind_method(D_METHOD("set_stage", "stage"), &HarvestItem::set_stage);
    ClassDB::bind_method(D_METHOD("get_stage"), &HarvestItem::get_stage);
    ClassDB::bind_method(D_METHOD("set_quality", "quality"), &HarvestItem::set_quality);
    ClassDB::bind_method(D_METHOD("get_quality"), &HarvestItem::get_quality);

    // PlantStage enum
    BIND_CONSTANT(STAGE_FRESH);
    BIND_CONSTANT(STAGE_DRIED);
    BIND_CONSTANT(STAGE_GROUND);
    BIND_CONSTANT(STAGE_SPENT);
    BIND_CONSTANT(STAGE_SPIRITS);
    BIND_CONSTANT(STAGE_SALT);
    BIND_CONSTANT(STAGE_TINCTURE);

    // HarvestQuality enum
    BIND_CONSTANT(QUALITY_CELESTIAL);
    BIND_CONSTANT(QUALITY_PRISTINE);
    BIND_CONSTANT(QUALITY_STANDARD);
    BIND_CONSTANT(QUALITY_STRESSED);
    BIND_CONSTANT(QUALITY_DEBASED);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "plant_name"), "set_plant_name", "get_plant_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "stage", PROPERTY_HINT_ENUM, "Fresh,Dried,Ground,Spent,Spirits,Salt,Tincture"), "set_stage", "get_stage");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "quality", PROPERTY_HINT_ENUM, "Celestial,Pristine,Standard,Stressed,Debased"), "set_quality", "get_quality");
}

String HarvestItem::get_stage_name(PlantStage p_stage) {
    switch (p_stage) {
        case STAGE_FRESH:    return "Fresh";
        case STAGE_DRIED:    return "Dried";
        case STAGE_GROUND:   return "Ground";
        case STAGE_SPENT:    return "Spent";
        case STAGE_SPIRITS:  return "Spirits";
        case STAGE_SALT:     return "Salt";
        case STAGE_TINCTURE: return "Tincture";
    }
    return "Unknown";
}

String HarvestItem::get_display_name() const {
    String qual_str;
    switch (quality) {
        case QUALITY_PRISTINE:  qual_str = "Pristine"; break;
        case QUALITY_STANDARD:  qual_str = "Standard"; break;
        case QUALITY_DEBASED:   qual_str = "Debased";  break;
        case QUALITY_CELESTIAL: qual_str = "Celestial"; break;
        case QUALITY_STRESSED:  qual_str = "Stressed"; break;
    }
    
    String name;
    if (stage == STAGE_SPIRITS) {
        name = "Spirits of " + plant_name;
    } else if (stage == STAGE_SALT) {
        name = "Salt of " + plant_name;
    } else if (stage == STAGE_TINCTURE) {
        name = "Tincture of " + plant_name;
    } else {
        name = get_stage_name(stage) + " " + plant_name;
    }
    
    return name + " (" + qual_str + ")";
}

Dictionary HarvestItem::to_dict() const {
    Dictionary d;
    d["plant_name"] = plant_name;
    d["stage"] = (int)stage;
    d["quality"] = (int)quality;
    return d;
}

void HarvestItem::from_dict(const Dictionary& p_dict) {
    plant_name = p_dict.get("plant_name", "");
    stage = (PlantStage)(int)p_dict.get("stage", 0);
    quality = (HarvestQuality)(int)p_dict.get("quality", 1);
}

// --- ItemDefinition ---

ItemDefinition::ItemDefinition() : type(ITEM_COUNT), tier(TIER_1_FORAGER), tile_width(1), tile_height(1), can_place_indoors(true), can_place_outdoors(true) {}
ItemDefinition::~ItemDefinition() {}

void ItemDefinition::_bind_methods() {}

// --- ItemDB (Static Database) ---

void ItemDB::_bind_methods() {
    ClassDB::bind_static_method("ItemDB", D_METHOD("get_item_definition", "type"), &ItemDB::get_item_definition);
}

Ref<ItemDefinition> ItemDB::get_item_definition(ItemType p_type) {
    Ref<ItemDefinition> def;
    def.instantiate();
    def->type = p_type;

    switch (p_type) {
        case ITEM_FIREPLACE:
            def->display_name = "Fireplace";
            def->description = "A stone hearth for warming and calcination.";
            def->tile_width = 2; def->tile_height = 2;
            break;
        case ITEM_DRYING_RACK:
            def->display_name = "Drying Rack";
            def->description = "Hangs herbs to dry.";
            def->tile_width = 2; def->tile_height = 1;
            break;
        case ITEM_MORTAR_AND_PESTLE:
            def->display_name = "Mortar & Pestle";
            def->description = "For grinding dried herbs.";
            def->tile_width = 1; def->tile_height = 1;
            break;
        // ... (Rest of the items would be populated here)
        default:
            def->display_name = "Unknown Apparatus";
            break;
    }
    return def;
}

} // namespace godot
