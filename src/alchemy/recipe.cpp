#include "recipe.h"

namespace godot {

Recipe::Recipe() {
    base_processing_time = 60.0f;
    required_station = ITEM_MORTAR_AND_PESTLE;
}

Recipe::~Recipe() {}

void Recipe::set_inputs(const Array& p_inputs) { inputs = p_inputs; }
Array Recipe::get_inputs() const { return inputs; }

void Recipe::set_outputs(const Array& p_outputs) { outputs = p_outputs; }
Array Recipe::get_outputs() const { return outputs; }

void Recipe::set_base_processing_time(float p_time) { base_processing_time = p_time; }
float Recipe::get_base_processing_time() const { return base_processing_time; }

void Recipe::set_required_station(ItemType p_station) { required_station = p_station; }
ItemType Recipe::get_required_station() const { return required_station; }

void Recipe::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_inputs", "inputs"), &Recipe::set_inputs);
    ClassDB::bind_method(D_METHOD("get_inputs"), &Recipe::get_inputs);
    ClassDB::bind_method(D_METHOD("set_outputs", "outputs"), &Recipe::set_outputs);
    ClassDB::bind_method(D_METHOD("get_outputs"), &Recipe::get_outputs);
    ClassDB::bind_method(D_METHOD("set_base_processing_time", "time"), &Recipe::set_base_processing_time);
    ClassDB::bind_method(D_METHOD("get_base_processing_time"), &Recipe::get_base_processing_time);
    ClassDB::bind_method(D_METHOD("set_required_station", "station"), &Recipe::set_required_station);
    ClassDB::bind_method(D_METHOD("get_required_station"), &Recipe::get_required_station);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "inputs"), "set_inputs", "get_inputs");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "outputs"), "set_outputs", "get_outputs");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "base_processing_time"), "set_base_processing_time", "get_base_processing_time");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "required_station", PROPERTY_HINT_ENUM, "Fireplace,Drying Rack,Mortar And Pestle,Maceration Jar,Compost Bin,Work Bench,Copper Alembic,Glass Flask,Glassblowing Station,Distillation Train,Soxhlet Extractor,Pelican Flask,Retort Train,Terrarium,Bookshelf,Storage Chest,Mailbox Post,Furnace,Leaching Dish"), "set_required_station", "get_required_station");
}

} // namespace godot
