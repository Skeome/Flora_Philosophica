#ifndef FLORA_PHILOSOPHICA_WORLD_ROOM_MANAGER_H
#define FLORA_PHILOSOPHICA_WORLD_ROOM_MANAGER_H

#include "map.h"
#include "placed_item.h"
#include "inventory.h"
#include "raylib.h"
#include <vector>
#include <memory>
#include <string>

namespace FloraPhilosophica {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// RoomID
// Every distinct map in the game has an ID.
// Add new rooms here as they are designed.
// ─────────────────────────────────────────────────────────────────────────────
enum class RoomID {
    Exterior,       // The mountain clearing, foraging map, approach to cabin
    CabinMain,      // Ground floor of the cabin — lab, fireplace, workbench
    CabinLoft,      // Upper floor — future expansion (sleeping area, study)
    Garden,         // Cultivated outdoor plot attached to the cabin grounds
    COUNT
};

// ─────────────────────────────────────────────────────────────────────────────
// RoomTransition
// A tile-aligned trigger zone in a room. When the player walks over it,
// they transition to targetRoom and spawn at targetEntryPoint.
// ─────────────────────────────────────────────────────────────────────────────
struct RoomTransition {
    Rectangle  triggerRect;     // World-space area that triggers the transition
    RoomID     targetRoom;
    Vector2    targetEntryPoint;// Where the player appears in the target room
    std::string label;          // e.g. "Enter Cabin", shown as an interaction hint
};

// ─────────────────────────────────────────────────────────────────────────────
// PlacementMode
// State for the Stardew-style item placement UI.
// When active, the player is holding a ghost of an item and choosing where
// to place it. Pressing confirm places it; pressing cancel returns it to
// inventory.
// ─────────────────────────────────────────────────────────────────────────────
struct PlacementMode {
    bool     active = false;
    ItemType heldItem = ItemType::Fireplace;  // What we're placing
    int      ghostTileX = 0;                 // Current cursor tile position
    int      ghostTileY = 0;
    bool     placementValid = false;         // Can we place here?
};

// ─────────────────────────────────────────────────────────────────────────────
// RoomManager
// Owns all room TileMaps and the placed items within each room.
// Handles:
//   - Room transitions (walking through doorways)
//   - Item placement (Stardew-style grid placement)
//   - Item interaction dispatch (E key near a placed item)
//   - Serialisation of all placed item positions for the save system
// ─────────────────────────────────────────────────────────────────────────────
class RoomManager {
public:
    RoomManager(int tileSize);

    // Build all room tilemaps and populate the default cabin layout
    void Initialize();

    // Update the active room and handle transition triggers
    void Update(float deltaTime, Vector2 playerPos);

    // Draw the active room (tiles + placed items)
    void Draw() const;

    // Draw placement mode ghost item overlay (called after Draw)
    void DrawPlacementGhost(Vector2 mouseWorldPos) const;

    // Attempt to interact with the nearest placed item in the active room.
    // Returns the result so main.cpp can dispatch to the correct UI.
    // If the result is InspectDecoration, outMessage is populated with
    // the item's flavour text.
    InteractionResult TryInteract(Vector2 playerPos, std::string& outMessage);

    // ── Placement mode ────────────────────────────────────────────────────
    // Begin placing an item from inventory
    void StartPlacement(ItemType type);

    // Update ghost tile position from mouse/touch world position
    void UpdatePlacementGhost(Vector2 mouseWorldPos);

    // Confirm placement at the current ghost position.
    // Removes the item from inventory and adds it as a PlacedItem.
    // Returns false if the placement is invalid.
    bool ConfirmPlacement(Inventory& inventory);

    // Cancel placement and return the item to inventory
    void CancelPlacement(Inventory& inventory);

    bool IsInPlacementMode() const { return m_placement.active; }

    // ── Room access ───────────────────────────────────────────────────────
    TileMap&       GetActiveMap()       { return *m_rooms[static_cast<int>(m_activeRoom)]; }
    const TileMap& GetActiveMap() const { return *m_rooms[static_cast<int>(m_activeRoom)]; }
    RoomID         GetActiveRoomID()    const { return m_activeRoom; }

    // Immediately switch to a new room and place the player at entryPoint.
    // Called by main.cpp when GetPendingTransition() returns non-null.
    void TransitionTo(RoomID target, Vector2 playerEntryPoint);

    // Returns the pending transition if the player just walked into one,
    // or nullptr if no transition is pending this frame.
    const RoomTransition* GetPendingTransition() const { return m_pendingTransition; }
    void ClearPendingTransition() { m_pendingTransition = nullptr; }

    // ── Save/load ─────────────────────────────────────────────────────────
    std::string Serialise() const;
    void        Deserialise(const std::string& json, int tileSize);

private:
    // One TileMap per room
    std::unique_ptr<TileMap> m_rooms[static_cast<int>(RoomID::COUNT)];

    // Placed items per room — indexed by RoomID
    std::vector<PlacedItem> m_placedItems[static_cast<int>(RoomID::COUNT)];

    // Transition zones per room
    std::vector<RoomTransition> m_transitions[static_cast<int>(RoomID::COUNT)];

    RoomID m_activeRoom;
    const RoomTransition* m_pendingTransition;
    PlacementMode m_placement;
    int m_tileSize;

    // Builds the default Tier 1 cabin layout with inherited items
    void SetupDefaultCabinLayout();

    // Adds a transition zone to a room
    void AddTransition(RoomID room, RoomTransition transition);

    // Returns true if the given tile position is a valid placement location
    // for the given item type in the given room
    bool IsValidPlacement(RoomID room, ItemType type, int tileX, int tileY) const;

    // Returns true if any placed item in the room overlaps the given tile rect
    bool TilesOccupied(RoomID room, int tileX, int tileY, int w, int h) const;
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_ROOM_MANAGER_H
