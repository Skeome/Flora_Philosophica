#include "plant_node.h"
#include "raymath.h"

namespace FloraPhilosophica {
namespace World {

PlantNode::PlantNode(std::string plantName, Vector2 position)
    : m_plantName(plantName)
    , m_position(position)
    , m_harvested(false)
    , m_respawnTimer(0.0f)
    , m_maxRespawnTime(300.0f) // 5 minutes for testing; will scale to real time later
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

    // TODO: Replace placeholder drawing with Aseprite texture rendering once assets are ready

    // Color encodes the plant's ruling planet for at-a-glance identification
    Color plantColor = GREEN;
    if (m_data) {
        switch (m_data->ruler) {
            case Core::Planet::Sun:     plantColor = GOLD;     break;
            case Core::Planet::Moon:    plantColor = WHITE;    break;
            case Core::Planet::Mars:    plantColor = RED;      break;
            case Core::Planet::Venus:   plantColor = PINK;     break;
            case Core::Planet::Mercury: plantColor = SKYBLUE;  break;
            case Core::Planet::Jupiter: plantColor = PURPLE;   break;
            case Core::Planet::Saturn:  plantColor = DARKGRAY; break;
        }
    }

    // Shadow
    DrawCircle(static_cast<int>(m_position.x),
               static_cast<int>(m_position.y) + 5, 10, Fade(BLACK, 0.3f));

    // Plant body
    DrawCircleV(m_position, 12.0f, plantColor);
    DrawCircleLines(static_cast<int>(m_position.x),
                    static_cast<int>(m_position.y), 12, DARKGREEN);

    // Label
    DrawText(m_plantName.c_str(),
             static_cast<int>(m_position.x) - 30,
             static_cast<int>(m_position.y) - 25,
             10, RAYWHITE);
}

bool PlantNode::CheckCollision(Vector2 playerPosition, float interactionRadius) const {
    if (m_harvested) return false;
    return CheckCollisionCircles(m_position, m_interactionRadius,
                                 playerPosition, interactionRadius);
}

HarvestQuality PlantNode::Harvest(Core::Planet dayRuler, Core::Planet hourRuler) {
    if (m_harvested) return HarvestQuality::Standard;

    m_harvested    = true;
    m_respawnTimer = m_maxRespawnTime;

    if (!m_data) return HarvestQuality::Standard;

    // Celestial: matching day AND hour
    if (dayRuler == m_data->ruler && hourRuler == m_data->ruler)
        return HarvestQuality::Celestial;

    // Pristine: matching hour only
    if (hourRuler == m_data->ruler)
        return HarvestQuality::Pristine;

    // Check for opposite planets
    std::vector<Core::Planet> opposites = GetOppositePlanets(static_cast<Core::Planet>(m_data->ruler));
    bool hourIsOpposite = false;
    bool dayIsOpposite = false;
    
    for (Core::Planet p : opposites) {
        if (hourRuler == p) hourIsOpposite = true;
        if (dayRuler == p) dayIsOpposite = true;
    }

    // Debased: BOTH day AND hour are opposite
    if (dayIsOpposite && hourIsOpposite)
        return HarvestQuality::Debased;

    // Stressed: hour is opposite only
    if (hourIsOpposite)
        return HarvestQuality::Stressed;

    return HarvestQuality::Standard;
}

std::vector<Core::Planet> PlantNode::GetOppositePlanets(Core::Planet planet) {
    // Classical astrological oppositions
    switch (planet) {
        case Core::Planet::Sun:     return {Core::Planet::Saturn};
        case Core::Planet::Moon:    return {Core::Planet::Saturn};
        case Core::Planet::Saturn:  return {Core::Planet::Sun, Core::Planet::Moon};
        case Core::Planet::Mars:    return {Core::Planet::Venus};
        case Core::Planet::Venus:   return {Core::Planet::Mars};
        case Core::Planet::Mercury: return {Core::Planet::Jupiter};
        case Core::Planet::Jupiter: return {Core::Planet::Mercury};
        default:                    return {};
    }
}

std::string PlantNode::GetQualityName(HarvestQuality quality) {
    switch (quality) {
        case HarvestQuality::Celestial: return "Celestial Harvest";
        case HarvestQuality::Pristine:  return "Pristine Harvest";
        case HarvestQuality::Standard:  return "Standard Quality";
        case HarvestQuality::Stressed:  return "Stressed / Negated";
        case HarvestQuality::Debased:   return "Debased / Inverted";
        default:                        return "Unknown";
    }
}

} // namespace World
} // namespace FloraPhilosophica
