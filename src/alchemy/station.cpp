#include "station.h"
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

Station::Station() {
    station_type = ITEM_MORTAR_AND_PESTLE;
    efficiency_multiplier = 1.0f;
    processing_time_remaining = 0.0f;
    is_processing = false;
}

Station::~Station() {}

void Station::set_station_type(ItemType p_type) { station_type = p_type; }
ItemType Station::get_station_type() const { return station_type; }

void Station::set_efficiency_multiplier(float p_mult) { efficiency_multiplier = p_mult; }
float Station::get_efficiency_multiplier() const { return efficiency_multiplier; }

Array Station::get_current_inputs() const { return current_inputs; }
Ref<Recipe> Station::get_current_recipe() const { return current_recipe; }
float Station::get_processing_time_remaining() const { return processing_time_remaining; }
bool Station::get_is_processing() const { return is_processing; }

bool Station::try_start_processing(const Ref<Recipe>& p_recipe, const Array& p_inputs) {
    if (is_processing) return false;
    if (p_recipe.is_null()) return false;
    if (p_recipe->get_required_station() != station_type) return false;
    
    // In a full implementation, validate that p_inputs matches p_recipe->get_inputs()
    // For now we assume valid.
    current_recipe = p_recipe;
    current_inputs = p_inputs;
    
    float base_time = current_recipe->get_base_processing_time();
    processing_time_remaining = base_time / efficiency_multiplier;
    is_processing = true;
    
    return true;
}

void Station::apply_minigame_progress(float p_amount) {
    if (!is_processing) return;
    processing_time_remaining -= p_amount;
    if (processing_time_remaining <= 0.0f) {
        finish_processing();
    }
}

void Station::finish_processing() {
    if (!is_processing) return;
    
    // Calculate average quality
    int total_qual = 0;
    int count = 0;
    
    // Calculate quality based on HarvestItems in current_inputs
    for (int i = 0; i < current_inputs.size(); i++) {
        Variant item = current_inputs[i];
        if (item.get_type() == Variant::OBJECT) {
            Object* obj = item;
            HarvestItem* hi = Object::cast_to<HarvestItem>(obj);
            if (hi) {
                total_qual += hi->get_quality();
                count++;
            }
        }
    }
    
    HarvestQuality avg_quality = QUALITY_STANDARD;
    if (count > 0) {
        int avg = total_qual / count;
        // Clip to valid bounds 0-4
        if (avg < 0) avg = 0;
        if (avg > 4) avg = 4;
        avg_quality = (HarvestQuality)avg;
    }
    
    Array generated_outputs;
    Array recipe_outputs = current_recipe->get_outputs();
    
    for (int i = 0; i < recipe_outputs.size(); i++) {
        Dictionary out_dict = recipe_outputs[i];
        
        Ref<HarvestItem> new_item;
        new_item.instantiate();
        
        // We attempt to carry over the plant_name from the first input if possible
        String p_name = "Unknown Plant";
        for (int j = 0; j < current_inputs.size(); j++) {
            Variant in_item = current_inputs[j];
            if (in_item.get_type() == Variant::OBJECT) {
                HarvestItem* hi = Object::cast_to<HarvestItem>((Object*)in_item);
                if (hi && !hi->get_plant_name().is_empty()) {
                    p_name = hi->get_plant_name();
                    break;
                }
            }
        }
        
        new_item->set_plant_name(p_name);
        if (out_dict.has("stage")) {
            new_item->set_stage((PlantStage)(int)out_dict["stage"]);
        }
        new_item->set_quality(avg_quality);
        generated_outputs.push_back(new_item);
    }
    
    current_inputs.clear();
    current_recipe.unref();
    processing_time_remaining = 0.0f;
    is_processing = false;
    
    emit_signal("processing_finished", generated_outputs);
}

void Station::abort_processing() {
    if (!is_processing) return;
    
    current_inputs.clear();
    current_recipe.unref();
    processing_time_remaining = 0.0f;
    is_processing = false;
}

void Station::_process(double delta) {
    if (is_processing) {
        processing_time_remaining -= delta;
        if (processing_time_remaining <= 0.0f) {
            finish_processing();
        }
    }
}

void Station::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_station_type", "type"), &Station::set_station_type);
    ClassDB::bind_method(D_METHOD("get_station_type"), &Station::get_station_type);
    ClassDB::bind_method(D_METHOD("set_efficiency_multiplier", "multiplier"), &Station::set_efficiency_multiplier);
    ClassDB::bind_method(D_METHOD("get_efficiency_multiplier"), &Station::get_efficiency_multiplier);
    
    ClassDB::bind_method(D_METHOD("get_current_inputs"), &Station::get_current_inputs);
    ClassDB::bind_method(D_METHOD("get_current_recipe"), &Station::get_current_recipe);
    ClassDB::bind_method(D_METHOD("get_processing_time_remaining"), &Station::get_processing_time_remaining);
    ClassDB::bind_method(D_METHOD("get_is_processing"), &Station::get_is_processing);
    
    ClassDB::bind_method(D_METHOD("try_start_processing", "recipe", "inputs"), &Station::try_start_processing);
    ClassDB::bind_method(D_METHOD("apply_minigame_progress", "amount"), &Station::apply_minigame_progress);
    ClassDB::bind_method(D_METHOD("finish_processing"), &Station::finish_processing);
    ClassDB::bind_method(D_METHOD("abort_processing"), &Station::abort_processing);

    ADD_SIGNAL(MethodInfo("processing_finished", PropertyInfo(Variant::ARRAY, "outputs")));

    ADD_PROPERTY(PropertyInfo(Variant::INT, "station_type", PROPERTY_HINT_ENUM, "Fireplace,Drying Rack,Mortar And Pestle,Maceration Jar,Compost Bin,Work Bench,Copper Alembic,Glass Flask,Glassblowing Station,Distillation Train,Soxhlet Extractor,Pelican Flask,Retort Train,Terrarium,Bookshelf,Storage Chest,Mailbox Post,Furnace,Leaching Dish"), "set_station_type", "get_station_type");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "efficiency_multiplier"), "set_efficiency_multiplier", "get_efficiency_multiplier");
}

} // namespace godot
