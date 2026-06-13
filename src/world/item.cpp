#include "item.h"
#include <array>
#include <stdexcept>

namespace FloraPhilosophica {
namespace World {

// ─────────────────────────────────────────────────────────────────────────────
// HarvestItem helpers
// ─────────────────────────────────────────────────────────────────────────────
std::string HarvestItem::GetStageName(PlantStage stage) {
    switch (stage) {
        case PlantStage::Fresh:    return "Fresh";
        case PlantStage::Dried:    return "Dried";
        case PlantStage::Ground:   return "Ground";
        case PlantStage::Spent:    return "Spent";
        case PlantStage::Spirits:  return "Spirits";
        case PlantStage::Salt:     return "Salt";
        case PlantStage::Tincture: return "Tincture";
    }
    return "Unknown";
}

std::string HarvestItem::GetDisplayName() const {
    std::string qualStr;
    switch (quality) {
        case HarvestQuality::Pristine: qualStr = "Pristine"; break;
        case HarvestQuality::Standard: qualStr = "Standard"; break;
        case HarvestQuality::Debased:  qualStr = "Debased";  break;
    }
    
    std::string name;
    if (stage == PlantStage::Spirits) {
        name = "Spirits of " + plantName;
    } else if (stage == PlantStage::Salt) {
        name = "Salt of " + plantName;
    } else if (stage == PlantStage::Tincture) {
        name = "Tincture of " + plantName;
    } else {
        name = GetStageName(stage) + " " + plantName;
    }
    
    return name + " (" + qualStr + ")";
}

namespace {
    // ─────────────────────────────────────────────────────────────────────────
    // G_ITEM_DEFINITIONS
    // Static lookup table for every placeable item type.
    // Order must match the ItemType enum exactly.
    // Fields: type, displayName, description, tier, tileW, tileH, indoor, outdoor
    // ─────────────────────────────────────────────────────────────────────────
    const std::array<ItemDefinition, static_cast<size_t>(ItemType::COUNT)> G_ITEM_DEFINITIONS = {{
        // ── Tier 1 ───────────────────────────────────────────────────────────
        {
            ItemType::Fireplace,
            "Fireplace",
            "A stone hearth that warms the cabin. Inspect it to discover its true purpose.",
            EquipmentTier::Tier1_Forager,
            2, 2,
            true, false
        },
        {
            ItemType::DryingRack,
            "Drying Rack",
            "A simple wooden frame for hanging freshly harvested herbs. "
            "Drying takes approximately 2 real hours. "
            "Dried herbs can be ground in the mortar or used directly in maceration.",
            EquipmentTier::Tier1_Forager,
            2, 1,
            true, true
        },
        {
            ItemType::MortarAndPestle,
            "Mortar & Pestle",
            "Stone grinding bowl. Breaks dried herbs into powder, increasing surface "
            "area for better extraction during maceration.",
            EquipmentTier::Tier1_Forager,
            1, 1,
            true, true
        },
        {
            ItemType::MacerationJar,
            "Maceration Jar",
            "A sealed mason jar for steeping herbs in alcohol. The start of every tincture.",
            EquipmentTier::Tier1_Forager,
            1, 1,
            true, true
        },
        {
            ItemType::CompostBin,
            "Compost Bin",
            "A wooden bin for organic waste. Generates fertiliser for the garden. "
            "Warning: anything placed inside cannot be retrieved.",
            EquipmentTier::Tier1_Forager,
            1, 1,
            false, true  // outdoor only — the trap is outside the lab
        },
        {
            ItemType::WorkBench,
            "Work Bench",
            "A sturdy wooden table for general preparation and sorting.",
            EquipmentTier::Tier1_Forager,
            2, 1,
            true, true
        },
        // ── Tier 2 ───────────────────────────────────────────────────────────
        {
            ItemType::CopperAlembic,
            "Copper Alembic",
            "A classic pot-still for distillation. Separates volatile spirits from plant matter.",
            EquipmentTier::Tier2_Herbalist,
            2, 2,
            true, false
        },
        {
            ItemType::GlassFlask,
            "Glass Flask",
            "Borosilicate glass vessel for heating, storing, and observing preparations.",
            EquipmentTier::Tier2_Herbalist,
            1, 1,
            true, false
        },
        {
            ItemType::GlassblowingStation,
            "Glassblowing Station",
            "Furnace and pipe for hand-blowing replacement vessels. "
            "Skill improves vessel quality and heat tolerance over time.",
            EquipmentTier::Tier2_Herbalist,
            2, 2,
            true, false
        },
        // ── Tier 3 ───────────────────────────────────────────────────────────
        {
            ItemType::DistillationTrain,
            "Distillation Train",
            "Multi-stage glass apparatus for producing refined distillates and magisteries.",
            EquipmentTier::Tier3_Paracelsian,
            3, 2,
            true, false
        },
        {
            ItemType::SoxhletExtractor,
            "Soxhlet Extractor",
            "Continuous-cycle solvent extractor for exhaustive plant extraction.",
            EquipmentTier::Tier3_Paracelsian,
            2, 2,
            true, false
        },
        // ── Tier 4 ───────────────────────────────────────────────────────────
        {
            ItemType::PelicanFlask,
            "Pelican Flask",
            "A self-feeding distillation vessel for cohobation. Required for Plant Stone creation.",
            EquipmentTier::Tier4_Adept,
            2, 2,
            true, false
        },
        {
            ItemType::RetortTrain,
            "Retort Train",
            "Full retort distillation apparatus for ens tinctures and advanced preparations.",
            EquipmentTier::Tier4_Adept,
            3, 2,
            true, false
        },
        {
            ItemType::Terrarium,
            "Enchanted Terrarium",
            "A sealed glass enclosure that slowly grows and auto-harvests a single plant species.",
            EquipmentTier::Tier4_Adept,
            2, 2,
            true, true
        },
        // ── Decoration / Furniture ───────────────────────────────────────────
        {
            ItemType::Bookshelf,
            "Bookshelf",
            "A shelf for herbals, alchemical texts, and the Plant Compendium.",
            EquipmentTier::Tier1_Forager,
            2, 1,
            true, false
        },
        {
            ItemType::StorageChest,
            "Storage Chest",
            "A lockable chest for storing harvested plants and processed materials.",
            EquipmentTier::Tier1_Forager,
            1, 1,
            true, true
        },
        {
            ItemType::MailboxPost,
            "Mailbox",
            "A post box outside the cabin. Letters from customers and the Ternary Order arrive here.",
            EquipmentTier::Tier1_Forager,
            1, 1,
            false, true  // outdoor only
        },
    }};
}

const ItemDefinition& GetItemDefinition(ItemType type) {
    size_t index = static_cast<size_t>(type);
    if (index >= static_cast<size_t>(ItemType::COUNT)) {
        throw std::out_of_range("ItemType index out of range in GetItemDefinition");
    }
    return G_ITEM_DEFINITIONS[index];
}

} // namespace World
} // namespace FloraPhilosophica
