#include "plant_db.h"
#include <algorithm>

namespace FloraPhilosophica {
namespace Alchemy {

namespace {
    // Static inline dataset representing our botanical entries.
    // Attributions are sourced from Culpeper's Complete Herbal.
    const std::vector<PlantData> G_BOTANICAL_ENTRIES = {
        {
            "St. John's Wort",
            Core::Planet::Sun,
            "Fire",
            "Under the Celestial influence of the Sun, and Element of Fire. It is a singular wound herb, clearing melancholy, healing burns, and driving away evil spirits."
        },
        {
            "Mugwort",
            Core::Planet::Moon,
            "Water",
            "An herb of the Moon. Placed under the Water element. It is excellent for clearing obstructions, inducing vivid dreams, and aiding female complaints."
        },
        {
            "Nettle",
            Core::Planet::Mars,
            "Fire",
            "An herb of Mars. Placed under the Fire element. It is hot, dry, and stinging. Excellent for purifying the blood, easing joint pain, and stimulating vitality."
        },
        {
            "Yarrow",
            Core::Planet::Venus,
            "Earth",
            "Under the dominion of Venus. Placed under the Earth element. An excellent styptic wound herb, it stops bleeding and opens pores to relieve fevers."
        },
        {
            "Lavender",
            Core::Planet::Mercury,
            "Air",
            "Ruled by Mercury. Placed under the Air element. It is highly aromatic, calming the brain, easing headaches, and restoring nervous energy."
        },
        {
            "Dandelion",
            Core::Planet::Jupiter,
            "Air",
            "An herb of Jupiter. Placed under the Air element. Singularly powerful for cleansing the liver and kidneys, acting as a gentle physical purifier."
        },
        {
            "Comfrey",
            Core::Planet::Saturn,
            "Earth",
            "Under the heavy dominion of Saturn. Placed under the Earth element. Commonly known as knit-bone; it excels at mending fractures, healing deep wounds, and strengthening bones."
        }
    };
}

const PlantData* PlantDatabase::GetPlant(const std::string& name) {
    auto it = std::find_if(G_BOTANICAL_ENTRIES.begin(), G_BOTANICAL_ENTRIES.end(),
        [&name](const PlantData& data) {
            return data.name == name;
        }
    );

    if (it != G_BOTANICAL_ENTRIES.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<PlantData> PlantDatabase::GetAllPlants() {
    return G_BOTANICAL_ENTRIES;
}

} // namespace Alchemy
} // namespace FloraPhilosophica