#ifndef FLORA_PHILOSOPHICA_ALCHEMY_PLANT_DB_H
#define FLORA_PHILOSOPHICA_ALCHEMY_PLANT_DB_H

#include "core/clock.h"
#include <string>
#include <vector>

namespace FloraPhilosophica {
namespace Alchemy {

// Holds alchemical data and Culpeper attributions for a botanical species.
struct PlantData {
    std::string name;
    Core::Planet ruler;          // The ruling planet of the plant
    std::string element;        // Elemental association (Fire, Water, Air, Earth)
    std::string properties;     // Historical Culpeper properties
};

class PlantDatabase {
public:
    // Retrieves details for a specific plant by its name key
    static const PlantData* GetPlant(const std::string& name);

    // Retrieves all registered plants in the botanical database
    static std::vector<PlantData> GetAllPlants();
};

} // namespace Alchemy
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_ALCHEMY_PLANT_DB_H