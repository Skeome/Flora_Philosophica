#ifndef FLORA_PHILOSOPHICA_WORLD_PLANT_NODE_H
#define FLORA_PHILOSOPHICA_WORLD_PLANT_NODE_H

#include "raylib.h"
#include "core/clock.h"
#include "alchemy/plant_db.h"
#include "item.h"
#include <string>
#include <vector>

namespace FloraPhilosophica {
namespace World {

class PlantNode {
public:
    PlantNode(std::string plantName, Vector2 position);

    void Update(float deltaTime);
    void Draw() const;

    bool CheckCollision(Vector2 playerPosition, float interactionRadius) const;

    // Trigger harvest — returns quality based on active astrological state.
    // HarvestQuality is now defined in item.h.
    HarvestQuality Harvest(Core::Planet dayRuler, Core::Planet hourRuler);

    // Classical astrological opposite/enemy planets
    static std::vector<Core::Planet> GetOppositePlanets(Core::Planet planet);

    // Quality display name
    static std::string GetQualityName(HarvestQuality quality);

    // Getters
    std::string GetName()  const { return m_plantName; }
    Vector2     GetPosition() const { return m_position; }
    bool        IsActive() const { return !m_harvested; }
    float       GetRespawnTimeRemaining() const { return m_respawnTimer; }

private:
    std::string m_plantName;
    Vector2     m_position;
    bool        m_harvested;
    float       m_respawnTimer;
    float       m_maxRespawnTime;
    float       m_interactionRadius;
    const Alchemy::PlantData* m_data;
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_PLANT_NODE_H
