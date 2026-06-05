#include "room_manager.h"
#include "raymath.h"
#include "nlohmann/json.hpp"
#include <cmath>
#include <stdexcept>

namespace FloraPhilosophia {
namespace World {

RoomManager::RoomManager(int tileSize)
    : m_activeRoom(RoomID::Exterior)
    , m_pendingTransition(nullptr)
    , m_tileSize(tileSize)
{}

// ─────────────────────────────────────────────────────────────────────────────
// Initialize
// Creates all room TileMaps, populates the default cabin layout, and sets up
// the transition zones between rooms.
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::Initialize() {
    // ── Create room TileMaps ──────────────────────────────────────────────────
    // Exterior: 20x15 tiles (matches current prototype map)
    m_rooms[static_cast<int>(RoomID::Exterior)] =
        std::make_unique<TileMap>(20, 15, m_tileSize);
    m_rooms[static_cast<int>(RoomID::Exterior)]->Initialize();

    // CabinMain: 12x10 tiles — cozy interior space
    m_rooms[static_cast<int>(RoomID::CabinMain)] =
        std::make_unique<TileMap>(12, 10, m_tileSize);
    m_rooms[static_cast<int>(RoomID::CabinMain)]->Initialize();

    // CabinLoft: 10x6 tiles — smaller upper floor (future expansion)
    m_rooms[static_cast<int>(RoomID::CabinLoft)] =
        std::make_unique<TileMap>(10, 6, m_tileSize);
    m_rooms[static_cast<int>(RoomID::CabinLoft)]->Initialize();

    // Garden: 15x10 tiles — outdoor cultivated plot
    m_rooms[static_cast<int>(RoomID::Garden)] =
        std::make_unique<TileMap>(15, 10, m_tileSize);
    m_rooms[static_cast<int>(RoomID::Garden)]->Initialize();

    // ── Transition zones ─────────────────────────────────────────────────────
    // Exterior → CabinMain: the cabin front door gap is at x:600-720, y:30-180.
    // The path approaches from below (y increases downward in screen space).
    // Trigger sits at the bottom of the door gap — player walks north into it.
    AddTransition(RoomID::Exterior, {
        Rectangle{ 604, 148, 112, 36 },         // Bottom of door gap, just inside the walls
        RoomID::CabinMain,
        Vector2{ 360, 480 },                    // Spawn near bottom of cabin interior
        "Enter Cabin"
    });

    // CabinMain → Exterior: doorway back out
    AddTransition(RoomID::CabinMain, {
        Rectangle{ 300, 480, 120, 60 },         // Bottom edge of cabin map
        RoomID::Exterior,
        Vector2{ 600, 180 },                    // Spawn just outside the cabin door
        "Exit Cabin"
    });

    // ── Default layout ───────────────────────────────────────────────────────
    SetupDefaultCabinLayout();
}

// ─────────────────────────────────────────────────────────────────────────────
// SetupDefaultCabinLayout
// Places the Tier 1 items that the previous herbalist left behind.
// This is the state of the cabin when a new player begins.
// Players can move any of these items after the first session.
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::SetupDefaultCabinLayout() {
    int room = static_cast<int>(RoomID::CabinMain);

    // Fireplace — back wall, centre. Looks decorative until inspected.
    m_placedItems[room].emplace_back(ItemType::Fireplace, 5, 0, m_tileSize);

    // WorkBench — left side of the room
    m_placedItems[room].emplace_back(ItemType::WorkBench, 0, 3, m_tileSize);

    // MortarAndPestle — on or near the workbench
    m_placedItems[room].emplace_back(ItemType::MortarAndPestle, 0, 5, m_tileSize);

    // MacerationJar — right side, near the window
    m_placedItems[room].emplace_back(ItemType::MacerationJar, 9, 3, m_tileSize);

    // Bookshelf — back wall, right of fireplace
    m_placedItems[room].emplace_back(ItemType::Bookshelf, 8, 0, m_tileSize);

    // StorageChest — corner
    m_placedItems[room].emplace_back(ItemType::StorageChest, 0, 0, m_tileSize);

    // CompostBin — placed outside by default (Exterior room)
    // It's near the cabin door, easy to find early, encouraging the "trap"
    int exterior = static_cast<int>(RoomID::Exterior);
    m_placedItems[exterior].emplace_back(ItemType::CompostBin, 11, 6, m_tileSize);

    // Mailbox — outside near the path
    m_placedItems[exterior].emplace_back(ItemType::MailboxPost, 8, 8, m_tileSize);
}

void RoomManager::AddTransition(RoomID room, RoomTransition transition) {
    m_transitions[static_cast<int>(room)].push_back(transition);
}

// ─────────────────────────────────────────────────────────────────────────────
// TransitionTo
// Switches the active room. main.cpp reads the entry point from the
// RoomTransition struct and teleports the player after calling this.
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::TransitionTo(RoomID target, Vector2 playerEntryPoint) {
    m_activeRoom = target;
    m_pendingTransition = nullptr;
    (void)playerEntryPoint; // entry point is applied by main.cpp, not here
}

// ─────────────────────────────────────────────────────────────────────────────
// Update
// Checks if the player has walked into a transition zone and flags it.
// Also updates all placed items in the active room.
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::Update(float deltaTime, Vector2 playerPos) {
    int roomIdx = static_cast<int>(m_activeRoom);

    // Update placed items
    for (auto& item : m_placedItems[roomIdx]) {
        item.Update(deltaTime);
    }

    // Check transition triggers
    m_pendingTransition = nullptr;
    for (const auto& transition : m_transitions[roomIdx]) {
        if (CheckCollisionPointRec(playerPos, transition.triggerRect)) {
            m_pendingTransition = &transition;
            break;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw
// Draws the active room's tile map and all placed items within it.
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::Draw() const {
    int roomIdx = static_cast<int>(m_activeRoom);

    // Draw the base tile map
    m_rooms[roomIdx]->Draw();

    // Draw transition zone hints
    for (const auto& transition : m_transitions[roomIdx]) {
        DrawRectangleLinesEx(transition.triggerRect, 2.0f, Fade(GOLD, 0.5f));
        DrawText(
            transition.label.c_str(),
            static_cast<int>(transition.triggerRect.x + 4),
            static_cast<int>(transition.triggerRect.y - 14),
            10, GOLD
        );
    }

    // Draw placed items
    for (const auto& item : m_placedItems[roomIdx]) {
        item.Draw();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// DrawPlacementGhost
// Draws a semi-transparent ghost of the item being placed at the current
// cursor tile. Green = valid placement, red = invalid.
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::DrawPlacementGhost(Vector2 mouseWorldPos) const {
    if (!m_placement.active) return;

    const ItemDefinition& def = GetItemDefinition(m_placement.heldItem);

    Rectangle ghostRect = {
        static_cast<float>(m_placement.ghostTileX * m_tileSize),
        static_cast<float>(m_placement.ghostTileY * m_tileSize),
        static_cast<float>(def.tileWidth  * m_tileSize),
        static_cast<float>(def.tileHeight * m_tileSize)
    };

    Color ghostColor = m_placement.placementValid
        ? Color{ 80, 200, 80, 120 }    // green = valid
        : Color{ 200, 60, 60, 120 };   // red = invalid

    DrawRectangleRec(ghostRect, ghostColor);
    DrawRectangleLinesEx(ghostRect, 2.0f, m_placement.placementValid ? GREEN : RED);
    DrawText(
        def.displayName.c_str(),
        static_cast<int>(ghostRect.x + 4),
        static_cast<int>(ghostRect.y + ghostRect.height / 2 - 6),
        10, RAYWHITE
    );
}

// ─────────────────────────────────────────────────────────────────────────────
// TryInteract
// Finds the nearest placed item the player can reach and calls Interact() on it.
// Returns None if nothing is in range.
// ─────────────────────────────────────────────────────────────────────────────
InteractionResult RoomManager::TryInteract(Vector2 playerPos) {
    int roomIdx = static_cast<int>(m_activeRoom);
    for (auto& item : m_placedItems[roomIdx]) {
        if (item.IsPlayerNear(playerPos)) {
            return item.Interact();
        }
    }
    return InteractionResult::None;
}

// ─────────────────────────────────────────────────────────────────────────────
// Placement mode methods
// ─────────────────────────────────────────────────────────────────────────────
void RoomManager::StartPlacement(ItemType type) {
    m_placement.active    = true;
    m_placement.heldItem  = type;
    m_placement.ghostTileX = 0;
    m_placement.ghostTileY = 0;
    m_placement.placementValid = false;
}

void RoomManager::UpdatePlacementGhost(Vector2 mouseWorldPos) {
    if (!m_placement.active) return;

    // Convert world position to tile coordinates
    m_placement.ghostTileX = static_cast<int>(mouseWorldPos.x / m_tileSize);
    m_placement.ghostTileY = static_cast<int>(mouseWorldPos.y / m_tileSize);

    m_placement.placementValid = IsValidPlacement(
        m_activeRoom,
        m_placement.heldItem,
        m_placement.ghostTileX,
        m_placement.ghostTileY
    );
}

bool RoomManager::ConfirmPlacement(Inventory& inventory) {
    if (!m_placement.active || !m_placement.placementValid) return false;

    // Remove from inventory
    if (!inventory.RemoveItem(m_placement.heldItem)) return false;

    // Add to room as a placed item
    int roomIdx = static_cast<int>(m_activeRoom);
    m_placedItems[roomIdx].emplace_back(
        m_placement.heldItem,
        m_placement.ghostTileX,
        m_placement.ghostTileY,
        m_tileSize
    );

    m_placement.active = false;
    return true;
}

void RoomManager::CancelPlacement(Inventory& inventory) {
    if (!m_placement.active) return;
    // Item stays in inventory — nothing to return since we didn't remove it yet
    m_placement.active = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// IsValidPlacement
// Returns true if an item can be placed at the given tile position.
// Checks: within map bounds, indoor/outdoor rule, no overlapping items.
// ─────────────────────────────────────────────────────────────────────────────
bool RoomManager::IsValidPlacement(RoomID room, ItemType type, int tileX, int tileY) const {
    const ItemDefinition& def = GetItemDefinition(type);
    int roomIdx = static_cast<int>(room);

    // Check indoor/outdoor rules
    bool isIndoor = (room == RoomID::CabinMain || room == RoomID::CabinLoft);
    if (isIndoor && !def.canPlaceIndoors)  return false;
    if (!isIndoor && !def.canPlaceOutdoors) return false;

    // Check map bounds
    const TileMap& map = *m_rooms[roomIdx];
    int mapTilesW = map.GetWidth()  / m_tileSize;
    int mapTilesH = map.GetHeight() / m_tileSize;
    if (tileX < 0 || tileY < 0) return false;
    if (tileX + def.tileWidth  > mapTilesW) return false;
    if (tileY + def.tileHeight > mapTilesH) return false;

    // Check that all covered tiles are walkable
    for (int dy = 0; dy < def.tileHeight; ++dy) {
        for (int dx = 0; dx < def.tileWidth; ++dx) {
            if (!map.IsWalkable(tileX + dx, tileY + dy)) return false;
        }
    }

    // Check no existing item occupies these tiles
    if (TilesOccupied(room, tileX, tileY, def.tileWidth, def.tileHeight)) return false;

    return true;
}

bool RoomManager::TilesOccupied(RoomID room, int tileX, int tileY, int w, int h) const {
    int roomIdx = static_cast<int>(room);
    for (const auto& existing : m_placedItems[roomIdx]) {
        const ItemDefinition& existDef = GetItemDefinition(existing.GetType());

        // Check if the footprints overlap
        bool overlapX = tileX < existing.GetTileX() + existDef.tileWidth
                     && tileX + w > existing.GetTileX();
        bool overlapY = tileY < existing.GetTileY() + existDef.tileHeight
                     && tileY + h > existing.GetTileY();

        if (overlapX && overlapY) return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// Serialise / Deserialise
// Saves and restores the position of every placed item in every room.
// The active room is also saved so the player returns to the right room.
// ─────────────────────────────────────────────────────────────────────────────
std::string RoomManager::Serialise() const {
    nlohmann::json j;
    j["activeRoom"] = static_cast<int>(m_activeRoom);

    nlohmann::json rooms = nlohmann::json::array();
    for (int r = 0; r < static_cast<int>(RoomID::COUNT); ++r) {
        nlohmann::json roomItems = nlohmann::json::array();
        for (const auto& item : m_placedItems[r]) {
            roomItems.push_back(nlohmann::json::parse(item.Serialise()));
        }
        rooms.push_back(roomItems);
    }
    j["rooms"] = rooms;
    return j.dump(2); // pretty-print with 2-space indent
}

void RoomManager::Deserialise(const std::string& jsonStr, int tileSize) {
    // Clear all placed items
    for (int r = 0; r < static_cast<int>(RoomID::COUNT); ++r) {
        m_placedItems[r].clear();
    }

    try {
        auto j = nlohmann::json::parse(jsonStr);

        m_activeRoom = static_cast<RoomID>(j.at("activeRoom").get<int>());

        auto& rooms = j.at("rooms");
        for (int r = 0; r < static_cast<int>(RoomID::COUNT) && r < (int)rooms.size(); ++r) {
            for (const auto& itemJson : rooms[r]) {
                int typeInt    = itemJson.at("type").get<int>();
                int tx         = itemJson.at("tileX").get<int>();
                int ty         = itemJson.at("tileY").get<int>();
                bool discovered = itemJson.value("discovered", false);

                if (typeInt >= 0 && typeInt < static_cast<int>(ItemType::COUNT)) {
                    PlacedItem item(static_cast<ItemType>(typeInt), tx, ty, tileSize);
                    item.SetDiscovered(discovered);
                    m_placedItems[r].push_back(std::move(item));
                }
            }
        }
    } catch (const nlohmann::json::exception&) {
        // Corrupt save: leave rooms in cleared state, SetupDefaultCabinLayout
        // will be called separately if needed
    }
}

} // namespace World
} // namespace FloraPhilosophia
