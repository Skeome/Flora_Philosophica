#ifndef FLORA_PHILOSOPHIA_WORLD_PLACED_ITEM_H
#define FLORA_PHILOSOPHIA_WORLD_PLACED_ITEM_H

#include "item.h"
#include "raylib.h"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

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
// Holds its grid position, type, and any instance-specific state.
// ─────────────────────────────────────────────────────────────────────────────
class PlacedItem {
public:
    struct DryingSlot {
        bool occupied = false;
        HarvestItem loadedItem = {};
        long long processStartUtc = 0;
        long long processDurationSec = 0;
    };

    struct StorageSlot {
        bool occupied = false;
        bool isHarvest = false;
        ItemType placeableType = ItemType::COUNT;
        HarvestItem harvestItem = {};
    };

    PlacedItem(ItemType type, int tileX, int tileY, int tileSize);

    // Draw the item at its grid position
    // TODO: Replace colored rectangles with sprites when art assets are ready
    void Draw() const;

    // Update per-frame logic (timer display, animation states)
    void Update(float deltaTime);

    // ── Apparatus / timed process interface ──────────────────────────────
    // Load an item into this apparatus. Returns false if the station is
    // already occupied or the item type is not accepted.
    bool LoadHarvestItem(const HarvestItem& item, long long nowUtc);

    // Returns true if the loaded process has completed
    bool IsProcessComplete(long long nowUtc) const;

    // Unloads and returns the processed item. Call only when IsProcessComplete().
    // Returns false if nothing is loaded or process not complete.
    bool UnloadProcessedItem(HarvestItem& outItem, long long nowUtc);

    // Returns progress 0.0–1.0, or -1 if nothing loaded
    float GetProgress(long long nowUtc) const;

    // Returns the display name of the currently loaded item, or empty string
    std::string GetLoadedItemName() const;

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

    std::vector<DryingSlot>& GetDryingSlots() { return m_dryingSlots; }
    const std::vector<DryingSlot>& GetDryingSlots() const { return m_dryingSlots; }

    std::vector<StorageSlot>& GetStorageGrid() { return m_storageGrid; }
    const std::vector<StorageSlot>& GetStorageGrid() const { return m_storageGrid; }

    // Setters
    void SetDiscovered(bool discovered) { m_discovered = discovered; }

    // Returns the world-space rectangle occupied by this item
    Rectangle GetWorldRect() const;

    // Serialise instance state to JSON string
    std::string Serialise() const;
    void Deserialise(const nlohmann::json& j);

private:
    ItemType m_type;
    int      m_tileX;       // Grid column (left edge of footprint)
    int      m_tileY;       // Grid row (top edge of footprint)
    int      m_tileSize;    // Pixels per tile — needed to convert to world coords

    // m_discovered tracks whether the player has inspected this item and
    // learned its true function.
    bool m_discovered;

    // ── Apparatus state ───────────────────────────────────────────────────
    // Used by timed stations (DryingRack, MacerationJar, etc.)
    bool        m_occupied;          // True if a process is running
    HarvestItem m_loadedItem;        // The item currently being processed
    long long   m_processStartUtc;   // UTC timestamp when the process began
    long long   m_processDurationSec;// Total process duration in seconds
    float       m_cachedProgress;    // 0.0-1.0, updated each frame for Draw()

    // ── Multi-slot state ──────────────────────────────────────────────────
    std::vector<DryingSlot>  m_dryingSlots;  // Size 7 for DryingRack
    std::vector<StorageSlot> m_storageGrid;  // Size 144 (12x12) for StorageChest

    // Duration constants (seconds)
    // 1 real hour = 1 in-game week
    static constexpr long long DRYING_DURATION_SEC     = 2 * 3600LL; // 2 hours
    static constexpr long long GRINDING_DURATION_SEC   = 0LL;         // instant
    static constexpr long long MACERATION_DURATION_SEC = 1 * 3600LL; // 1 hour

    // Interaction radius in world pixels
    static constexpr float INTERACTION_RADIUS = 36.0f;
};

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHIA_WORLD_PLACED_ITEM_H
