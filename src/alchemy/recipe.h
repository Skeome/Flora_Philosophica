#ifndef FLORA_PHILOSOPHICA_ALCHEMY_RECIPE_H
#define FLORA_PHILOSOPHICA_ALCHEMY_RECIPE_H

#include "../world/item.h"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

class Recipe : public Resource {
    GDCLASS(Recipe, Resource)

private:
    Array inputs; 
    Array outputs; 
    float base_processing_time;
    ItemType required_station;

public:
    Recipe();
    ~Recipe();

    void set_inputs(const Array& p_inputs);
    Array get_inputs() const;

    void set_outputs(const Array& p_outputs);
    Array get_outputs() const;

    void set_base_processing_time(float p_time);
    float get_base_processing_time() const;

    void set_required_station(ItemType p_station);
    ItemType get_required_station() const;

protected:
    static void _bind_methods();
};

} // namespace godot

#endif // FLORA_PHILOSOPHICA_ALCHEMY_RECIPE_H
