#include "raylib.h"
#include "raymath.h"
#include "world/player.h"
#include "world/map.h"
#include "core/clock.h"
#include <chrono>
#include <string>

int main() {
    // 1. Initialise the Raylib Window
    const int screenWidth = 1200;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Flora Philosophia - Apothecary Sanctuary");

    SetTargetFPS(60);

    // 2. Instantiate Game Systems
    using namespace FloraPhilosophia::World;
    using namespace FloraPhilosophia::Core;

    // Create a 20x15 tile map with 60px size (1200x900 world space)
    TileMap gameMap(20, 15, 60);
    gameMap.Initialize();

    // Spawn player at map center
    Player player({ 600.0f, 450.0f });

    // Astrological engine setup
    AstrologicalClock clock;
    // TODO: Replace with device GPS coordinates at runtime
    // Test coordinates: Medford, Oregon (approx. 42.3265 N, -122.8756 W)
    const double observerLat = 42.3265;
    const double observerLon = -122.8756;

    // 3. Configure Camera2D (for smooth scrolling behavior)
    Camera2D camera = { 0 };
    camera.target = player.GetPosition();
    camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // 4. Input state management
    bool joystickActive = false;
    Vector2 joystickAnchor = { 0.0f, 0.0f };
    Vector2 joystickDirection = { 0.0f, 0.0f };
    const float joystickMaxRadius = 60.0f;

    float clickHoldTimer = 0.0f;
    const float joystickHoldThreshold = 0.2f; // Seconds of hold before joystick activates
    bool isHolding = false;

    // Planetary glyph lookup — matches Planet enum order (Saturn=0 ... Moon=6)
    // Used for the astrological clock HUD display
    const char* PLANET_GLYPHS[7] = { "♄", "♃", "♂", "☉", "♀", "☿", "☽" };

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // --- Core Input Handling ---
        Vector2 mousePos = GetMousePosition();
        bool isLeftClickDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool leftClickPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool leftClickReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        // Reset joystick direction each frame
        joystickDirection = { 0.0f, 0.0f };

        if (leftClickPressed) {
            isHolding = true;
            clickHoldTimer = 0.0f;
            joystickAnchor = mousePos;
        }

        if (isHolding) {
            clickHoldTimer += deltaTime;

            if (clickHoldTimer >= joystickHoldThreshold && !joystickActive) {
                // Long press threshold met: activate floating joystick
                joystickActive = true;
            }

            if (leftClickReleased) {
                if (!joystickActive) {
                    // Short tap: trigger A* pathfinding to tapped world position
                    Vector2 worldClickPos = GetScreenToWorld2D(mousePos, camera);
                    auto path = gameMap.FindPath(player.GetPosition(), worldClickPos);
                    player.SetPath(path);
                }

                // Reset hold state
                isHolding = false;
                joystickActive = false;
                clickHoldTimer = 0.0f;
            }
        }

        if (joystickActive && isLeftClickDown) {
            // Compute normalised joystick direction from anchor drag
            Vector2 offset = Vector2Subtract(mousePos, joystickAnchor);
            float distance = Vector2Length(offset);

            if (distance > 0.0f) {
                Vector2 normalizedDir = Vector2Scale(offset, 1.0f / distance);
                if (distance > joystickMaxRadius) {
                    offset = Vector2Scale(normalizedDir, joystickMaxRadius);
                }
                joystickDirection = normalizedDir;
            }
        }

        // --- Core Updates ---
        Vector2 oldPos = player.GetPosition();
        player.Update(deltaTime, joystickActive, joystickDirection);

        // Constrain player position against walls and map edges
        Vector2 proposedPos = player.GetPosition();
        Vector2 resolvedPos = gameMap.ConstrainPosition(oldPos, proposedPos, 15.0f);
        player.SetPosition(resolvedPos);

        // Update map (plant respawn timers)
        gameMap.Update(deltaTime);

        // Smooth camera follow
        camera.target = player.GetPosition();

        // --- Calculate Astrological State ---
        // Fetch current system Unix UTC timestamp
        long long currentUnixTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        PlanetaryHourInfo clockInfo = clock.CalculatePlanetaryHour(observerLat, observerLon, currentUnixTime);

        // --- Interaction Logic ---
        static std::string lastHarvestLog = "Walk near a plant and press [E] to harvest";
        static float logTimer = 0.0f;
        if (logTimer > 0.0f) logTimer -= deltaTime;
        else lastHarvestLog = "Walk near a plant and press [E] to harvest";

        if (IsKeyPressed(KEY_E)) {
            PlantNode* node = gameMap.CheckPlantInteraction(player.GetPosition(), 20.0f);
            if (node) {
                // Use the real day ruler and hour ruler from the clock engine.
                // Pristine quality requires BOTH to match the plant's ruling planet.
                HarvestQuality quality = node->Harvest(clockInfo.dayRuler, clockInfo.rulingPlanet);
                lastHarvestLog = "Harvested " + node->GetName() + ": " + PlantNode::GetQualityName(quality);
                logTimer = 3.0f;
            }
        }

        // --- Rendering ---
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode2D(camera);
                gameMap.Draw();

                // Draw A* path debug visualisation
                if (player.HasActivePath()) {
                    const auto& path = player.GetPath();
                    for (size_t i = player.GetCurrentPathIndex(); i < path.size(); ++i) {
                        DrawCircleV(path[i], 3, Fade(GOLD, 0.5f));
                        if (i > (size_t)player.GetCurrentPathIndex()) {
                            DrawLineV(path[i-1], path[i], Fade(GOLD, 0.3f));
                        } else {
                            DrawLineV(player.GetPosition(), path[i], Fade(GOLD, 0.3f));
                        }
                    }
                }

                player.Draw();
            EndMode2D();

            // --- UI Overlays ---

            // 1. Virtual Joystick
            if (joystickActive) {
                DrawCircleV(joystickAnchor, joystickMaxRadius, Fade(GRAY, 0.4f));
                DrawCircleLinesV(joystickAnchor, joystickMaxRadius, Fade(LIGHTGRAY, 0.6f));
                Vector2 knobPos = Vector2Add(joystickAnchor, Vector2Scale(joystickDirection, joystickMaxRadius * 0.6f));
                DrawCircleV(knobPos, 22, Fade(SKYBLUE, 0.8f));
                DrawCircleLinesV(knobPos, 22, BLUE);
            }

            // 2. Astrological Clock HUD (top-left)
            DrawRectangle(15, 15, 340, 115, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, 15, 340, 115, Color{ 140, 100, 70, 255 });

            std::string timeStr     = "Planetary Hour: " + clockInfo.planetName;
            std::string cycleStr    = clockInfo.hourIndex < 12
                                    ? "Day Hour (Segment "
                                    : "Night Hour (Segment ";
            cycleStr += std::to_string(clockInfo.hourIndex % 12 + 1) + "/12)";
            std::string remainingStr = "Time remaining: " + std::to_string(static_cast<int>(clockInfo.minutesRemaining)) + "m";

            // Planetary glyph circle — uses real glyph for the ruling planet
            int planetEnumIndex = static_cast<int>(clockInfo.rulingPlanet);
            const char* glyph = PLANET_GLYPHS[planetEnumIndex];
            DrawCircle(40, 50, 18, Color{ 160, 100, 40, 255 });
            DrawText(glyph, 30, 42, 20, RAYWHITE);

            DrawText("ASTROLOGICAL CLOCK", 75, 25, 12, GOLD);
            DrawText(timeStr.c_str(), 75, 42, 16, RAYWHITE);
            DrawText(cycleStr.c_str(), 75, 63, 13, LIGHTGRAY);
            DrawText(remainingStr.c_str(), 75, 80, 13, LIGHTGRAY);
            DrawText("Medford: 42.3N, -122.8W", 75, 100, 11, GRAY);

            // 3. Controls Legend (bottom-left)
            DrawRectangle(15, screenHeight - 115, 400, 100, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, screenHeight - 115, 400, 100, Color{ 110, 110, 120, 255 });
            DrawText("CONTROLS:", 30, screenHeight - 105, 12, GOLD);
            DrawText("- Desktop: Use WASD / Arrow Keys to move", 30, screenHeight - 87, 14, RAYWHITE);
            DrawText("- Hold click/tap anywhere to activate Joystick", 30, screenHeight - 69, 14, RAYWHITE);
            DrawText("- Tap anywhere to pathfind", 30, screenHeight - 51, 14, RAYWHITE);

            // 4. Harvest Log (top-center)
            DrawRectangle(screenWidth / 2 - 200, 20, 400, 40, Fade(BLACK, 0.6f));
            DrawText(lastHarvestLog.c_str(),
                     screenWidth / 2 - MeasureText(lastHarvestLog.c_str(), 16) / 2,
                     32, 16, GOLD);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}