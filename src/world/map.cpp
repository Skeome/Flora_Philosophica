#include "map.h"
#include "raymath.h"
#include <cmath>
#include <queue>
#include <unordered_map>
#include <algorithm>

namespace FloraPhilosophica {
namespace World {

TileMap::TileMap(int width, int height, int tileSize)
    : m_width(width)
    , m_height(height)
    , m_tileSize(tileSize)
{
    m_tiles.resize(width * height, TileType::Grass);
}

void TileMap::Initialize(MapType type) {
    // Clear any existing state (safe to call Initialize multiple times)
    m_tiles.assign(m_width * m_height, TileType::Grass);
    m_obstacles.clear();
    m_plants.clear();

    if (type == MapType::CabinInterior || type == MapType::Loft) {
        InitializeCabinInterior();
        return;
    }

    if (type == MapType::Garden) {
        InitializeGarden();
        return;
    }

    // Default: Exterior map
    InitializeExterior();
}

void TileMap::InitializeExterior() {
    // 1. Draw cobblestone paths
    int pathRow  = m_height / 2;
    int cabinCol = m_width / 2;

    // Horizontal path through the middle
    for (int x = 0; x < m_width; ++x) {
        m_tiles[pathRow * m_width + x] = TileType::Cobblestone;
        if (pathRow + 1 < m_height)
            m_tiles[(pathRow + 1) * m_width + x] = TileType::Cobblestone;
    }

    // Vertical branch up to the cabin door
    for (int y = 2; y <= pathRow; ++y) {
        m_tiles[y * m_width + cabinCol]     = TileType::Cobblestone;
        m_tiles[y * m_width + cabinCol + 1] = TileType::Cobblestone;
    }

    // 2. Define physical static obstacles
    // Obstacle 1a: Cabin left wall (leaves a 2-tile door gap at columns 10-11)
    // cabinCol=10, so door is at x:600-720. Left wall runs from x:480 to x:600.
    m_obstacles.push_back({
        Rectangle{
            static_cast<float>((cabinCol - 2) * m_tileSize),   // x: 480
            static_cast<float>(0.5f * m_tileSize),              // y: 30
            static_cast<float>(2 * m_tileSize),                 // width: 120 (cols 8-9)
            static_cast<float>(2.5f * m_tileSize)               // height: 150
        },
        Color{ 140, 100, 70, 255 }
    });

    // Obstacle 1b: Cabin right wall (mirror of left wall, starts after the door gap)
    // Door gap ends at x:720 (col 12), right wall runs from x:720 to x:840.
    m_obstacles.push_back({
        Rectangle{
            static_cast<float>((cabinCol + 2) * m_tileSize),   // x: 720
            static_cast<float>(0.5f * m_tileSize),              // y: 30
            static_cast<float>(2 * m_tileSize),                 // width: 120 (cols 12-13)
            static_cast<float>(2.5f * m_tileSize)               // height: 150
        },
        Color{ 140, 100, 70, 255 }
    });

    // Obstacle 1c: Cabin back wall (top, full width, above the door gap)
    // Thin back wall so the cabin reads as a complete structure.
    m_obstacles.push_back({
        Rectangle{
            static_cast<float>((cabinCol - 2) * m_tileSize),   // x: 480
            static_cast<float>(0.5f * m_tileSize),              // y: 30
            static_cast<float>(6 * m_tileSize),                 // full width: 360
            static_cast<float>(0.6f * m_tileSize)               // thin top bar: 36px
        },
        Color{ 120, 85, 55, 255 } // slightly darker for roof line
    });

    // Obstacle 2: A large fallen log (horizontal wall)
    m_obstacles.push_back({
        Rectangle{
            static_cast<float>(3 * m_tileSize),
            static_cast<float>(10 * m_tileSize),
            static_cast<float>(4 * m_tileSize),
            static_cast<float>(1.2f * m_tileSize)
        },
        Color{ 90, 70, 50, 255 } // Dark wood
    });

    // Obstacle 3: A standing mossy stone pillar
    m_obstacles.push_back({
        Rectangle{
            static_cast<float>(15 * m_tileSize),
            static_cast<float>(4 * m_tileSize),
            static_cast<float>(1.5f * m_tileSize),
            static_cast<float>(1.5f * m_tileSize)
        },
        Color{ 110, 120, 100, 255 } // Moss green/gray
    });

    // 3. Populate harvestable plant nodes
    // Note: Nettle moved to 300, 750 to avoid overlapping the fallen log obstacle
    m_plants.push_back(std::make_unique<PlantNode>("St. John's Wort", Vector2{ 200, 200 }));
    m_plants.push_back(std::make_unique<PlantNode>("Mugwort", Vector2{ 1000, 150 }));
    m_plants.push_back(std::make_unique<PlantNode>("Nettle", Vector2{ 300, 750 }));
    m_plants.push_back(std::make_unique<PlantNode>("Lavender", Vector2{ 700, 450 }));
    m_plants.push_back(std::make_unique<PlantNode>("Comfrey", Vector2{ 1000, 600 }));
}

// ─────────────────────────────────────────────────────────────────────────────
// InitializeCabinInterior
// Fills the map with warm wood-plank floor tiles.
// No plant nodes — plants don't grow indoors.
// No structural obstacles — placed items (PlacedItem) act as the furniture
// and are managed by RoomManager, not TileMap.
// ─────────────────────────────────────────────────────────────────────────────
void TileMap::InitializeCabinInterior() {
    // Fill every tile with WoodFloor
    // Alternate between two tones for a plank pattern
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            m_tiles[y * m_width + x] = TileType::WoodFloor;
        }
    }
    // No obstacles or plant nodes — the cabin interior is open space
    // Placed items (fireplace, workbench, etc.) are drawn by RoomManager
}

// ─────────────────────────────────────────────────────────────────────────────
// InitializeGarden
// Outdoor cultivated plot — grass with a few soil patches.
// Placeholder until the garden system is fully designed.
// ─────────────────────────────────────────────────────────────────────────────
void TileMap::InitializeGarden() {
    // Default to grass — soil tile patches will be added when
    // the cultivation system is implemented in a future step
    m_tiles.assign(m_width * m_height, TileType::Grass);
}

void TileMap::Update(float deltaTime) {
    for (auto& plant : m_plants) {
        plant->Update(deltaTime);
    }
}

void TileMap::Draw() const {
    // 1. Draw the basic grass and paths
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            Rectangle tileRect = {
                static_cast<float>(x * m_tileSize),
                static_cast<float>(y * m_tileSize),
                static_cast<float>(m_tileSize),
                static_cast<float>(m_tileSize)
            };

            TileType type = m_tiles[y * m_width + x];
            Color tileColor = GREEN;

            if (type == TileType::Grass) {
                // Checkerboard pattern for visual texture
                tileColor = ((x + y) % 2 == 0)
                    ? Color{ 40, 130,  60, 255 }
                    : Color{ 35, 120,  55, 255 };
            } else if (type == TileType::Cobblestone) {
                tileColor = Color{ 160, 160, 170, 255 };
            } else if (type == TileType::WoodFloor) {
                // Alternating plank rows — every 2 rows shifts to a slightly
                // darker tone to suggest horizontal floor boards
                tileColor = ((y / 2) % 2 == 0)
                    ? Color{ 160, 110,  65, 255 }  // lighter plank
                    : Color{ 140,  95,  55, 255 };  // darker plank
            }

            DrawRectangleRec(tileRect, tileColor);

            // Draw plank divider lines on wood floor tiles
            if (type == TileType::WoodFloor) {
                // Horizontal line every 2 tile rows to suggest board edges
                if (y % 2 == 0) {
                    DrawLine(
                        static_cast<int>(tileRect.x),
                        static_cast<int>(tileRect.y),
                        static_cast<int>(tileRect.x + tileRect.width),
                        static_cast<int>(tileRect.y),
                        Color{ 100, 65, 35, 80 }
                    );
                }
            }

            if (type == TileType::Cobblestone) {
                DrawRectangleLines(
                    static_cast<int>(tileRect.x),
                    static_cast<int>(tileRect.y),
                    m_tileSize,
                    m_tileSize,
                    Color{ 130, 130, 140, 100 }
                );
            }
        }
    }

    // 2. Draw static physical obstacles
    for (const auto& obstacle : m_obstacles) {
        DrawRectangleRec(
            Rectangle{ obstacle.rect.x + 4, obstacle.rect.y + 4, obstacle.rect.width, obstacle.rect.height },
            Fade(BLACK, 0.3f)
        );
        DrawRectangleRec(obstacle.rect, obstacle.color);
        DrawRectangleLinesEx(obstacle.rect, 2.0f, ColorAlpha(WHITE, 0.2f));

        // Draw cabin label on the wide back-wall obstacle only
        if (obstacle.rect.width >= static_cast<float>(6 * m_tileSize) 
            && obstacle.rect.height < static_cast<float>(m_tileSize)) {
            DrawText("Cabin Sanctuary (Under Construction)",
                     static_cast<int>(obstacle.rect.x + 15),
                     static_cast<int>(obstacle.rect.y + 8),
                     16, RAYWHITE);
        }
    }

    // 3. Draw plant nodes
    for (const auto& plant : m_plants) {
        plant->Draw();
    }
}

Vector2 TileMap::ConstrainPosition(Vector2 oldPosition, Vector2 newPosition, float playerRadius) const {
    Vector2 resolved = newPosition;

    // 1. Map Boundary Constraints
    float minX = playerRadius;
    float maxX = static_cast<float>(m_width * m_tileSize) - playerRadius;
    float minY = playerRadius;
    float maxY = static_cast<float>(m_height * m_tileSize) - playerRadius;

    if (resolved.x < minX) resolved.x = minX;
    if (resolved.x > maxX) resolved.x = maxX;
    if (resolved.y < minY) resolved.y = minY;
    if (resolved.y > maxY) resolved.y = maxY;

    // 2. Obstacle Collision Resolution (multiple passes for corner cases)
    for (int iter = 0; iter < 4; ++iter) {
        bool collisionFound = false;

        for (const auto& obstacle : m_obstacles) {
            float closestX = std::max(obstacle.rect.x, std::min(resolved.x, obstacle.rect.x + obstacle.rect.width));
            float closestY = std::max(obstacle.rect.y, std::min(resolved.y, obstacle.rect.y + obstacle.rect.height));

            float dx = resolved.x - closestX;
            float dy = resolved.y - closestY;
            float distanceSquared = dx * dx + dy * dy;

            if (distanceSquared < playerRadius * playerRadius && distanceSquared > 0.0001f) {
                float distance = std::sqrt(distanceSquared);
                float overlap = playerRadius - distance;
                Vector2 pushOut = { (dx / distance) * overlap, (dy / distance) * overlap };
                resolved.x += pushOut.x;
                resolved.y += pushOut.y;
                collisionFound = true;
            }
            else if (distanceSquared <= 0.0001f) {
                resolved.x = (resolved.x > obstacle.rect.x + obstacle.rect.width / 2) ?
                             obstacle.rect.x + obstacle.rect.width + playerRadius :
                             obstacle.rect.x - playerRadius;
                collisionFound = true;
            }
        }

        if (!collisionFound) break;
    }

    return resolved;
}

PlantNode* TileMap::CheckPlantInteraction(Vector2 playerPosition, float interactionRadius) {
    for (auto& plant : m_plants) {
        if (plant->CheckCollision(playerPosition, interactionRadius)) {
            return plant.get();
        }
    }
    return nullptr;
}

bool TileMap::IsWalkable(int tileX, int tileY) const {
    if (tileX < 0 || tileX >= m_width || tileY < 0 || tileY >= m_height) return false;

    TileType type = m_tiles[tileY * m_width + tileX];
    if (type == TileType::Water || type == TileType::ObstacleWall) return false;

    const float playerRadius = 15.0f;
    Vector2 tileCenter = {
        tileX * m_tileSize + m_tileSize / 2.0f,
        tileY * m_tileSize + m_tileSize / 2.0f
    };

    for (const auto& obstacle : m_obstacles) {
        if (CheckCollisionCircleRec(tileCenter, playerRadius, obstacle.rect)) return false;
    }

    return true;
}

bool TileMap::HasLineOfSight(Vector2 start, Vector2 end) const {
    Vector2 dir = Vector2Subtract(end, start);
    float distance = Vector2Length(dir);
    if (distance < 1.0f) return true;

    Vector2 normalizedDir = Vector2Scale(dir, 1.0f / distance);
    const float playerRadius = 15.0f;
    float stepSize = 10.0f;
    float currentDist = 0.0f;

    while (currentDist < distance) {
        Vector2 checkPoint = Vector2Add(start, Vector2Scale(normalizedDir, std::min(currentDist, distance)));

        if (checkPoint.x < playerRadius || checkPoint.x > GetWidth() - playerRadius ||
            checkPoint.y < playerRadius || checkPoint.y > GetHeight() - playerRadius) return false;

        for (const auto& obstacle : m_obstacles) {
            if (CheckCollisionCircleRec(checkPoint, playerRadius, obstacle.rect)) return false;
        }

        if (currentDist >= distance) break;
        currentDist += stepSize;
    }

    return true;
}

struct AStarNode {
    int x, y;
    float gCost;
    float hCost;
    AStarNode* parent;

    float fCost() const { return gCost + hCost; }

    bool operator>(const AStarNode& other) const {
        if (std::abs(fCost() - other.fCost()) < 0.001f) {
            return gCost < other.gCost;
        }
        return fCost() > other.fCost();
    }
};

std::vector<Vector2> TileMap::FindPath(Vector2 startWorld, Vector2 endWorld) const {
    // Fast path: direct line of sight
    if (HasLineOfSight(startWorld, endWorld)) {
        return { endWorld };
    }

    int startX = static_cast<int>(startWorld.x / m_tileSize);
    int startY = static_cast<int>(startWorld.y / m_tileSize);
    int endX = static_cast<int>(endWorld.x / m_tileSize);
    int endY = static_cast<int>(endWorld.y / m_tileSize);

    endX = std::max(0, std::min(endX, m_width - 1));
    endY = std::max(0, std::min(endY, m_height - 1));

    if (startX == endX && startY == endY) return { endWorld };

    // If end tile is blocked, find nearest walkable tile
    if (!IsWalkable(endX, endY)) {
        bool found = false;
        for (int r = 1; r < 5 && !found; ++r) {
            for (int dy = -r; dy <= r && !found; ++dy) {
                for (int dx = -r; dx <= r && !found; ++dx) {
                    if (IsWalkable(endX + dx, endY + dy)) {
                        endX += dx;
                        endY += dy;
                        found = true;
                    }
                }
            }
        }
        if (!found) return {};
    }

    // Heuristic: Euclidean distance with small tie-breaker
    auto heuristic = [endX, endY](int x, int y) {
        float dx = static_cast<float>(x - endX);
        float dy = static_cast<float>(y - endY);
        return std::sqrt(dx * dx + dy * dy) * 1.001f;
    };

    std::priority_queue<AStarNode*, std::vector<AStarNode*>, std::greater<AStarNode*>> openSet;
    std::unordered_map<int, AStarNode*> allNodes;

    auto getNode = [&](int x, int y) {
        int key = y * m_width + x;
        if (allNodes.find(key) == allNodes.end()) {
            allNodes[key] = new AStarNode{ x, y, 1e9f, 0.0f, nullptr };
        }
        return allNodes[key];
    };

    AStarNode* startNode = getNode(startX, startY);
    startNode->gCost = 0.0f;
    startNode->hCost = heuristic(startX, startY);
    openSet.push(startNode);

    AStarNode* targetNode = nullptr;

    while (!openSet.empty()) {
        AStarNode* current = openSet.top();
        openSet.pop();

        if (current->x == endX && current->y == endY) {
            targetNode = current;
            break;
        }

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;

                int nx = current->x + dx;
                int ny = current->y + dy;

                if (IsWalkable(nx, ny)) {
                    float moveCost = (dx != 0 && dy != 0) ? 1.414f : 1.0f;
                    AStarNode* neighbor = getNode(nx, ny);
                    float newGCost = current->gCost + moveCost;

                    if (newGCost < neighbor->gCost) {
                        neighbor->gCost = newGCost;
                        neighbor->hCost = heuristic(nx, ny);
                        neighbor->parent = current;
                        openSet.push(neighbor);
                    }
                }
            }
        }
    }

    std::vector<Vector2> rawPath;
    if (targetNode) {
        AStarNode* curr = targetNode;
        while (curr) {
            rawPath.push_back({ static_cast<float>(curr->x * m_tileSize + m_tileSize / 2),
                                static_cast<float>(curr->y * m_tileSize + m_tileSize / 2) });
            curr = curr->parent;
        }
        std::reverse(rawPath.begin(), rawPath.end());
    }

    // Clean up heap-allocated A* nodes
    // TODO: Replace with std::unique_ptr once smart pointers are covered in C++ study
    for (auto pair : allNodes) delete pair.second;

    if (rawPath.empty()) return {};

    // Path smoothing via string pulling (line-of-sight optimisation)
    std::vector<Vector2> smoothedPath;
    smoothedPath.push_back(startWorld);

    Vector2 currentPoint = startWorld;
    size_t i = 1;
    while (i < rawPath.size()) {
        size_t bestNext = i;
        for (size_t j = i + 1; j < rawPath.size(); ++j) {
            if (HasLineOfSight(currentPoint, rawPath[j])) {
                bestNext = j;
            } else {
                break;
            }
        }

        smoothedPath.push_back(rawPath[bestNext]);
        currentPoint = rawPath[bestNext];
        i = bestNext + 1;
    }

    if (!smoothedPath.empty()) smoothedPath.back() = endWorld;

    return smoothedPath;
}

} // namespace World
} // namespace FloraPhilosophica
