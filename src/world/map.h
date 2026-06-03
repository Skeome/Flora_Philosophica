#ifndef FLORA_PHILOSOPHIA_WORLD_MAP_H
#define FLORA_PHILOSOPHIA_WORLD_MAP_H

#include "raylib.h"
#include "plant_node.h"
#include <vector>
#include <memory>

namespace FloraPhilosophia {
namespace World {

enum class TileType {
    Grass,
    Cobblestone,
    Water,
    ObstacleWall
};

struct MapObstacle {
    Rectangle rect;
    Color color;
};

class TileMap {
public:
    TileMap(int width, int height, int tileSize);

    // Initialise map tiles and place layout elements
    void Initialize();

    // Update the map state (including plant respawns)
    void Update(float deltaTime);

    // Render map tiles, obstacles, and plant nodes
    void Draw() const;

    // Checks if a proposed player movement collides with boundaries or obstacles.
    // If a collision occurs, resolves the position.
    Vector2 ConstrainPosition(Vector2 oldPosition, Vector2 newPosition, float playerRadius) const;

    // Interaction check for the player
    PlantNode* CheckPlantInteraction(Vector2 playerPosition, float interactionRadius);

    // Pathfinding support
    bool IsWalkable(int tileX, int tileY) const;
    bool HasLineOfSight(Vector2 start, Vector2 end) const;
    std::vector<Vector2> FindPath(Vector2 startWorld, Vector2 endWorld) const;

    int GetWidth() const { return m_width * m_tileSize; }
    int GetHeight() const { return m_height * m_tileSize; }
    int GetTileSize() const { return m_tileSize; }

private:
    int m_width;                         // Number of tiles horizontally
    int m_height;                        // Number of tiles vertically
    int m_tileSize;                      // Size of each tile in pixels
    std::vector<TileType> m_tiles;       // Flattened grid array representing layout
    std::vector<MapObstacle> m_obstacles;// Static physical obstacles (walls, logs, trees)
    std::vector<std::unique_ptr<PlantNode>> m_plants; // Harvestable plant nodes
};

} // namespace World
} // namespace FloraPhilosophia

#endif // FLORA_PHILOSOPHIA_WORLD_MAP_H
