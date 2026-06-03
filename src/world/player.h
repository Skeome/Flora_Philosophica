#ifndef FLORA_PHILOSOPHIA_WORLD_PLAYER_H
#define FLORA_PHILOSOPHIA_WORLD_PLAYER_H

#include "raylib.h"
#include <vector>

namespace FloraPhilosophia {
namespace World {

class Player {
public:
    Player(Vector2 startPosition);

    // Update player position based on keyboard, mouse/touch clicks (tap-to-move), 
    // and the virtual joystick state.
    void Update(float deltaTime, bool joystickActive, Vector2 joystickDirection);

    // Render the player character in the game world.
    void Draw() const;

    // Direct movement interface
    void Move(Vector2 direction, float deltaTime);

    // Set a pathfinding/movement destination (tap-to-move)
    void SetPath(const std::vector<Vector2>& path);

    // Getter for player position
    Vector2 GetPosition() const { return m_position; }

    // Setter for player position (used for teleportation and collision resolution)
    void SetPosition(Vector2 pos) { m_position = pos; }

    bool HasActivePath() const { return m_hasDestination; }
    const std::vector<Vector2>& GetPath() const { return m_path; }
    int GetCurrentPathIndex() const { return m_currentPathIndex; }

private:
    Vector2 m_position;          // Active 2D position in the game world
    std::vector<Vector2> m_path; // Sequence of points to follow
    int m_currentPathIndex;      // Index of current target waypoint
    bool m_hasDestination;       // True if moving towards a tapped destination
    float m_speed;               // Movement speed in units/sec
};

} // namespace World
} // namespace FloraPhilosophia

#endif // FLORA_PHILOSOPHIA_WORLD_PLAYER_H
