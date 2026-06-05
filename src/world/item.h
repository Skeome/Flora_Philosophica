#ifndef FLORA_PHILOSOPHIA_WORLD_ITEM_H
#define FLORA_PHILOSOPHIA_WORLD_ITEM_H

#include <string>

namespace FloraPhilosophia {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// ItemType
// Every placeable object in the game — lab stations, furniture, storage — is
// one of these types. The type determines how the item looks, what it does
// when inspected, and which tier unlocks it.
//
// Adding a new station: add an entry here, handle it in Apparatus and
// PlacedItem::GetDisplayName().
// ─────────────────────────────────────────────────────────────────────────────
enum class ItemType {
    // ── Tier 1 (inherited from the previous herbalist) ────────────────────
    Fireplace,          // Decoration that doubles as a calcination furnace
    MortarAndPestle,    // Grinding station for plant preparation
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

    // ── Harvested Plant Material ──────────────────────────────────────────
    // One entry per plant in the botanical database.
    // Quality (Pristine/Standard/Debased) is tracked separately in inventory.
    // These are consumed by laboratory apparatus, not placed on the grid.
    Plant_StJohnsWort,
    Plant_Mugwort,
    Plant_Nettle,
    Plant_Yarrow,
    Plant_Lavender,
    Plant_Dandelion,
    Plant_Comfrey,

    // Sentinel value — keep last
    COUNT
};

// ─────────────────────────────────────────────────────────────────────────────
// EquipmentTier
// Which progression tier unlocks this item for purchase.
// Tier 1 items are inherited and never need to be bought.
// ─────────────────────────────────────────────────────────────────────────────
enum class EquipmentTier {
    Tier1_Forager    = 1,
    Tier2_Herbalist  = 2,
    Tier3_Paracelsian = 3,
    Tier4_Adept      = 4
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
} // namespace FloraPhilosophia

#endif // FLORA_PHILOSOPHIA_WORLD_ITEM_H
