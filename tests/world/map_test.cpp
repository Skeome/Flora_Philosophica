#include "doctest/doctest.h"
#include "world/map.h"

TEST_CASE("TileMap collision and sliding resolution") {
    using namespace FloraPhilosophica::World;

    // Create a 20x15 tile map with 60px tileSize
    TileMap map(20, 15, 60);
    map.Initialize();

    // Spawn point (600, 450) is on path, no obstacles.
    Vector2 initialPos = { 600.0f, 450.0f };
    float playerRadius = 15.0f;

    SUBCASE("No collision when moving in open space") {
        Vector2 targetPos = { 610.0f, 440.0f };
        Vector2 resolved = map.ConstrainPosition(initialPos, targetPos, playerRadius);

        // Position should move freely
        CHECK(resolved.x == doctest::Approx(targetPos.x));
        CHECK(resolved.y == doctest::Approx(targetPos.y));
    }

    SUBCASE("Collision with map boundary is clamped") {
        Vector2 targetPos = { -10.0f, 450.0f }; // Way outside left edge
        Vector2 resolved = map.ConstrainPosition(initialPos, targetPos, playerRadius);

        // X coordinate should be clamped to playerRadius (15.0)
        CHECK(resolved.x == doctest::Approx(playerRadius));
        CHECK(resolved.y == doctest::Approx(targetPos.y));
    }

    SUBCASE("Collision with stone pillar resolves with sliding") {
        // Oblique stone pillar is located at Rectangle { 15 * 60, 4 * 60, 1.5 * 60, 1.5 * 60 }
        // Rect = { 900, 240, 90, 90 }
        // Let's place player at (880, 285) - near the left edge of the obstacle
        Vector2 startPos = { 880.0f, 285.0f };
        
        // Player tries to walk diagonally-right-up: (910, 260) - inside the pillar.
        Vector2 targetPos = { 910.0f, 260.0f };
        Vector2 resolved = map.ConstrainPosition(startPos, targetPos, playerRadius);

        // Resolved position should be pushed out to the edge of the pillar.
        // Pillar left edge is at 900. Radius is 15. Push-out position is 900 - 15 = 885.
        CHECK(resolved.x == doctest::Approx(885.0f));
        // Y coordinate should remain as targetPos.y = 260 as it's not blocked at the edge.
        CHECK(resolved.y == doctest::Approx(targetPos.y));
    }
}
