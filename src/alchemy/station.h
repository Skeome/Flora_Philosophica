#ifndef FLORA_PHILOSOPHICA_ALCHEMY_STATION_H
#define FLORA_PHILOSOPHICA_ALCHEMY_STATION_H

#include "../world/item.h"
#include "recipe.h"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Station : public Node {
    GDCLASS(Station, Node)

private:
    ItemType station_type;
    float efficiency_multiplier;
    Array current_inputs;
    Ref<Recipe> current_recipe;
    float processing_time_remaining;
    bool is_processing;

public:
    Station();
    ~Station();

    void set_station_type(ItemType p_type);
    ItemType get_station_type() const;

    void set_efficiency_multiplier(float p_mult);
    float get_efficiency_multiplier() const;

    Array get_current_inputs() const;
    Ref<Recipe> get_current_recipe() const;
    float get_processing_time_remaining() const;
    bool get_is_processing() const;

    bool try_start_processing(const Ref<Recipe>& p_recipe, const Array& p_inputs);
    void apply_minigame_progress(float p_amount); // e.g. 300 seconds for 5 minutes
    void finish_processing();
    void abort_processing();

    void _process(double delta) override;

protected:
    static void _bind_methods();
};

} // namespace godot

#endif
