#include "register_types.h"

#include "clock.h" 
#include "orbit_calculator.h"
#include "world/item.h"
#include "world/inventory.h"
#include "world/placed_item.h"
#include "world/room_manager.h"
#include "alchemy/plant_db.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_flora_philosophica_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Core
    ClassDB::register_class<PlanetaryHourCalculator>();
    ClassDB::register_class<PlanetaryOrbitCalculator>();

    // World Logic
    ClassDB::register_class<HarvestItem>();
    ClassDB::register_class<ItemDefinition>();
    ClassDB::register_class<ItemDB>();
    ClassDB::register_class<InventorySlot>();
    ClassDB::register_class<Inventory>();
    ClassDB::register_class<PlacedItem>();
    ClassDB::register_class<RoomManager>();

    // Alchemy
    ClassDB::register_class<PlantData>();
    ClassDB::register_class<PlantDatabase>();
}

void uninitialize_flora_philosophica_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// This name must match the entry_symbol field in your .gdextension manifest
GDExtensionBool GDE_EXPORT flora_philosophica_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_flora_philosophica_module);
    init_obj.register_terminator(uninitialize_flora_philosophica_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
