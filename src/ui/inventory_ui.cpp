#include "inventory_ui.h"
#include "world/item.h"
#include <string>

namespace FloraPhilosophica {
namespace UI {

InventoryUI::InventoryUI()
    : m_open(false)
    , m_selectedSlot(0)
    , m_draggedSlot(-1)
    , m_isDragging(false)
    , m_activeDryingRack(nullptr)
    , m_activeStorage(nullptr)
{}

void InventoryUI::Toggle() {
    m_open = !m_open;
    if (!m_open) {
        m_isDragging = false;
        m_draggedSlot = -1;
    }
}

Rectangle InventoryUI::GetGridSlotRect(int row, int col) const {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int gridW = GRID_COLS * SLOT_SIZE + (GRID_COLS - 1) * SLOT_PADDING;
    int gridH = (GRID_ROWS + 1) * SLOT_SIZE + GRID_ROWS * SLOT_PADDING + 20; 
    int startX = (sw - gridW) / 2;
    int startY = (sh - gridH) / 2;

    return Rectangle{
        static_cast<float>(startX + col * (SLOT_SIZE + SLOT_PADDING)),
        static_cast<float>(startY + row * (SLOT_SIZE + SLOT_PADDING)),
        static_cast<float>(SLOT_SIZE),
        static_cast<float>(SLOT_SIZE)
    };
}

Rectangle InventoryUI::GetHotbarSlotRectInside(int i) const {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int gridW = GRID_COLS * SLOT_SIZE + (GRID_COLS - 1) * SLOT_PADDING;
    int hotbarW = HOTBAR_COLS * SLOT_SIZE + (HOTBAR_COLS - 1) * SLOT_PADDING;
    int gridH = (GRID_ROWS + 1) * SLOT_SIZE + GRID_ROWS * SLOT_PADDING + 20;
    
    int startX = (sw - gridW) / 2 + (gridW - hotbarW) / 2;
    int startY = (sh - gridH) / 2 + (GRID_ROWS * (SLOT_SIZE + SLOT_PADDING)) + 20;

    return Rectangle{
        static_cast<float>(startX + i * (SLOT_SIZE + SLOT_PADDING)),
        static_cast<float>(startY),
        static_cast<float>(SLOT_SIZE),
        static_cast<float>(SLOT_SIZE)
    };
}

InventoryUI::SlotClickResult InventoryUI::GetSlotAt(Vector2 mousePos) const {
    SlotClickResult res;
    if (!m_open) return res;

    // Check Hotbar (0-9)
    for (int i = 0; i < 10; ++i) {
        if (CheckCollisionPointRec(mousePos, GetHotbarSlotRectInside(i))) {
            res.index = i;
            return res;
        }
    }

    // Check Grid (10-45)
    for (int i = 0; i < GRID_ROWS * GRID_COLS; ++i) {
        if (CheckCollisionPointRec(mousePos, GetGridSlotRect(i / GRID_COLS, i % GRID_COLS))) {
            res.index = 10 + i;
            return res;
        }
    }

    return res;
}

void InventoryUI::UpdateDragging(Vector2 mousePos, World::Inventory& inventory) {
    if (!m_open) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        SlotClickResult slot = GetSlotAt(mousePos);
        if (slot.index != -1 && inventory.GetSlot(slot.index).occupied) {
            m_draggedSlot = slot.index;
            m_isDragging = true;
        }
    }

    if (m_isDragging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        SlotClickResult target = GetSlotAt(mousePos);
        if (target.index != -1 && target.index != m_draggedSlot) {
            inventory.SwapSlots(m_draggedSlot, target.index);
        }
        m_isDragging = false;
        m_draggedSlot = -1;
    }
}

static void DrawInventorySlot(const World::InventorySlot& s, Rectangle rect, bool isSelected, bool isDragged) {
    DrawRectangleRec(rect, Fade(Color{ 45, 55, 40, 255 }, isDragged ? 0.3f : 0.5f));
    if (isSelected) {
        DrawRectangleLinesEx(rect, 2.0f, GOLD);
        DrawRectangleRec(rect, Fade(GOLD, 0.1f));
    } else {
        DrawRectangleLinesEx(rect, 1.0f, Color{ 100, 80, 60, 200 });
    }

    if (s.occupied && !isDragged) {
        if (s.isHerb) {
            Color qualColor = GRAY;
            if (s.herb.quality == World::HarvestQuality::Pristine) qualColor = GOLD;
            else if (s.herb.quality == World::HarvestQuality::Standard) qualColor = WHITE;
            DrawRectangle(rect.x + 4, rect.y + rect.height - 5, rect.width - 8, 2, qualColor);
            std::string stage = World::HarvestItem::GetStageName(s.herb.stage).substr(0, 1);
            DrawText(stage.c_str(), rect.x + rect.width - 10, rect.y + 4, 9, ColorAlpha(RAYWHITE, 0.6f));
            std::string name = s.herb.plantName.substr(0, 4);
            DrawText(name.c_str(), rect.x + 4, rect.y + 12, 10, RAYWHITE);
        } else {
            const auto& def = World::GetItemDefinition(s.station);
            std::string name = def.displayName.substr(0, 4);
            DrawText(name.c_str(), rect.x + 4, rect.y + 12, 10, RAYWHITE);
            DrawText(std::to_string(s.quantity).c_str(), rect.x + rect.width - 14, rect.y + rect.height - 14, 10, GOLD);
        }
    }
}

void InventoryUI::Draw(const World::Inventory& inventory) const {
    if (!m_open) return;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int gridW = GRID_COLS * SLOT_SIZE + (GRID_COLS - 1) * SLOT_PADDING;
    int gridH = (GRID_ROWS + 1) * SLOT_SIZE + GRID_ROWS * SLOT_PADDING + 20;
    int bgW = gridW + 40;
    int bgH = gridH + 80;
    int bgX = (sw - bgW) / 2;
    int bgY = (sh - bgH) / 2;

    DrawRectangle(bgX, bgY, bgW, bgH, Fade(Color{ 30, 25, 20, 255 }, 0.95f));
    DrawRectangleLinesEx(Rectangle{ (float)bgX, (float)bgY, (float)bgW, (float)bgH }, 2.0f, Color{ 140, 100, 70, 255 });
    DrawText("INVENTORY", bgX + 20, bgY + 20, 18, GOLD);

    // 1. Draw Grid
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            int idx = 10 + r * GRID_COLS + c;
            DrawInventorySlot(inventory.GetSlot(idx), GetGridSlotRect(r, c), false, idx == m_draggedSlot);
        }
    }

    // 2. Draw Hotbar
    DrawText("Hotbar", bgX + 20, bgY + bgH - 105, 11, Color{ 160, 200, 140, 255 });
    for (int i = 0; i < 10; ++i) {
        DrawInventorySlot(inventory.GetSlot(i), GetHotbarSlotRectInside(i), i == m_selectedSlot, i == m_draggedSlot);
        DrawText(std::to_string((i == 9) ? 0 : i + 1).c_str(), GetHotbarSlotRectInside(i).x + 4, GetHotbarSlotRectInside(i).y + 4, 9, GRAY);
    }

    // 3. Draw Dragged Item
    if (m_isDragging && m_draggedSlot != -1) {
        Vector2 m = GetMousePosition();
        Rectangle r = { m.x - SLOT_SIZE/2, m.y - SLOT_SIZE/2, (float)SLOT_SIZE, (float)SLOT_SIZE };
        DrawInventorySlot(inventory.GetSlot(m_draggedSlot), r, false, false);
    }

    DrawText("Drag items to reorder. Click station to place.", bgX + 20, bgY + bgH - 24, 10, GRAY);
}

void InventoryUI::SetSelectedSlot(int slot) {
    if (slot >= 0 && slot < 10) m_selectedSlot = slot;
}

void InventoryUI::DrawHotbar(const World::Inventory& inventory) const {
    const int slotSize = 60;
    const int spacing = 8;
    const int totalWidth = 10 * slotSize + 9 * spacing;
    const int startX = (GetScreenWidth() - totalWidth) / 2;
    const int startY = GetScreenHeight() - slotSize - 20;

    int panelX = startX - 8, panelY = startY - 8, panelW = totalWidth + 16, panelH = slotSize + 16;
    DrawRectangleRec(Rectangle{ (float)panelX, (float)panelY, (float)panelW, (float)panelH }, Fade(BLACK, 0.6f));
    DrawRectangleLinesEx(Rectangle{ (float)panelX, (float)panelY, (float)panelW, (float)panelH }, 1.5f, Color{ 140, 100, 70, 255 });

    for (int i = 0; i < 10; ++i) {
        Rectangle r = { (float)(startX + i * (slotSize + spacing)), (float)startY, (float)slotSize, (float)slotSize };
        const auto& s = inventory.GetSlot(i);
        
        DrawRectangleRec(r, Fade(Color{ 45, 55, 40, 255 }, 0.5f));
        if (i == m_selectedSlot) {
            DrawRectangleLinesEx(r, 2.5f, GOLD);
            DrawRectangleRec(r, Fade(GOLD, 0.12f));
        } else {
            DrawRectangleLinesEx(r, 1.0f, Color{ 100, 80, 60, 200 });
        }

        std::string numStr = (i == 9) ? "0" : std::to_string(i + 1);
        DrawText(numStr.c_str(), r.x + 4, r.y + 4, 10, (i == m_selectedSlot) ? GOLD : GRAY);

        if (s.occupied) {
            if (s.isHerb) {
                Color q = GRAY;
                if (s.herb.quality == World::HarvestQuality::Pristine) q = GOLD;
                else if (s.herb.quality == World::HarvestQuality::Standard) q = WHITE;
                DrawRectangle(r.x + 4, r.y + slotSize - 6, slotSize - 8, 3, q);
                
                std::string abbr = "?"; Color sc = WHITE;
                switch (s.herb.stage) {
                    case World::PlantStage::Fresh: abbr = "F"; sc = GREEN; break;
                    case World::PlantStage::Dried: abbr = "D"; sc = Color{ 210, 140, 40, 255 }; break;
                    case World::PlantStage::Ground: abbr = "G"; sc = Color{ 230, 200, 50, 255 }; break;
                    case World::PlantStage::Spent: abbr = "S"; sc = DARKGRAY; break;
                    case World::PlantStage::Spirits: abbr = "Sp"; sc = SKYBLUE; break;
                    case World::PlantStage::Salt: abbr = "Sa"; sc = LIGHTGRAY; break;
                    case World::PlantStage::Tincture: abbr = "T"; sc = PURPLE; break;
                }
                DrawText(abbr.c_str(), r.x + slotSize - 16, r.y + 4, 10, sc);
                std::string n = s.herb.plantName; if (n.length() > 7) n = n.substr(0, 6) + ".";
                DrawText(n.c_str(), r.x + 4, r.y + 24, 10, RAYWHITE);
            } else {
                const auto& def = World::GetItemDefinition(s.station);
                std::string name = def.displayName.substr(0, 7);
                DrawText(name.c_str(), r.x + 4, r.y + 24, 10, RAYWHITE);
                DrawText(std::to_string(s.quantity).c_str(), r.x + slotSize - 16, r.y + slotSize - 14, 10, GOLD);
            }
        }
    }
}

void InventoryUI::DrawDryingRackUI(World::Inventory& inventory) {
    if (!m_activeDryingRack) return;
    int sw = GetScreenWidth(), sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.5f));
    int menuW = 500, menuH = 180, menuX = (sw - menuW) / 2, menuY = (sh - menuH) / 2;
    DrawRectangle(menuX, menuY, menuW, menuH, Fade(Color{ 30, 25, 20, 255 }, 0.95f));
    DrawRectangleLinesEx(Rectangle{ (float)menuX, (float)menuY, (float)menuW, (float)menuH }, 2.0f, Color{ 140, 100, 70, 255 });
    DrawText("DRYING RACK", menuX + 16, menuY + 16, 16, GOLD);

    Rectangle closeRect = { (float)(menuX + menuW - 36), (float)(menuY + 12), 24, 24 };
    if (CheckCollisionPointRec(GetMousePosition(), closeRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { m_activeDryingRack = nullptr; return; }
    DrawRectangleRec(closeRect, Color{ 45, 40, 35, 255 }); DrawText("X", closeRect.x + 8, closeRect.y + 6, 12, RAYWHITE);

    auto& slots = m_activeDryingRack->GetDryingSlots();
    long long now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    for (int i = 0; i < 7; ++i) {
        Rectangle sr = { (float)(menuX + 40 + i * 62), (float)(menuY + 60), 52, 52 };
        bool h = CheckCollisionPointRec(GetMousePosition(), sr);
        DrawRectangleRec(sr, Fade(Color{ 45, 55, 40, 255 }, 0.5f));
        
        auto& slot = slots[i];
        if (slot.occupied) {
            float p = std::min(static_cast<float>(now - slot.processStartUtc) / slot.processDurationSec, 1.0f);
            bool ready = (p >= 1.0f);
            DrawRectangleLinesEx(sr, 1.5f, ready ? GREEN : GOLD);
            DrawText(slot.loadedItem.plantName.substr(0,5).c_str(), sr.x + 4, sr.y + 10, 9, RAYWHITE);
            if (ready) DrawText("Ready", sr.x + 4, sr.y + 32, 9, GREEN);
            else DrawRectangle(sr.x + 4, sr.y + 36, static_cast<int>((sr.width-8)*p), 4, GREEN);
            
            if (h && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                World::HarvestItem out = slot.loadedItem; if (ready) out.stage = World::PlantStage::Dried;
                inventory.AddHarvestItem(out); slot.occupied = false;
            }
        } else {
            DrawRectangleLinesEx(sr, 1.0f, h ? GOLD : GRAY);
            if (h && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                const auto& s = inventory.GetSlot(m_selectedSlot);
                if (s.occupied && s.isHerb && s.herb.stage == World::PlantStage::Fresh) {
                    slot.occupied = true; slot.loadedItem = s.herb; slot.processStartUtc = now; slot.processDurationSec = 7200;
                    inventory.ClearSlot(m_selectedSlot);
                }
            }
        }
    }
}

void InventoryUI::DrawStorageUI(World::Inventory& inventory) {
    if (!m_activeStorage) return;
    int sw = GetScreenWidth(), sh = GetScreenHeight();
    DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.5f));
    int menuW = 800, menuH = 550, menuX = (sw - menuW) / 2, menuY = (sh - menuH) / 2;
    DrawRectangle(menuX, menuY, menuW, menuH, Fade(Color{ 30, 25, 20, 255 }, 0.95f));
    DrawRectangleLinesEx(Rectangle{ (float)menuX, (float)menuY, (float)menuW, (float)menuH }, 2.0f, Color{ 140, 100, 70, 255 });
    DrawText("STORAGE CHEST", menuX + 24, menuY + 20, 18, GOLD);

    Rectangle closeRect = { (float)(menuX + menuW - 40), (float)(menuY + 16), 24, 24 };
    if (CheckCollisionPointRec(GetMousePosition(), closeRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { m_activeStorage = nullptr; return; }
    DrawRectangleRec(closeRect, Color{ 45, 40, 35, 255 }); DrawText("X", closeRect.x + 8, closeRect.y + 6, 12, RAYWHITE);

    auto& grid = m_activeStorage->GetStorageGrid();
    for (int y = 0; y < 12; ++y) {
        for (int x = 0; x < 12; ++x) {
            int idx = y * 12 + x;
            Rectangle sr = { (float)(menuX + 32 + x * 36), (float)(menuY + 64 + y * 36), 32, 32 };
            bool h = CheckCollisionPointRec(GetMousePosition(), sr);
            DrawRectangleRec(sr, Fade(Color{ 45, 55, 40, 255 }, 0.4f));
            DrawRectangleLinesEx(sr, 1.0f, h ? GOLD : Color{ 100, 80, 60, 150 });

            auto& slot = grid[idx];
            if (slot.occupied) {
                if (slot.isHarvest) {
                    DrawText("H", sr.x + 10, sr.y + 11, 10, GREEN);
                    if (h && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { inventory.AddHarvestItem(slot.harvestItem); slot.occupied = false; }
                } else {
                    DrawText("S", sr.x + 10, sr.y + 11, 10, RAYWHITE);
                    if (h && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { inventory.AddItem(slot.placeableType, 1); slot.occupied = false; }
                }
            } else {
                if (h && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    const auto& s = inventory.GetSlot(m_selectedSlot);
                    if (s.occupied) {
                        slot.occupied = true; slot.isHarvest = s.isHerb;
                        if (s.isHerb) slot.harvestItem = s.herb; else { slot.placeableType = s.station; }
                        inventory.ClearSlot(m_selectedSlot);
                    }
                }
            }
        }
    }
}

} // namespace UI
} // namespace FloraPhilosophica
