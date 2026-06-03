#include "player.h"
#include "raymath.h"

namespace FloraPhilosophia {
namespace World {

Player::Player(Vector2 startPosition)
    : m_position(startPosition)
    , m_currentPathIndex(0)
    , m_hasDestination(false)
    , m_speed(200.0f) // Movement speed: 200 pixels per second
{}

void Player::Update(float deltaTime, bool joystickActive, Vector2 joystickDirection) {
    Vector2 keyboardDir = { 0.0f, 0.0f };

    // 1. Poll keyboard input for WASD/Arrow keys (crucial for Desktop development and testing)
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    keyboardDir.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  keyboardDir.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  keyboardDir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) keyboardDir.x += 1.0f;

    // 2. Resolve inputs (Keyboard takes priority, then virtual joystick, then tap-to-move)
    if (keyboardDir.x != 0.0f || keyboardDir.y != 0.0f) {
        // Keyboard movement active: normalize direction to prevent faster diagonal movement
        keyboardDir = Vector2Normalize(keyboardDir);
        Move(keyboardDir, deltaTime);
        m_hasDestination = false; // Intercept and cancel any active tap-to-move path
    }
    else if (joystickActive && (joystickDirection.x != 0.0f || joystickDirection.y != 0.0f)) {
        // Virtual joystick movement active
        Move(joystickDirection, deltaTime);
        m_hasDestination = false; // Intercept and cancel any active tap-to-move path
    }
    else if (m_hasDestination && m_currentPathIndex < m_path.size()) {
        // Path following logic
        Vector2 target = m_path[m_currentPathIndex];
        Vector2 toTarget = Vector2Subtract(target, m_position);
        float distance = Vector2Length(toTarget);

        if (distance <= 2.5f) {
            // Target waypoint reached
            m_currentPathIndex++;
            if (m_currentPathIndex >= m_path.size()) {
                m_hasDestination = false;
            }
        } else {
            // Move towards current waypoint
            Vector2 direction = Vector2Normalize(toTarget);
            Move(direction, deltaTime);
        }
    }
}

void Player::Move(Vector2 direction, float deltaTime) {
    // Basic displacement vector: direction * speed * time delta
    Vector2 displacement = Vector2Scale(direction, m_speed * deltaTime);
    m_position = Vector2Add(m_position, displacement);
}

void Player::SetPath(const std::vector<Vector2>& path) {
    if (path.empty()) return;
    m_path = path;
    m_currentPathIndex = 0;
    m_hasDestination = true;
}

void Player::Draw() const {
    // For Step 2, draw the player as a simple, high-visibility circular avatar.
    // In later tiers, this will render the layered sprites.
    
    // Shadow circle
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y) + 12, 16, Fade(DARKGRAY, 0.4f));

    // Player body
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y), 15, SKYBLUE);
    DrawCircleLines(static_cast<int>(m_position.x), static_cast<int>(m_position.y), 15, BLUE);

    // Inner detail (facing direction indicator or styling)
    DrawCircle(static_cast<int>(m_position.x), static_cast<int>(m_position.y) - 3, 5, RAYWHITE);
}

} // namespace World
} // namespace FloraPhilosophia
