#ifndef FLORA_PHILOSOPHICA_WORLD_PLACED_ITEM_H
#define FLORA_PHILOSOPHICA_WORLD_PLACED_ITEM_H

#include "item.h"
#include "raylib.h"
#include <string>

namespace FloraPhilosophica {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// InteractionResult
// What happens when the player presses E near a placed item.
// The game loop reads this and dispatches to the correct subsystem.
// ─────────────────────────────────────────────────────────────────────────────
enum class InteractionResult {
    None,               // Nothing happened (not close enough, or item has no action)
    OpenApparatus,      // Open the apparatus UI for this station (lab work)
    OpenStorage,        // Open the storage chest UI
    OpenCompost,        // Open the compost confirmation dialog
    InspectDecoration,  // Show the flavour-text description (fireplace "discovery" moment)
    OpenMailbox,        // Open the mailbox to read letters
    OpenPlacementMenu,  // Not used here — triggered by inventory UI instead
};

// ─────────────────────────────────────────────────────────────────────────────
// PlacedItem
// An instance of an item that has been placed on a room's tile grid.
// Holds its grid position, type, and any instance-specific state
// (e.g. whether the fireplace has been "discovered" as a furnace yet).
// ─────────────────────────────────────────────────────────────────────────────
class PlacedItem {
public:
    PlacedItem(ItemType type, int tileX, int tileY, int tileSize);

    // Draw the item at its grid position
    // TODO: Replace colored rectangles with sprites when art assets are ready
    void Draw() const;

    // Update per-frame logic (e.g. animated states, timer display)
    void Update(float deltaTime);

    // Returns true if the player position is within interaction range
    bool IsPlayerNear(Vector2 playerWorldPos) const;

    // Called when the player presses E near this item
    // Returns what action the game loop should take
    InteractionResult Interact();

    // Returns the flavour text shown when an item is inspected.
    // Only meaningful when Interact() returns InspectDecoration.
    std::string GetInspectionMessage() const;

    // Getters
    ItemType GetType()  const { return m_type; }
    int      GetTileX() const { return m_tileX; }
    int      GetTileY() const { return m_tileY; }
    bool     IsDiscovered() const { return m_discovered; }

    // Setters
    void SetDiscovered(bool discovered) { m_discovered = discovered; }

    // Returns the world-space rectangle occupied by this item
    Rectangle GetWorldRect() const;

    // Serialise instance state to JSON string (position + discovered flag)
    std::string Serialise() const;

private:
    ItemType m_type;
    int      m_tileX;       // Grid column (left edge of footprint)
    int      m_tileY;       // Grid row (top edge of footprint)
    int      m_tileSize;    // Pixels per tile — needed to convert to world coords

    // m_discovered tracks whether the player has inspected this item and
    // learned its true function. Used for the fireplace "it's a furnace!" moment
    // and any other items that have a hidden secondary purpose.
    bool m_discovered;

    // Interaction radius in world pixels — player must be within this to interact
    static constexpr float INTERACTION_RADIUS = 60.0f;
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_PLACED_ITEM_H
