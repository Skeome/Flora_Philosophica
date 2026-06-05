#ifndef FLORA_PHILOSOPHICA_WORLD_PLANT_NODE_H
#define FLORA_PHILOSOPHICA_WORLD_PLANT_NODE_H

#include "raylib.h"
#include "core/clock.h"
#include "alchemy/plant_db.h"
#include "world/item.h"
#include <string>

namespace FloraPhilosophica {
namespace World {

enum class HarvestQuality {
    Pristine,   // Harvested during matching planetary day AND hour
    Standard,   // Harvested during standard times
    Debased     // Harvested during the opposite planetary hour
};

class PlantNode {
public:
    PlantNode(std::string plantName, Vector2 position);

    // Update node state (decrements respawn timer if harvested)
    void Update(float deltaTime);

    // Draw the plant node in the game world
    void Draw() const;

    // Checks if the player is close enough to harvest the plant
    bool CheckCollision(Vector2 playerPosition, float interactionRadius) const;

    // Trigger harvest and calculate quality based on the active astrological state
    HarvestQuality Harvest(Core::Planet dayRuler, Core::Planet hourRuler);

    // Helper to get the astrological opposite/enemy planet
    static Core::Planet GetOppositePlanet(Core::Planet planet);

    // Get string representation of quality
    static std::string GetQualityName(HarvestQuality quality);

    // Returns the ItemType enum value for this plant's harvested material
    // Used to add the plant to the player's inventory after harvesting
    static ItemType GetPlantItemType(const std::string& plantName);

    // Getters
    std::string GetName() const { return m_plantName; }
    Vector2 GetPosition() const { return m_position; }
    bool IsActive() const { return !m_harvested; }
    float GetRespawnTimeRemaining() const { return m_respawnTimer; }

private:
    std::string m_plantName;             // Name of the plant (key to database)
    Vector2 m_position;                  // Position in world space
    bool m_harvested;                    // True if currently picked (cooldown active)
    float m_respawnTimer;                // Time remaining until respawn (seconds)
    float m_maxRespawnTime;              // Total respawn duration (seconds)
    float m_interactionRadius;           // Radius of player interaction circle
    const Alchemy::PlantData* m_data;    // Cached pointer to alchemical details
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_PLANT_NODE_H
