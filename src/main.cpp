#include "raylib.h"
#include "raymath.h"
#include "world/player.h"
#include "world/room_manager.h"
#include "world/inventory.h"
#include "core/clock.h"
#include "core/fonts.h"
#include "ui/inventory_ui.h"
#include <chrono>
#include <string>

int main() {
    // 1. Initialise the Raylib Window
    const int screenWidth  = 1200;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Flora Philosophica - Apothecary Sanctuary");
    SetTargetFPS(60);

    // Load the Archemy alchemical font for planetary glyphs.
    // Falls back to Raylib default if assets/fonts/Archemy.otf is missing.
    Font alchemyFont = FloraPhilosophica::Core::LoadAlchemyFont(28);

    using namespace FloraPhilosophica::World;
    using namespace FloraPhilosophica::Core;
    using namespace FloraPhilosophica::UI;

    // 2. Instantiate Game Systems
    const int TILE_SIZE = 60;

    // Room manager owns all maps and placed items
    RoomManager roomManager(TILE_SIZE);
    roomManager.Initialize();

    // Player inventory — starts empty (inherited items are pre-placed, not in inventory)
    // TODO: Load from save file on startup
    Inventory inventory;

    // Player spawns on the exterior map near the cabin approach
    Player player({ 600.0f, 450.0f });

    // Astrological engine
    AstrologicalClock clock;
    // Medford, Oregon — matches device IP geolocation
    const double observerLat =  42.3265;
    const double observerLon = -122.8756;

    // 3. Camera2D — follows the player
    Camera2D camera = { 0 };
    camera.target   = player.GetPosition();
    camera.offset   = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    // 4. Input state
    bool joystickActive        = false;
    Vector2 joystickAnchor     = { 0.0f, 0.0f };
    Vector2 joystickDirection  = { 0.0f, 0.0f };
    const float joystickMaxRadius    = 60.0f;
    const float joystickHoldThreshold = 0.2f;
    float clickHoldTimer       = 0.0f;
    bool isHolding             = false;

    // 5. UI systems
    InventoryUI inventoryUI;

    // Planetary glyph lookup — matches Planet enum (Saturn=0 ... Moon=6)
    const char* PLANET_GLYPHS[7] = { "♄", "♃", "♂", "☉", "♀", "☿", "☽" };

    // Interaction / harvest log
    static std::string interactionLog = "Walk near objects and press [E] to interact";
    static float logTimer = 0.0f;

    // ── MAIN GAME LOOP ──────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // ── Input ─────────────────────────────────────────────────────────
        Vector2 mouseScreenPos = GetMousePosition();
        Vector2 mouseWorldPos  = GetScreenToWorld2D(mouseScreenPos, camera);
        bool leftPressed   = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool leftDown      = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool leftReleased  = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        joystickDirection = { 0.0f, 0.0f };

        // ── Inventory toggle ──────────────────────────────────────────────
        if (IsKeyPressed(KEY_I)) {
            inventoryUI.Toggle();
        }

        // ── Placement mode input ──────────────────────────────────────────
        if (roomManager.IsInPlacementMode()) {
            roomManager.UpdatePlacementGhost(mouseWorldPos);

            if (leftPressed) {
                // Left click = confirm placement
                if (roomManager.ConfirmPlacement(inventory)) {
                    interactionLog = "Item placed.";
                    logTimer = 2.0f;
                } else {
                    interactionLog = "Cannot place here.";
                    logTimer = 1.5f;
                }
            }
            // B cancels placement — same key used to open inventory placement
            if (IsKeyPressed(KEY_B)) {
                roomManager.CancelPlacement(inventory);
                interactionLog = "Placement cancelled.";
                logTimer = 1.5f;
            }
        }
        // ── Normal input (not in placement mode) ─────────────────────────
        else {
            // Inventory panel click — start placement for clicked item
            if (leftPressed && inventoryUI.IsOpen()) {
                ItemType clicked = inventoryUI.HandleClick(mouseScreenPos, inventory);
                if (clicked != ItemType::COUNT) {
                    inventoryUI.Toggle(); // close panel
                    roomManager.StartPlacement(clicked);
                    interactionLog = "Click to place. [B] to cancel.";
                    logTimer = 5.0f;
                }
            }

            // Joystick / tap-to-move
            if (leftPressed) {
                isHolding      = true;
                clickHoldTimer = 0.0f;
                joystickAnchor = mouseScreenPos;
            }

            if (isHolding) {
                clickHoldTimer += deltaTime;
                if (clickHoldTimer >= joystickHoldThreshold && !joystickActive) {
                    joystickActive = true;
                }
                if (leftReleased) {
                    if (!joystickActive) {
                        // Tap: pathfind to tapped world position
                        auto& activeMap = roomManager.GetActiveMap();
                        auto path = activeMap.FindPath(player.GetPosition(), mouseWorldPos);
                        player.SetPath(path);
                    }
                    isHolding      = false;
                    joystickActive = false;
                    clickHoldTimer = 0.0f;
                }
            }

            if (joystickActive && leftDown) {
                Vector2 offset   = Vector2Subtract(mouseScreenPos, joystickAnchor);
                float   distance = Vector2Length(offset);
                if (distance > 0.0f) {
                    Vector2 norm = Vector2Scale(offset, 1.0f / distance);
                    if (distance > joystickMaxRadius)
                        offset = Vector2Scale(norm, joystickMaxRadius);
                    joystickDirection = norm;
                }
            }
        }

        // ── E key: interact with placed items or harvest plants ───────────
        if (IsKeyPressed(KEY_E)) {
            // First try placed item interaction
            std::string inspectionMessage;
            InteractionResult result = roomManager.TryInteract(player.GetPosition(), inspectionMessage);
            switch (result) {
                case InteractionResult::InspectDecoration:
                    interactionLog = inspectionMessage;
                    logTimer = 6.0f;
                    break;
                case InteractionResult::OpenApparatus:
                    interactionLog = "[Apparatus UI — coming in Step 4]";
                    logTimer = 2.0f;
                    break;
                case InteractionResult::OpenCompost:
                    interactionLog = "Compost bin: place spent plant material here. Cannot be retrieved.";
                    logTimer = 3.0f;
                    break;
                case InteractionResult::OpenMailbox:
                    interactionLog = "[Mailbox — no letters yet]";
                    logTimer = 2.0f;
                    break;
                case InteractionResult::OpenStorage:
                    interactionLog = "[Storage UI — coming soon]";
                    logTimer = 2.0f;
                    break;
                case InteractionResult::None: {
                    // Try plant harvest if no placed item was nearby
                    auto& activeMap = roomManager.GetActiveMap();
                    PlantNode* node = activeMap.CheckPlantInteraction(player.GetPosition(), 20.0f);
                    if (node) {
                        long long currentUnixTime = std::chrono::duration_cast<std::chrono::seconds>(
                            std::chrono::system_clock::now().time_since_epoch()
                        ).count();
                        PlanetaryHourInfo clockInfo = clock.CalculatePlanetaryHour(
                            observerLat, observerLon, currentUnixTime);
                        HarvestQuality quality = node->Harvest(clockInfo.dayRuler, clockInfo.rulingPlanet);

                        // Add the harvested plant to the player's inventory
                        ItemType plantItem = PlantNode::GetPlantItemType(node->GetName());
                        inventory.AddItem(plantItem, 1);

                        interactionLog = "Harvested " + node->GetName() + ": "
                                       + PlantNode::GetQualityName(quality);
                        logTimer = 3.0f;
                    }
                    break;
                }
                default: break;
            }
        }

        // ── Update ────────────────────────────────────────────────────────
        Vector2 oldPos = player.GetPosition();
        player.Update(deltaTime, joystickActive, joystickDirection);

        // Constrain against active room's obstacles
        auto& activeMap  = roomManager.GetActiveMap();
        Vector2 proposed = player.GetPosition();
        Vector2 resolved = activeMap.ConstrainPosition(oldPos, proposed, 15.0f);
        player.SetPosition(resolved);

        // Update room (plant respawns, placed item animations, transition checks)
        roomManager.Update(deltaTime, player.GetPosition());

        // Handle room transition
        const RoomTransition* transition = roomManager.GetPendingTransition();
        if (transition) {
            Vector2 entryPoint = transition->targetEntryPoint;
            RoomID  targetRoom = transition->targetRoom;
            roomManager.TransitionTo(targetRoom, entryPoint);
            player.SetPosition(entryPoint);
            player.ClearPath(); // cancel any active pathfinding from the previous room
            camera.target = entryPoint;
            interactionLog = "Entered " + transition->label;
            logTimer = 2.0f;
        }

        camera.target = player.GetPosition();

        // Interaction log timer
        if (logTimer > 0.0f) logTimer -= deltaTime;
        else interactionLog = "Walk near objects and press [E] to interact";

        // ── Astrological clock ────────────────────────────────────────────
        long long currentUnixTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        PlanetaryHourInfo clockInfo = clock.CalculatePlanetaryHour(
            observerLat, observerLon, currentUnixTime);

        // ── Rendering ─────────────────────────────────────────────────────
        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode2D(camera);
                roomManager.Draw();

                // Path debug visualisation
                if (player.HasActivePath()) {
                    const auto& path = player.GetPath();
                    for (size_t i = player.GetCurrentPathIndex(); i < path.size(); ++i) {
                        DrawCircleV(path[i], 3, Fade(GOLD, 0.5f));
                        if (i > (size_t)player.GetCurrentPathIndex())
                            DrawLineV(path[i-1], path[i], Fade(GOLD, 0.3f));
                        else
                            DrawLineV(player.GetPosition(), path[i], Fade(GOLD, 0.3f));
                    }
                }

                player.Draw();

                // Placement ghost (drawn in world space)
                roomManager.DrawPlacementGhost(mouseWorldPos);
            EndMode2D();

            // ── Screen-space UI ───────────────────────────────────────────

            // Virtual joystick
            if (joystickActive) {
                DrawCircleV(joystickAnchor, joystickMaxRadius, Fade(GRAY, 0.4f));
                DrawCircleLinesV(joystickAnchor, joystickMaxRadius, Fade(LIGHTGRAY, 0.6f));
                Vector2 knobPos = Vector2Add(joystickAnchor,
                    Vector2Scale(joystickDirection, joystickMaxRadius * 0.6f));
                DrawCircleV(knobPos, 22, Fade(SKYBLUE, 0.8f));
                DrawCircleLinesV(knobPos, 22, BLUE);
            }

            // Astrological clock HUD (top-left)
            DrawRectangle(15, 15, 340, 115, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, 15, 340, 115, Color{ 140, 100, 70, 255 });
            {
                int planetIdx = static_cast<int>(clockInfo.rulingPlanet);
                const char* glyph = PLANET_GLYPHS[planetIdx];
                std::string timeStr  = "Planetary Hour: " + clockInfo.planetName;
                std::string cycleStr = (clockInfo.hourIndex < 12)
                    ? "Day Hour (Segment " : "Night Hour (Segment ";
                cycleStr += std::to_string(clockInfo.hourIndex % 12 + 1) + "/12)";
                std::string remStr = "Time remaining: "
                    + std::to_string(static_cast<int>(clockInfo.minutesRemaining)) + "m";

                DrawCircle(40, 72, 18, Color{ 160, 100, 40, 255 });
                DrawTextEx(alchemyFont, glyph, Vector2{ 28.0f, 62.0f }, 24, 0, RAYWHITE);
                DrawText("ASTROLOGICAL CLOCK", 75, 25, 12, GOLD);
                DrawText(timeStr.c_str(),  75, 42, 16, RAYWHITE);
                DrawText(cycleStr.c_str(), 75, 63, 13, LIGHTGRAY);
                DrawText(remStr.c_str(),   75, 80, 13, LIGHTGRAY);
                DrawText("Medford: 42.3N, -122.8W", 75, 100, 11, GRAY);
            }

            // Room indicator (top-left, below clock)
            {
                const char* roomName = "Exterior";
                switch (roomManager.GetActiveRoomID()) {
                    case RoomID::CabinMain: roomName = "Cabin Interior"; break;
                    case RoomID::CabinLoft: roomName = "Cabin Loft";     break;
                    case RoomID::Garden:    roomName = "Garden";          break;
                    default: break;
                }
                DrawRectangle(15, 140, 200, 28, Fade(BLACK, 0.7f));
                DrawRectangleLines(15, 140, 200, 28, Color{ 100, 80, 60, 200 });
                DrawText(roomName, 24, 148, 13, LIGHTGRAY);
            }

            // Inventory UI (right side)
            inventoryUI.Draw(inventory);

            // Controls legend (bottom-left)
            DrawRectangle(15, screenHeight - 120, 420, 105, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, screenHeight - 120, 420, 105, Color{ 110, 110, 120, 255 });
            DrawText("CONTROLS:",                                          30, screenHeight - 110, 12, GOLD);
            DrawText("- WASD / Arrow Keys to move",                       30, screenHeight -  92, 13, RAYWHITE);
            DrawText("- Hold click/drag to use joystick",                 30, screenHeight -  74, 13, RAYWHITE);
            DrawText("- Tap to pathfind",                                 30, screenHeight -  56, 13, RAYWHITE);
            DrawText("- [E] Interact / Harvest   [I] Inventory",          30, screenHeight -  38, 13, RAYWHITE);
            DrawText("- [B] Cancel placement",                            30, screenHeight -  20, 13, RAYWHITE);

            // Interaction log (top-centre) — wide enough for inspection messages
            int logW = 700;
            int logX = screenWidth / 2 - logW / 2;
            DrawRectangle(logX, 20, logW, 50, Fade(BLACK, 0.6f));
            // Word-wrap is not available in Raylib's DrawText, so we truncate
            // long messages to fit — full messages are readable in a future
            // dialogue panel (Step 8).
            std::string displayLog = interactionLog;
            if (MeasureText(displayLog.c_str(), 15) > logW - 20) {
                // Truncate to roughly 80 characters with ellipsis
                while (displayLog.size() > 80) displayLog.pop_back();
                displayLog += "...";
            }
            DrawText(displayLog.c_str(),
                screenWidth / 2 - MeasureText(displayLog.c_str(), 15) / 2,
                36, 15, GOLD);

        EndDrawing();
    }

    // Unload the font texture before closing the window
    UnloadFont(alchemyFont);
    CloseWindow();
    return 0;
}
