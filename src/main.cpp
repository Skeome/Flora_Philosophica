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
    // Base test coordinates: Portland, Oregon (approx. 45.5152 N, -122.6784 W)
    const double observerLat = 45.5152;
    const double observerLon = -122.6784;

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
    const float joystickHoldThreshold = 0.2f; // Seconds to hold before joystick activates
    bool isHolding = false;

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // --- Core Input Handling ---
        Vector2 mousePos = GetMousePosition();
        bool isLeftClickDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool leftClickPressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool leftClickReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        // Reset joystick direction
        joystickDirection = { 0.0f, 0.0f };

        if (leftClickPressed) {
            isHolding = true;
            clickHoldTimer = 0.0f;
            joystickAnchor = mousePos;
        }

        if (isHolding) {
            clickHoldTimer += deltaTime;
            
            if (clickHoldTimer >= joystickHoldThreshold && !joystickActive) {
                // Long press threshold met, activate joystick
                joystickActive = true;
            }
            
            if (leftClickReleased) {
                if (!joystickActive) {
                    // Tap detected (released before threshold)
                    Vector2 worldClickPos = GetScreenToWorld2D(mousePos, camera);
                    auto path = gameMap.FindPath(player.GetPosition(), worldClickPos);
                    player.SetPath(path);
                }
                
                // Cleanup
                isHolding = false;
                joystickActive = false;
                clickHoldTimer = 0.0f;
            }
        }

        if (joystickActive) {
            if (isLeftClickDown) {
                // Dragging the joystick
                Vector2 offset = Vector2Subtract(mousePos, joystickAnchor);
                float distance = Vector2Length(offset);

                if (distance > 0.0f) {
                    // Compute normalized direction
                    Vector2 normalizedDir = Vector2Scale(offset, 1.0f / distance);
                    
                    // Clamp joystick knob visual inside boundaries
                    if (distance > joystickMaxRadius) {
                        offset = Vector2Scale(normalizedDir, joystickMaxRadius);
                    }
                    
                    // Apply direction to movement vector
                    joystickDirection = normalizedDir;
                }
            }
        }

        // --- Core Updates ---
        // Save current position for slide collision resolution
        Vector2 oldPos = player.GetPosition();

        // Update player movements
        player.Update(deltaTime, joystickActive, joystickDirection);

        // Constrain player position to collide with walls and map edges
        Vector2 proposedPos = player.GetPosition();
        Vector2 resolvedPos = gameMap.ConstrainPosition(oldPos, proposedPos, 15.0f);
        player.SetPosition(resolvedPos);

        // Update map (plant respawns)
        gameMap.Update(deltaTime);

        // Update camera position to follow the player smoothly
        camera.target = player.GetPosition();

        // --- Calculate Astrological State ---
        // Fetch current system Unix UTC timestamp
        long long currentUnixTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        // Update planetary hour display data
        PlanetaryHourInfo clockInfo = clock.CalculatePlanetaryHour(observerLat, observerLon, currentUnixTime);

        // --- Interaction Logic ---
        static std::string lastHarvestLog = "Walk near a plant and press [E] to harvest";
        static float logTimer = 0.0f;
        if (logTimer > 0.0f) logTimer -= deltaTime;
        else lastHarvestLog = "Walk near a plant and press [E] to harvest";

        if (IsKeyPressed(KEY_E)) {
            PlantNode* node = gameMap.CheckPlantInteraction(player.GetPosition(), 20.0f);
            if (node) {
                // Calculate matching day ruler (simplification for prototype: weekday)
                // Real implementation would calculate sunrise ruler.
                // For now, let's just use the current clockInfo.rulingPlanet as both for testing matching state.
                // To get actual day ruler, we need more from AstrologicalClock.
                // Let's assume day ruler is currently Sun for testing.
                Planet dayRuler = Planet::Sun; 
                
                HarvestQuality quality = node->Harvest(dayRuler, clockInfo.rulingPlanet);
                lastHarvestLog = "Harvested " + node->GetName() + ": " + PlantNode::GetQualityName(quality);
                logTimer = 3.0f;
            }
        }

        // --- Rendering ---
        BeginDrawing();
            ClearBackground(DARKGRAY);

            // Render world components relative to the scrolling camera
            BeginMode2D(camera);
                gameMap.Draw();
                
                // Draw path debug lines
                if (player.HasActivePath()) {
                    const auto& path = player.GetPath();
                    for (size_t i = player.GetCurrentPathIndex(); i < path.size(); ++i) {
                        DrawCircleV(path[i], 3, Fade(GOLD, 0.5f));
                        if (i > player.GetCurrentPathIndex()) {
                            DrawLineV(path[i-1], path[i], Fade(GOLD, 0.3f));
                        } else {
                            DrawLineV(player.GetPosition(), path[i], Fade(GOLD, 0.3f));
                        }
                    }
                }
                
                player.Draw();
            EndMode2D();

            // Render UI overlays (Absolute screen coordinate space)
            
            // 1. Draw Virtual Joystick (if active)
            if (joystickActive) {
                // Draw base circle
                DrawCircleV(joystickAnchor, joystickMaxRadius, Fade(GRAY, 0.4f));
                DrawCircleLinesV(joystickAnchor, joystickMaxRadius, Fade(LIGHTGRAY, 0.6f));
                
                // Draw center knob
                Vector2 knobPos = Vector2Add(joystickAnchor, Vector2Scale(joystickDirection, joystickMaxRadius * 0.6f));
                DrawCircleV(knobPos, 22, Fade(SKYBLUE, 0.8f));
                DrawCircleLinesV(knobPos, 22, BLUE);
            }

            // 2. Draw Astrological Engine HUD overlay (Top-Left corner)
            DrawRectangle(15, 15, 340, 115, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, 15, 340, 115, Color{ 140, 100, 70, 255 }); // Dark brown border

            std::string timeStr = "Planetary Hour: " + clockInfo.planetName;
            std::string cycleStr = clockInfo.hourIndex < 12 ? "Day Hour (Segment " : "Night Hour (Segment ";
            cycleStr += std::to_string(clockInfo.hourIndex % 12 + 1) + "/12)";
            std::string remainingStr = "Time remaining: " + std::to_string(static_cast<int>(clockInfo.minutesRemaining)) + "m";

            // Draw astrological glyph placeholder
            DrawCircle(40, 50, 18, Color{ 160, 100, 40, 255 });
            DrawText("H", 34, 42, 20, RAYWHITE); // Astro placeholder symbol

            DrawText("ASTROLOGICAL CLOCK", 75, 25, 12, GOLD);
            DrawText(timeStr.c_str(), 75, 42, 16, RAYWHITE);
            DrawText(cycleStr.c_str(), 75, 63, 13, LIGHTGRAY);
            DrawText(remainingStr.c_str(), 75, 80, 13, LIGHTGRAY);
            DrawText("Portland: 45.5N, -122.6W", 75, 100, 11, GRAY);

            // 3. Draw Controls Legend Overlay (Bottom-Left corner)
            DrawRectangle(15, screenHeight - 115, 400, 100, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, screenHeight - 115, 400, 100, Color{ 110, 110, 120, 255 });
            DrawText("CONTROLS:", 30, screenHeight - 105, 12, GOLD);
            DrawText("- Desktop: Use WASD / Arrow Keys to move", 30, screenHeight - 87, 14, RAYWHITE);
            DrawText("- Mobile/Mouse: Drag on LEFT side for Joystick", 30, screenHeight - 69, 14, RAYWHITE);
            DrawText("- Mobile/Mouse: Click/Tap on RIGHT side to pathfind", 30, screenHeight - 51, 14, RAYWHITE);

            // 4. Draw Harvest Log
            DrawRectangle(screenWidth / 2 - 200, 20, 400, 40, Fade(BLACK, 0.6f));
            DrawText(lastHarvestLog.c_str(), screenWidth / 2 - MeasureText(lastHarvestLog.c_str(), 16) / 2, 32, 16, GOLD);

        EndDrawing();
    }

    // De-initialization
    CloseWindow();

    return 0;
}
