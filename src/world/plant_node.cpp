#include "plant_node.h"
#include "raymath.h"
#include <iostream>

namespace FloraPhilosophia {
namespace World {

PlantNode::PlantNode(std::string plantName, Vector2 position)
    : m_plantName(plantName)
    , m_position(position)
    , m_harvested(false)
    , m_respawnTimer(0.0f)
    , m_maxRespawnTime(300.0f) // 5 minutes respawn for testing
    , m_interactionRadius(40.0f)
{
    m_data = Alchemy::PlantDatabase::GetPlant(plantName);
}

void PlantNode::Update(float deltaTime) {
    if (m_harvested) {
        m_respawnTimer -= deltaTime;
        if (m_respawnTimer <= 0.0f) {
            m_harvested = false;
            m_respawnTimer = 0.0f;
        }
    }
}

void PlantNode::Draw() const {
    if (m_harvested) return;

    // Draw the plant node
    // In a full implementation, this would use sprites.
    // For now, we use colored shapes based on the plant's ruler.
    
    Color plantColor = GREEN;
    if (m_data) {
        switch (m_data->ruler) {
            case Core::Planet::Sun:     plantColor = GOLD; break;
            case Core::Planet::Moon:    plantColor = WHITE; break;
            case Core::Planet::Mars:    plantColor = RED; break;
            case Core::Planet::Venus:   plantColor = PINK; break;
            case Core::Planet::Mercury: plantColor = SKYBLUE; break;
            case Core::Planet::Jupiter: plantColor = PURPLE; break;
            case Core::Planet::Saturn:  plantColor = DARKGRAY; break;
        }
    }

    // Shadow
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y) + 5, 10, Fade(BLACK, 0.3f));
    
    // Plant "Body"
    DrawCircleV(m_position, 12.0f, plantColor);
    DrawCircleLines(static_cast<int>(m_position.x), static_cast<int>(m_position.y), 12, DARKGREEN);
    
    // Label
    DrawText(m_plantName.c_str(), static_cast<int>(m_position.x) - 30, static_cast<int>(m_position.y) - 25, 10, RAYWHITE);
}

bool PlantNode::CheckCollision(Vector2 playerPosition, float interactionRadius) const {
    if (m_harvested) return false;
    return CheckCollisionCircles(m_position, m_interactionRadius, playerPosition, interactionRadius);
}

HarvestQuality PlantNode::Harvest(Core::Planet dayRuler, Core::Planet hourRuler) {
    if (m_harvested) return HarvestQuality::Standard;

    m_harvested = true;
    m_respawnTimer = m_maxRespawnTime;

    if (!m_data) return HarvestQuality::Standard;

    // 1. Pristine: Matching Day AND Hour
    if (dayRuler == m_data->ruler && hourRuler == m_data->ruler) {
        return HarvestQuality::Pristine;
    }

    // 2. Debased: Opposite Planetary Hour
    if (hourRuler == GetOppositePlanet(m_data->ruler)) {
        return HarvestQuality::Debased;
    }

    // 3. Standard: Everything else
    return HarvestQuality::Standard;
}

Core::Planet PlantNode::GetOppositePlanet(Core::Planet planet) {
    // Classical astrological oppositions:
    // Sun opposes Saturn, Moon opposes Mars,
    // Mercury opposes Jupiter, Venus opposes Mars (secondary)
    switch (planet) {
        case Core::Planet::Sun:     return Core::Planet::Saturn;
        case Core::Planet::Saturn:  return Core::Planet::Sun;
        case Core::Planet::Moon:    return Core::Planet::Mars;
        case Core::Planet::Mars:    return Core::Planet::Moon;
        case Core::Planet::Mercury: return Core::Planet::Jupiter;
        case Core::Planet::Jupiter: return Core::Planet::Mercury;
        case Core::Planet::Venus:   return Core::Planet::Mars;
        default:                    return Core::Planet::Saturn;
    }
}

std::string PlantNode::GetQualityName(HarvestQuality quality) {
    switch (quality) {
        case HarvestQuality::Pristine: return "Pristine Celestial Harvest";
        case HarvestQuality::Standard: return "Standard Quality";
        case HarvestQuality::Debased:  return "Stressed / Debased specimen";
        default:                       return "Unknown";
    }
}

} // namespace World
} // namespace FloraPhilosophia