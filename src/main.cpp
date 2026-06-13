#include "raylib.h"
#include "raymath.h"
#include "world/player.h"
#include "world/room_manager.h"
#include "world/inventory.h"
#include "core/clock.h"
#include "core/fonts.h"
#include "ui/inventory_ui.h"
#include "nlohmann/json.hpp"
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>

using namespace FloraPhilosophica::World;
using namespace FloraPhilosophica::Core;
using namespace FloraPhilosophica::UI;

// ─────────────────────────────────────────────────────────────────────────────
// Save/Load Helpers
// ─────────────────────────────────────────────────────────────────────────────
void SaveGame(const RoomManager& roomManager, const Inventory& inventory, Vector2 playerPos) {
    nlohmann::json save;
    save["roomData"] = nlohmann::json::parse(roomManager.Serialise());
    save["inventoryData"] = nlohmann::json::parse(inventory.Serialise());
    save["playerPos"] = { {"x", playerPos.x}, {"y", playerPos.y} };
    save["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    std::ofstream o("save.json");
    o << save.dump(2);
    o.close();
}

bool LoadGame(RoomManager& roomManager, Inventory& inventory, Vector2& playerPos, int tileSize) {
    std::ifstream i("save.json");
    if (!i.is_open()) return false;

    try {
        nlohmann::json save;
        i >> save;
        
        if (save.contains("roomData")) {
            roomManager.Deserialise(save["roomData"].dump(), tileSize);
        }
        if (save.contains("inventoryData")) {
            inventory.Deserialise(save["inventoryData"].dump());
        }
        if (save.contains("playerPos")) {
            playerPos.x = save["playerPos"]["x"];
            playerPos.y = save["playerPos"]["y"];
        }
        return true;
    } catch (...) {
        return false;
    }
}

int main() {
    const int screenWidth  = 1280;
    const int screenHeight = 960;
    InitWindow(screenWidth, screenHeight, "Flora Philosophica - Apothecary Sanctuary");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    Font alchemyFont = FloraPhilosophica::Core::LoadAlchemyFont(28);

    const int TILE_SIZE = 64;
    RoomManager roomManager(TILE_SIZE);
    roomManager.Initialize();

    Inventory inventory;
    Vector2 playerStartPos = { 640.0f, 480.0f };

    if (!LoadGame(roomManager, inventory, playerStartPos, TILE_SIZE)) {
        inventory.AddHarvestItem({ "Wine", PlantStage::Spirits, HarvestQuality::Standard });
        inventory.AddHarvestItem({ "Wine", PlantStage::Spirits, HarvestQuality::Standard });
        inventory.AddHarvestItem({ "Wine", PlantStage::Spirits, HarvestQuality::Standard });
        inventory.AddHarvestItem({ "Lavender", PlantStage::Fresh, HarvestQuality::Standard });
        inventory.AddHarvestItem({ "Mugwort", PlantStage::Fresh, HarvestQuality::Standard });
    }

    Player player(playerStartPos);
    AstrologicalClock clock;
    const double observerLat =  42.3265;
    const double observerLon = -122.8756;

    Camera2D camera = { 0 };
    camera.target   = player.GetPosition();
    camera.offset   = { screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    bool joystickActive        = false;
    Vector2 joystickAnchor     = { 0.0f, 0.0f };
    Vector2 joystickDirection  = { 0.0f, 0.0f };
    const float joystickMaxRadius    = 64.0f;
    const float joystickHoldThreshold = 0.2f;
    float clickHoldTimer       = 0.0f;
    bool isHolding             = false;

    InventoryUI inventoryUI;
    bool isPaused = false;
    int pauseMenuSelection = 0;

    const char* PLANET_GLYPHS[7] = { "♄", "♃", "♂", "☉", "♀", "☿", "☽" };
    static std::string interactionLog = "Walk near objects and press [E] to interact";
    static float logTimer = 0.0f;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        Vector2 mouseScreenPos = GetMousePosition();
        Vector2 mouseWorldPos  = GetScreenToWorld2D(mouseScreenPos, camera);
        bool leftPressed   = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        bool leftDown      = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool leftReleased  = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        joystickDirection = { 0.0f, 0.0f };

        // ── Global Inputs (Pause, Esc, Toggles) ───────────────────────────
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (inventoryUI.IsAnyUIOpen()) {
                inventoryUI.CloseActiveStationUIs();
                if (inventoryUI.IsOpen()) inventoryUI.Toggle();
            } else {
                isPaused = !isPaused;
                if (isPaused) pauseMenuSelection = 0;
            }
        }

        if (IsKeyPressed(KEY_F11) || ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))) {
            ToggleFullscreen();
        }

        if (isPaused) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) pauseMenuSelection = (pauseMenuSelection - 1 + 4) % 4;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) pauseMenuSelection = (pauseMenuSelection + 1) % 4;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (pauseMenuSelection == 0) isPaused = false;
                else if (pauseMenuSelection == 1) { SaveGame(roomManager, inventory, player.GetPosition()); interactionLog = "Game Saved."; logTimer = 2.0f; isPaused = false; }
                else if (pauseMenuSelection == 2) ToggleFullscreen();
                else if (pauseMenuSelection == 3) { SaveGame(roomManager, inventory, player.GetPosition()); break; }
            }
        }

        if (!isPaused) {
            // Station auto-close logic
            if (inventoryUI.GetActiveDryingRack() && !inventoryUI.GetActiveDryingRack()->IsPlayerNear(player.GetPosition())) inventoryUI.SetActiveDryingRack(nullptr);
            if (inventoryUI.GetActiveStorage() && !inventoryUI.GetActiveStorage()->IsPlayerNear(player.GetPosition())) inventoryUI.SetActiveStorage(nullptr);

            if (IsKeyPressed(KEY_I)) inventoryUI.Toggle();

            for (int i = 0; i < 10; ++i) {
                int key = (i == 9) ? KEY_ZERO : (KEY_ONE + i);
                if (IsKeyPressed(key)) inventoryUI.SetSelectedSlot(i);
            }

            if (IsKeyPressed(KEY_B)) {
                roomManager.ToggleBuildMode();
                interactionLog = roomManager.IsInBuildMode() ? "BUILD MODE: [E] Pick up, [X] Delete, [B] Exit" : "Exit Build Mode.";
                logTimer = roomManager.IsInBuildMode() ? 4.0f : 1.5f;
            }

            // ── INTERACTION HANDLER ───────────────────────────────────────
            // 1. UI Interaction & Drag-and-Drop
            if (inventoryUI.IsOpen()) {
                inventoryUI.UpdateDragging(mouseScreenPos, inventory);
                
                // Clicking a station in the inventory to start placement
                if (leftPressed) {
                    InventoryUI::SlotClickResult click = inventoryUI.GetSlotAt(mouseScreenPos);
                    if (click.index != -1) {
                        const auto& slot = inventory.GetSlot(click.index);
                        if (slot.occupied) {
                            if (click.index < 10) {
                                inventoryUI.SetSelectedSlot(click.index);
                            } else if (!slot.isHerb) {
                                // Station placement
                                inventoryUI.Toggle();
                                roomManager.StartPlacement(slot.station);
                                interactionLog = "Click to place station. [B] to cancel.";
                                logTimer = 5.0f;
                            }
                        }
                    }
                }
            }
            // 2. Build Mode / Placement
            else if (roomManager.IsInPlacementMode()) {
                roomManager.UpdatePlacementGhost(mouseWorldPos);
                if (leftPressed) {
                    if (roomManager.ConfirmPlacement(inventory)) {
                        interactionLog = "Item placed."; logTimer = 2.0f; SaveGame(roomManager, inventory, player.GetPosition());
                    } else { interactionLog = "Cannot place here."; logTimer = 1.5f; }
                }
            }
            else if (roomManager.IsInBuildMode()) {
                if (IsKeyPressed(KEY_E)) {
                    if (roomManager.PickupItem(player.GetPosition(), inventory)) {
                        interactionLog = "Item returned to inventory."; logTimer = 2.0f; SaveGame(roomManager, inventory, player.GetPosition());
                    }
                }
                if (IsKeyPressed(KEY_X)) {
                    if (roomManager.RemoveItem(player.GetPosition())) {
                        interactionLog = "Item removed."; logTimer = 2.0f; SaveGame(roomManager, inventory, player.GetPosition());
                    }
                }
            }
            // 3. Movement / Pathfinding (Only if UI is NOT open)
            else if (!inventoryUI.IsAnyUIOpen()) {
                if (leftPressed) { isHolding = true; clickHoldTimer = 0.0f; joystickAnchor = mouseScreenPos; }
                if (isHolding) {
                    clickHoldTimer += deltaTime;
                    if (clickHoldTimer >= joystickHoldThreshold && !joystickActive) joystickActive = true;
                    if (leftReleased) {
                        if (!joystickActive) {
                            auto path = roomManager.GetActiveMap().FindPath(player.GetPosition(), mouseWorldPos);
                            player.SetPath(path);
                        }
                        isHolding = false; joystickActive = false; clickHoldTimer = 0.0f;
                    }
                }
                if (joystickActive && leftDown) {
                    Vector2 offset = Vector2Subtract(mouseScreenPos, joystickAnchor);
                    float distance = Vector2Length(offset);
                    if (distance > 0.0f) {
                        Vector2 norm = Vector2Scale(offset, 1.0f / distance);
                        if (distance > joystickMaxRadius) offset = Vector2Scale(norm, joystickMaxRadius);
                        joystickDirection = norm;
                    }
                }
            }

            // ── World Interaction ([E] Key) ──────────────────────────────
            if (IsKeyPressed(KEY_E) && !roomManager.IsInBuildMode() && !inventoryUI.IsAnyUIOpen()) {
                std::string inspectionMessage;
                InteractionResult result = roomManager.TryInteract(player.GetPosition(), inspectionMessage);
                switch (result) {
                    case InteractionResult::InspectDecoration:
                        interactionLog = inspectionMessage; logTimer = 6.0f; break;
                    case InteractionResult::OpenApparatus: {
                        PlacedItem* station = roomManager.GetNearestPlacedItem(player.GetPosition());
                        if (!station) break;
                        if (station->GetType() == ItemType::DryingRack) inventoryUI.SetActiveDryingRack(station);
                        else {
                            long long nowUtc = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                            if (station->GetType() == ItemType::MortarAndPestle) {
                                int sel = inventoryUI.GetSelectedSlot();
                                const auto& slot = inventory.GetSlot(sel);
                                if (slot.occupied && slot.isHerb && slot.herb.stage == PlantStage::Dried) {
                                    if (station->LoadHarvestItem(slot.herb, nowUtc)) {
                                        inventory.ClearSlot(sel);
                                        HarvestItem out; station->UnloadProcessedItem(out, nowUtc);
                                        inventory.AddHarvestItem(out);
                                        interactionLog = "Ground " + out.plantName + " into powder."; logTimer = 3.0f;
                                    }
                                } else { interactionLog = "Requires a Dried herb in your hotbar."; logTimer = 2.5f; }
                            } else if (station->GetType() == ItemType::MacerationJar) {
                                if (station->IsProcessComplete(nowUtc) && !station->GetLoadedItemName().empty()) {
                                    HarvestItem tincture; if (station->UnloadProcessedItem(tincture, nowUtc)) {
                                        inventory.AddHarvestItem(tincture); HarvestItem spent = tincture; spent.stage = PlantStage::Spent;
                                        inventory.AddHarvestItem(spent); interactionLog = "Collected tincture and residue."; logTimer = 3.0f;
                                    }
                                } else if (station->GetLoadedItemName().empty()) {
                                    int sel = inventoryUI.GetSelectedSlot();
                                    const auto& slot = inventory.GetSlot(sel);
                                    if (slot.occupied && slot.isHerb && (slot.herb.stage == PlantStage::Ground || slot.herb.stage == PlantStage::Dried)) {
                                        // Find spirits in inventory
                                        int spiritsIdx = -1;
                                        for (int i=0; i<Inventory::TOTAL_SLOTS; ++i) {
                                            const auto& s = inventory.GetSlot(i);
                                            if (s.occupied && s.isHerb && s.herb.stage == PlantStage::Spirits) { spiritsIdx = i; break; }
                                        }
                                        if (spiritsIdx != -1 && station->LoadHarvestItem(slot.herb, nowUtc)) {
                                            inventory.ClearSlot(sel); inventory.ClearSlot(spiritsIdx);
                                            interactionLog = "Started maceration. Ready in 1 hour."; logTimer = 3.0f;
                                        } else if (spiritsIdx == -1) { interactionLog = "Requires Spirits (alcohol)."; logTimer = 3.0f; }
                                    }
                                }
                            } else if (station->GetType() == ItemType::CompostBin) {
                                int sel = inventoryUI.GetSelectedSlot();
                                const auto& slot = inventory.GetSlot(sel);
                                if (slot.occupied && slot.isHerb) {
                                    std::string name = slot.herb.plantName;
                                    if (station->LoadHarvestItem(slot.herb, nowUtc)) {
                                        HarvestItem out; 
                                        if (station->UnloadProcessedItem(out, nowUtc)) {
                                            inventory.ClearSlot(sel);
                                            interactionLog = "Composted " + name + "."; logTimer = 2.5f;
                                        } else {
                                            interactionLog = "[Compost Error: Could not unload]"; logTimer = 2.0f;
                                        }
                                    } else {
                                        interactionLog = "[Compost Error: Could not load]"; logTimer = 2.0f;
                                    }
                                } else { interactionLog = "Select an herb to compost first."; logTimer = 2.0f; }
                            }

                        }
                        break;
                    }
                    case InteractionResult::OpenStorage: {
                        PlacedItem* station = roomManager.GetNearestPlacedItem(player.GetPosition());
                        if (station && station->GetType() == ItemType::StorageChest) inventoryUI.SetActiveStorage(station);
                        break;
                    }
                    case InteractionResult::OpenMailbox: interactionLog = "[Mailbox — no letters yet]"; logTimer = 2.0f; break;
                    case InteractionResult::None: {
                        PlantNode* node = roomManager.GetActiveMap().CheckPlantInteraction(player.GetPosition(), 24.0f);
                        if (node) {
                            long long now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                            PlanetaryHourInfo info = clock.CalculatePlanetaryHour(observerLat, observerLon, now);
                            HarvestQuality q = node->Harvest(info.dayRuler, info.rulingPlanet);
                            inventory.AddHarvestItem(node->GetName(), q);
                            interactionLog = "Harvested " + node->GetName() + ": " + PlantNode::GetQualityName(q); logTimer = 3.0f;
                        }
                        break;
                    }
                    default: break;
                }
            }

            Vector2 oldPos = player.GetPosition();
            player.Update(deltaTime, joystickActive, joystickDirection);
            Vector2 resolved = roomManager.GetActiveMap().ConstrainPosition(oldPos, player.GetPosition(), 15.0f);
            player.SetPosition(resolved);
            roomManager.Update(deltaTime, player.GetPosition());

            const RoomTransition* transition = roomManager.GetPendingTransition();
            if (transition) {
                SaveGame(roomManager, inventory, player.GetPosition());
                roomManager.TransitionTo(transition->targetRoom, transition->targetEntryPoint);
                player.SetPosition(transition->targetEntryPoint); player.ClearPath();
                camera.target = transition->targetEntryPoint; interactionLog = "Entered " + transition->label; logTimer = 2.0f;
            }
            camera.target = player.GetPosition();
            if (logTimer > 0.0f) logTimer -= deltaTime;
            else interactionLog = "Walk near objects and press [E] to interact";
        }

        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode2D(camera);
                roomManager.Draw();
                if (player.HasActivePath()) {
                    const auto& path = player.GetPath();
                    for (size_t i = player.GetCurrentPathIndex(); i < path.size(); ++i) {
                        DrawCircleV(path[i], 3, Fade(GOLD, 0.5f));
                        if (i > (size_t)player.GetCurrentPathIndex()) DrawLineV(path[i-1], path[i], Fade(GOLD, 0.3f));
                        else DrawLineV(player.GetPosition(), path[i], Fade(GOLD, 0.3f));
                    }
                }
                player.Draw();
                roomManager.DrawPlacementGhost(mouseWorldPos);
            EndMode2D();

            if (joystickActive) {
                DrawCircleV(joystickAnchor, joystickMaxRadius, Fade(GRAY, 0.4f));
                DrawCircleLinesV(joystickAnchor, joystickMaxRadius, Fade(LIGHTGRAY, 0.6f));
                Vector2 knobPos = Vector2Add(joystickAnchor, Vector2Scale(joystickDirection, joystickMaxRadius * 0.6f));
                DrawCircleV(knobPos, 22, Fade(SKYBLUE, 0.8f));
                DrawCircleLinesV(knobPos, 22, BLUE);
            }

            DrawRectangle(15, 15, 340, 115, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, 15, 340, 115, Color{ 140, 100, 70, 255 });
            {
                long long now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                PlanetaryHourInfo info = clock.CalculatePlanetaryHour(observerLat, observerLon, now);
                const char* glyph = PLANET_GLYPHS[static_cast<int>(info.rulingPlanet)];
                DrawCircle(40, 72, 18, Color{ 160, 100, 40, 255 });
                DrawTextEx(alchemyFont, glyph, Vector2{ 28.0f, 62.0f }, 24, 0, RAYWHITE);
                DrawText("ASTROLOGICAL CLOCK", 75, 25, 12, GOLD);
                DrawText(("Planetary Hour: " + info.planetName).c_str(), 75, 42, 16, RAYWHITE);
                std::string cycle = (info.hourIndex < 12) ? "Day Hour (Segment " : "Night Hour (Segment ";
                cycle += std::to_string(info.hourIndex % 12 + 1) + "/12)";
                DrawText(cycle.c_str(), 75, 63, 13, LIGHTGRAY);
                DrawText(("Time remaining: " + std::to_string(static_cast<int>(info.minutesRemaining)) + "m").c_str(), 75, 80, 13, LIGHTGRAY);
            }

            inventoryUI.Draw(inventory);
            inventoryUI.DrawHotbar(inventory);

            DrawRectangle(15, screenHeight - 128, 290, 105, Fade(BLACK, 0.7f));
            DrawRectangleLines(15, screenHeight - 128, 290, 105, Color{ 110, 110, 120, 255 });
            DrawText("CONTROLS:", 30, screenHeight - 118, 12, GOLD);
            DrawText("- WASD / Arrows to move", 30, screenHeight - 100, 13, RAYWHITE);
            DrawText("- [E] Interact / Harvest", 30, screenHeight - 82, 13, RAYWHITE);
            DrawText("- [I] Inventory   [B] Build Mode", 30, screenHeight - 64, 13, RAYWHITE);
            DrawText("- [ESC] Pause Menu", 30, screenHeight - 46, 13, RAYWHITE);

            int logW = 700; int logX = screenWidth / 2 - logW / 2;
            DrawRectangle(logX, 20, logW, 50, Fade(BLACK, 0.6f));
            std::string disp = interactionLog; if (MeasureText(disp.c_str(), 15) > logW - 20) { while (disp.size() > 80) disp.pop_back(); disp += "..."; }
            DrawText(disp.c_str(), screenWidth / 2 - MeasureText(disp.c_str(), 15) / 2, 36, 15, GOLD);

            if (inventoryUI.GetActiveDryingRack()) inventoryUI.DrawDryingRackUI(inventory);
            else if (inventoryUI.GetActiveStorage()) inventoryUI.DrawStorageUI(inventory);

            if (isPaused) {
                int sw = GetScreenWidth(); int sh = GetScreenHeight();
                DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));
                int menuW = 320; int menuH = 310; int menuX = (sw - menuW) / 2; int menuY = (sh - menuH) / 2;
                DrawRectangle(menuX, menuY, menuW, menuH, Fade(Color{ 30, 25, 20, 255 }, 0.92f));
                DrawRectangleLinesEx(Rectangle{ (float)menuX, (float)menuY, (float)menuW, (float)menuH }, 2.0f, Color{ 140, 100, 70, 255 });
                DrawText("GAME PAUSED", menuX + menuW / 2 - MeasureText("GAME PAUSED", 20) / 2, menuY + 24, 20, GOLD);
                int btnW = 220, btnH = 36, btnX = menuX + (menuW - btnW) / 2, startBtnY = menuY + 74, btnSpacing = 12;
                auto drawBtn = [&](int idx, const char* label, Color normal, Color hoverCol) {
                    Rectangle r = { (float)btnX, (float)(startBtnY + idx * (btnH + btnSpacing)), (float)btnW, (float)btnH };
                    bool h = CheckCollisionPointRec(mouseScreenPos, r); if (h) pauseMenuSelection = idx;
                    DrawRectangleRec(r, (pauseMenuSelection == idx) ? hoverCol : normal);
                    DrawRectangleLinesEx(r, 1.0f, (pauseMenuSelection == idx) ? GOLD : Color{ 100, 80, 60, 200 });
                    DrawText(label, btnX + btnW / 2 - MeasureText(label, 13) / 2, static_cast<int>(r.y) + 11, 13, RAYWHITE);
                    return h && leftPressed;
                };
                if (drawBtn(0, "Resume Game", Color{ 45, 40, 35, 255 }, Color{ 60, 50, 45, 255 })) isPaused = false;
                if (drawBtn(1, "Save Game", Color{ 45, 40, 35, 255 }, Color{ 60, 50, 45, 255 })) { SaveGame(roomManager, inventory, player.GetPosition()); interactionLog = "Game Saved."; logTimer = 2.0f; isPaused = false; }
                if (drawBtn(2, "Toggle Fullscreen", Color{ 45, 40, 35, 255 }, Color{ 60, 50, 45, 255 })) ToggleFullscreen();
                if (drawBtn(3, "Quit to Desktop", Color{ 55, 30, 30, 255 }, Color{ 80, 40, 40, 255 })) { SaveGame(roomManager, inventory, player.GetPosition()); break; }
            }
        EndDrawing();
    }
    UnloadFont(alchemyFont);
    CloseWindow();
    return 0;
}
