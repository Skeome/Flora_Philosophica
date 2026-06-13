#ifndef FLORA_PHILOSOPHICA_WORLD_ITEM_H
#define FLORA_PHILOSOPHICA_WORLD_ITEM_H

#include <string>

namespace FloraPhilosophica {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// PlantStage
// The processing stage a piece of plant material is currently in.
// Material moves through these stages as the player processes it.
// Future stages (Root, Leaf, Flower, Seed) will be added when plant part
// separation is implemented.
// ─────────────────────────────────────────────────────────────────────────────
enum class PlantStage {
    Fresh,      // Just harvested — must be dried before further processing
    Dried,      // Dried on the rack — ready for grinding or direct maceration
    Ground,     // Dried and ground in mortar — better extraction than whole dried
    Spent,      // Post-maceration residue — goes to furnace (calcination) or compost
    Spirits,    // Ethanol distilled/extracted from herb
    Salt,       // Potassium Carbonate (Salt of Tartar/Ash) from calcination
    Tincture,   // Basic Tincture from maceration
};

// ─────────────────────────────────────────────────────────────────────────────
// HarvestQuality
// Moved here from plant_node.h so inventory.h can include it without
// pulling in the full plant node system.
// ─────────────────────────────────────────────────────────────────────────────
enum class HarvestQuality {
    Pristine,   // Harvested during matching planetary day AND hour
    Standard,   // Harvested during standard times
    Debased,    // Harvested during the opposite planetary hour
};

// ─────────────────────────────────────────────────────────────────────────────
// HarvestItem
// A piece of plant material in the player's inventory.
// Carries the plant's name, its current processing stage, and the quality
// at which it was originally harvested. Quality persists through processing.
//
// This is separate from ItemType because plant material is not placeable
// on the grid and has too many variants (plant × stage × quality) to enumerate
// as flat enum values.
// ─────────────────────────────────────────────────────────────────────────────
struct HarvestItem {
    std::string    plantName;   // e.g. "St. John's Wort" — keys into PlantDatabase
    PlantStage     stage;       // Current processing stage
    HarvestQuality quality;     // Quality from original harvest — never changes

    // Convenience: returns a display string like "Dried St. John's Wort (Pristine)"
    std::string GetDisplayName() const;

    // Returns the stage name as a short string for display
    static std::string GetStageName(PlantStage stage);
};

// ─────────────────────────────────────────────────────────────────────────────
// ItemType
// Every placeable object in the game — lab stations, furniture, storage — is
// one of these types. The type determines how the item looks, what it does
// when inspected, and which tier unlocks it.
//
// Adding a new station: add an entry here, handle it in PlacedItem and
// item.cpp's G_ITEM_DEFINITIONS table.
// ─────────────────────────────────────────────────────────────────────────────
enum class ItemType {
    // ── Tier 1 (inherited from the previous herbalist) ────────────────────
    Fireplace,          // Decoration that doubles as a calcination furnace
    DryingRack,         // Hangs harvested herbs to dry (2 real hours)
    MortarAndPestle,    // Grinding station — converts Dried → Ground plant
    MacerationJar,      // Mason jar for alcohol maceration (basic tincture)
    CompostBin,         // Irretrievable disposal — the "trap" mechanic
    WorkBench,          // General preparation surface

    // ── Tier 2 ───────────────────────────────────────────────────────────
    CopperAlembic,      // Distillation apparatus for simple extractions
    GlassFlask,         // Borosilicate vessel for heating and storage
    GlassblowingStation,// Mini-game station: blow replacement vessels

    // ── Tier 3 ───────────────────────────────────────────────────────────
    DistillationTrain,  // Multi-stage distillation setup
    SoxhletExtractor,   // Continuous solvent extraction apparatus

    // ── Tier 4 ───────────────────────────────────────────────────────────
    PelicanFlask,       // Cohobation vessel for Plant Stone creation
    RetortTrain,        // Full retort apparatus for advanced distillation
    Terrarium,          // Passive idle plant grower

    // ── Decoration / Furniture ───────────────────────────────────────────
    Bookshelf,
    StorageChest,
    MailboxPost,        // Placed outside; where Ternary Order letters arrive

    // Sentinel value — keep last
    COUNT
};

// ─────────────────────────────────────────────────────────────────────────────
// EquipmentTier
// Which progression tier unlocks this item for purchase.
// Tier 1 items are inherited and never need to be bought.
// ─────────────────────────────────────────────────────────────────────────────
enum class EquipmentTier {
    Tier1_Forager     = 1,
    Tier2_Herbalist   = 2,
    Tier3_Paracelsian = 3,
    Tier4_Adept       = 4
};

// ─────────────────────────────────────────────────────────────────────────────
// ItemDefinition
// Static data about an item type — its name, tier, grid footprint, and
// whether it can be placed indoors, outdoors, or both.
// ─────────────────────────────────────────────────────────────────────────────
struct ItemDefinition {
    ItemType      type;
    std::string   displayName;
    std::string   description;
    EquipmentTier tier;
    int           tileWidth;    // Footprint width in tiles
    int           tileHeight;   // Footprint height in tiles
    bool          canPlaceIndoors;
    bool          canPlaceOutdoors;
};

// ─────────────────────────────────────────────────────────────────────────────
// GetItemDefinition
// Returns static metadata for a given item type.
// ─────────────────────────────────────────────────────────────────────────────
const ItemDefinition& GetItemDefinition(ItemType type);

} // namespace World
} // namespace FloraPhilosophica

#endif // FLORA_PHILOSOPHICA_WORLD_ITEM_H
