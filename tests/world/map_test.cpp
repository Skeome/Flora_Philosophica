#include "doctest/doctest.h"
#include "world/map.h"

TEST_CASE("TileMap collision and sliding resolution") {
    using namespace FloraPhilosophica::World;

    // Create a 20x15 tile map with 64px tileSize
    TileMap map(20, 15, 64);
    map.Initialize();

    // Spawn point (640, 480) is on path, no obstacles.
    Vector2 initialPos = { 640.0f, 480.0f };
    float playerRadius = 15.0f;

    SUBCASE("No collision when moving in open space") {
        Vector2 targetPos = { 650.0f, 470.0f };
        Vector2 resolved = map.ConstrainPosition(initialPos, targetPos, playerRadius);

        // Position should move freely
        CHECK(resolved.x == doctest::Approx(targetPos.x));
        CHECK(resolved.y == doctest::Approx(targetPos.y));
    }

    SUBCASE("Collision with map boundary is clamped") {
        Vector2 targetPos = { -10.0f, 480.0f }; // Way outside left edge
        Vector2 resolved = map.ConstrainPosition(initialPos, targetPos, playerRadius);

        // X coordinate should be clamped to playerRadius (15.0)
        CHECK(resolved.x == doctest::Approx(playerRadius));
        CHECK(resolved.y == doctest::Approx(targetPos.y));
    }

    SUBCASE("Collision with stone pillar resolves with sliding") {
        // Oblique stone pillar is located at Rectangle { 15 * 64, 4 * 64, 1.5 * 64, 1.5 * 64 }
        // Rect = { 960, 256, 96, 96 }
        // Let's place player at (940, 304) - near the left edge of the obstacle
        Vector2 startPos = { 940.0f, 304.0f };
        
        // Player tries to walk diagonally-right-up: (970, 277) - inside the pillar.
        Vector2 targetPos = { 970.0f, 277.0f };
        Vector2 resolved = map.ConstrainPosition(startPos, targetPos, playerRadius);

        // Resolved position should be pushed out to the edge of the pillar.
        // Pillar left edge is at 960. Radius is 15. Push-out position is 960 - 15 = 945.
        CHECK(resolved.x == doctest::Approx(945.0f));
        // Y coordinate should remain as targetPos.y = 277 as it's not blocked at the edge.
        CHECK(resolved.y == doctest::Approx(targetPos.y));
    }
}
