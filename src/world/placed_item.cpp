#include "placed_item.h"
#include "item.h"
#include "raymath.h"
#include "nlohmann/json.hpp"
#include <cmath>
#include <chrono>
#include <algorithm>

namespace FloraPhilosophica {
namespace World {

PlacedItem::PlacedItem(ItemType type, int tileX, int tileY, int tileSize)
    : m_type(type)
    , m_tileX(tileX)
    , m_tileY(tileY)
    , m_tileSize(tileSize)
    , m_discovered(false)
    , m_occupied(false)
    , m_loadedItem{}
    , m_processStartUtc(0)
    , m_processDurationSec(0)
    , m_cachedProgress(0.0f)
{
    if (type == ItemType::DryingRack) {
        m_dryingSlots.resize(7);
    } else if (type == ItemType::StorageChest) {
        m_storageGrid.resize(144);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// GetWorldRect
// Converts the tile-grid position to a world-space rectangle.
// Uses the item's tile footprint from the definition table.
// ─────────────────────────────────────────────────────────────────────────────
Rectangle PlacedItem::GetWorldRect() const {
    const ItemDefinition& def = GetItemDefinition(m_type);
    return Rectangle{
        static_cast<float>(m_tileX * m_tileSize),
        static_cast<float>(m_tileY * m_tileSize),
        static_cast<float>(def.tileWidth  * m_tileSize),
        static_cast<float>(def.tileHeight * m_tileSize)
    };
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw
// Temporary colored-rectangle placeholder rendering.
// Color encodes the item's tier/function at a glance during development.
// Replace with sprite rendering once art assets are available.
// ─────────────────────────────────────────────────────────────────────────────
void PlacedItem::Draw() const {
    Rectangle rect = GetWorldRect();
    const ItemDefinition& def = GetItemDefinition(m_type);

    // Choose a development placeholder color by item type
    Color fillColor;
    switch (m_type) {
        case ItemType::Fireplace:
            // Undiscovered: grey stone. Discovered: warm orange glow.
            fillColor = m_discovered
                ? Color{ 200, 100, 30, 255 }   // orange — furnace revealed
                : Color{ 120, 110, 100, 255 };  // stone grey — looks decorative
            break;
        case ItemType::DryingRack:
            fillColor = m_occupied
                ? Color{ 160, 130, 80, 255 }   // warm tan — herbs hanging
                : Color{ 120,  90, 50, 255 };  // dark wood — empty rack
            break;
        case ItemType::MacerationJar:   fillColor = Color{  80, 160, 120, 220 }; break; // green tint glass
        case ItemType::CompostBin:      fillColor = Color{  90, 130,  60, 255 }; break; // earthy green
        case ItemType::WorkBench:       fillColor = Color{ 150, 110,  70, 255 }; break; // wood brown
        case ItemType::CopperAlembic:   fillColor = Color{ 180, 100,  40, 255 }; break; // copper
        case ItemType::Bookshelf:       fillColor = Color{ 120,  80,  40, 255 }; break; // dark wood
        case ItemType::StorageChest:    fillColor = Color{ 160, 120,  60, 255 }; break; // medium wood
        case ItemType::MailboxPost:     fillColor = Color{ 180,  40,  40, 255 }; break; // post-box red
        default:                        fillColor = Color{  80,  80, 160, 255 }; break; // blue = higher tier
    }

    // Shadow
    DrawRectangleRec(
        Rectangle{ rect.x + 4, rect.y + 4, rect.width, rect.height },
        Fade(BLACK, 0.25f)
    );

    // Item body
    DrawRectangleRec(rect, fillColor);
    DrawRectangleLinesEx(rect, 2.0f, ColorAlpha(WHITE, 0.3f));

    // Item label
    DrawText(
        def.displayName.c_str(),
        static_cast<int>(rect.x + 4),
        static_cast<int>(rect.y + rect.height / 2 - 6),
        10,
        RAYWHITE
    );

    // Show interaction hint when discovered (fireplace) or always for lab stations
    if (m_discovered && m_type == ItemType::Fireplace) {
        DrawText("[Furnace]",
            static_cast<int>(rect.x + 4),
            static_cast<int>(rect.y + rect.height / 2 + 6),
            9, GOLD);
    }

    // Progress bar for occupied timed apparatus
    if (m_occupied && m_processDurationSec > 0) {
        // Use current time approximation — full UTC not available in Draw(),
        // so we use a stored cached progress set by Update()
        float barW   = rect.width - 8;
        float barX   = rect.x + 4;
        float barY   = rect.y + rect.height - 10;

        // Background track
        DrawRectangle(static_cast<int>(barX), static_cast<int>(barY),
                      static_cast<int>(barW), 5, Fade(BLACK, 0.5f));

        // Progress fill — m_cachedProgress is updated by Update()
        DrawRectangle(static_cast<int>(barX), static_cast<int>(barY),
                      static_cast<int>(barW * m_cachedProgress), 5,
                      Color{ 80, 200, 120, 255 });

        // Loaded item name (truncated)
        std::string loaded = m_loadedItem.plantName;
        if (loaded.length() > 8) loaded = loaded.substr(0, 7) + ".";
        DrawText(loaded.c_str(),
                 static_cast<int>(rect.x + 4),
                 static_cast<int>(rect.y + rect.height / 2 + 6),
                 9, GOLD);
    }

    // Display summary count for DryingRack in world space
    if (m_type == ItemType::DryingRack) {
        int occupiedCount = 0;
        int completeCount = 0;
        long long nowUtc = static_cast<long long>(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );
        for (const auto& slot : m_dryingSlots) {
            if (slot.occupied) {
                occupiedCount++;
                if ((nowUtc - slot.processStartUtc) >= slot.processDurationSec) {
                    completeCount++;
                }
            }
        }

        if (occupiedCount > 0) {
            std::string labelStr = std::to_string(occupiedCount) + " herbs (" + std::to_string(completeCount) + " ready)";
            DrawText(labelStr.c_str(),
                     static_cast<int>(rect.x + 4),
                     static_cast<int>(rect.y + rect.height / 2 + 6),
                     9, GOLD);
        }
    }
}

void PlacedItem::Update(float deltaTime) {
    (void)deltaTime;

    // Update the cached progress so Draw() can show it without needing UTC time
    if (m_occupied && m_processDurationSec > 0) {
        long long nowUtc = static_cast<long long>(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );
        float elapsed = static_cast<float>(nowUtc - m_processStartUtc);
        float total   = static_cast<float>(m_processDurationSec);
        m_cachedProgress = std::min(elapsed / total, 1.0f);
    } else {
        m_cachedProgress = 0.0f;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// IsPlayerNear
// Uses circle-rectangle collision: checks if the player's position is within
// INTERACTION_RADIUS of the item's bounding rectangle.
// ─────────────────────────────────────────────────────────────────────────────
bool PlacedItem::IsPlayerNear(Vector2 playerWorldPos) const {
    Rectangle rect = GetWorldRect();
    return CheckCollisionCircleRec(playerWorldPos, INTERACTION_RADIUS, rect);
}

// ─────────────────────────────────────────────────────────────────────────────
// Interact
// Called when the player presses E near this item.
// Returns an InteractionResult so the game loop can dispatch correctly.
//
// The fireplace has special logic: first inspection triggers the discovery
// moment (sets m_discovered = true, shows the revelation message).
// Subsequent interactions open it as a furnace apparatus.
// ─────────────────────────────────────────────────────────────────────────────
InteractionResult PlacedItem::Interact() {
    switch (m_type) {
        case ItemType::Fireplace:
            if (!m_discovered) {
                // First interaction: "discovery" moment — this is a furnace
                m_discovered = true;
                return InteractionResult::InspectDecoration;
            }
            // Subsequent interactions: open as calcination furnace
            return InteractionResult::OpenApparatus;

        case ItemType::DryingRack:
        case ItemType::MacerationJar:
        case ItemType::MortarAndPestle:
        case ItemType::CopperAlembic:
        case ItemType::GlassFlask:
        case ItemType::GlassblowingStation:
        case ItemType::DistillationTrain:
        case ItemType::SoxhletExtractor:
        case ItemType::PelicanFlask:
        case ItemType::RetortTrain:
        case ItemType::Terrarium:
        case ItemType::CompostBin:
            return InteractionResult::OpenApparatus;

        case ItemType::StorageChest:
            return InteractionResult::OpenStorage;

        case ItemType::MailboxPost:
            return InteractionResult::OpenMailbox;

        case ItemType::Bookshelf:
        case ItemType::WorkBench:
            m_discovered = true;
            return InteractionResult::InspectDecoration;

        default:
            return InteractionResult::None;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// GetInspectionMessage
// Returns the flavour text shown when the player first inspects this item.
// Each inspectable item has its own message rooted in the game's lore.
// ─────────────────────────────────────────────────────────────────────────────
std::string PlacedItem::GetInspectionMessage() const {
    switch (m_type) {
        case ItemType::Fireplace:
            // The discovery moment — the player realises this is a furnace
            return "This fireplace... the heat is controllable. The previous herbalist "
                   "used this as a calcination furnace. It's a furnace!";

        case ItemType::Bookshelf:
            return "The shelves are packed with dog-eared herbals and loose manuscript "
                   "pages. Culpeper's Complete Herbal sits open to the chapter on "
                   "Solar herbs. A handwritten note reads: \"Begin with what the Sun gives freely.\"";

        case ItemType::WorkBench:
            return "A worn but sturdy preparation table. Dried plant stalks and a faint "
                   "smell of alcohol linger in the wood grain. Whoever worked here last "
                   "was methodical.";

        case ItemType::StorageChest:
            return "A lockable chest. The lock is broken — forced open from the outside, "
                   "or the previous herbalist left in a hurry. Whatever was stored here "
                   "is gone.";

        case ItemType::MailboxPost:
            return "A weathered post box. The slot is empty, but the inside smells faintly "
                   "of sealed wax and old paper.";

        default:
            return "Nothing of note.";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Apparatus — timed process methods
// ─────────────────────────────────────────────────────────────────────────────

bool PlacedItem::LoadHarvestItem(const HarvestItem& item, long long nowUtc) {
    if (m_occupied) return false;

    switch (m_type) {
        case ItemType::DryingRack:
            // Only accepts Fresh herbs
            if (item.stage != PlantStage::Fresh) return false;
            m_loadedItem         = item;
            m_processStartUtc    = nowUtc;
            m_processDurationSec = DRYING_DURATION_SEC;
            m_occupied           = true;
            return true;

        case ItemType::MortarAndPestle:
            // Only accepts Dried herbs — grinding is instant
            if (item.stage != PlantStage::Dried) return false;
            m_loadedItem         = item;
            m_loadedItem.stage   = PlantStage::Ground; // immediate
            m_processStartUtc    = nowUtc;
            m_processDurationSec = 0;
            m_occupied           = true;
            return true;

        case ItemType::MacerationJar:
            // Accepts Ground or Dried herbs
            if (item.stage != PlantStage::Ground && item.stage != PlantStage::Dried) return false;
            m_loadedItem         = item;
            m_processStartUtc    = nowUtc;
            m_processDurationSec = MACERATION_DURATION_SEC;
            m_occupied           = true;
            return true;

        case ItemType::CompostBin:
            // Compost accepts plants at any stage and is instant; it stays occupied
            // until the instantaneous "unload" step clears the bin.
            m_loadedItem         = item;
            m_processStartUtc    = nowUtc;
            m_processDurationSec = 0; // Composting is instant "deletion" or has no timer here
            m_occupied           = true;
            return true;

        default:
            return false;
    }
}

bool PlacedItem::IsProcessComplete(long long nowUtc) const {
    if (!m_occupied) return false;
    if (m_processDurationSec == 0) return true; // instant processes
    return (nowUtc - m_processStartUtc) >= m_processDurationSec;
}

bool PlacedItem::UnloadProcessedItem(HarvestItem& outItem, long long nowUtc) {
    if (!m_occupied || !IsProcessComplete(nowUtc)) return false;

    if (m_type == ItemType::CompostBin) {
        // Compost bin consumes the item and does not return it.
        outItem = HarvestItem{};
        m_loadedItem = HarvestItem{}; // Clear the loaded item
    } else {
        outItem = m_loadedItem;

        // Advance stage on unload for timed processes
        if (m_type == ItemType::DryingRack) {
            outItem.stage = PlantStage::Dried;
        } else if (m_type == ItemType::MacerationJar) {
            outItem.stage = PlantStage::Tincture;
        }
        // MortarAndPestle stage was already set to Ground on load
    }

    m_occupied           = false;
    m_processStartUtc    = 0;
    m_processDurationSec = 0;
    return true;
}

float PlacedItem::GetProgress(long long nowUtc) const {
    if (!m_occupied) return -1.0f;
    if (m_processDurationSec == 0) return 1.0f;
    float elapsed = static_cast<float>(nowUtc - m_processStartUtc);
    float total   = static_cast<float>(m_processDurationSec);
    return std::min(elapsed / total, 1.0f);
}

std::string PlacedItem::GetLoadedItemName() const {
    if (!m_occupied) return "";
    return m_loadedItem.GetDisplayName();
}

// ─────────────────────────────────────────────────────────────────────────────
// Serialise
// Saves the item's grid position and discovered state.
// The type is stored as its integer enum value.
// ─────────────────────────────────────────────────────────────────────────────
std::string PlacedItem::Serialise() const {
    nlohmann::json j = {
        { "type",       static_cast<int>(m_type) },
        { "tileX",      m_tileX                  },
        { "tileY",      m_tileY                  },
        { "discovered", m_discovered             },
        { "occupied",   m_occupied               },
        { "processStartUtc", m_processStartUtc   },
        { "processDurationSec", m_processDurationSec },
        { "loadedItem", {
            { "plant",   m_loadedItem.plantName },
            { "stage",   static_cast<int>(m_loadedItem.stage) },
            { "quality", static_cast<int>(m_loadedItem.quality) }
        }}
    };

    if (m_type == ItemType::DryingRack) {
        nlohmann::json slots = nlohmann::json::array();
        for (const auto& slot : m_dryingSlots) {
            slots.push_back({
                { "occupied", slot.occupied },
                { "processStartUtc", slot.processStartUtc },
                { "processDurationSec", slot.processDurationSec },
                { "loadedItem", {
                    { "plant",   slot.loadedItem.plantName },
                    { "stage",   static_cast<int>(slot.loadedItem.stage) },
                    { "quality", static_cast<int>(slot.loadedItem.quality) }
                }}
            });
        }
        j["dryingSlots"] = slots;
    }

    if (m_type == ItemType::StorageChest) {
        nlohmann::json grid = nlohmann::json::array();
        for (const auto& slot : m_storageGrid) {
            grid.push_back({
                { "occupied", slot.occupied },
                { "isHarvest", slot.isHarvest },
                { "placeableType", static_cast<int>(slot.placeableType) },
                { "harvestItem", {
                    { "plant",   slot.harvestItem.plantName },
                    { "stage",   static_cast<int>(slot.harvestItem.stage) },
                    { "quality", static_cast<int>(slot.harvestItem.quality) }
                }}
            });
        }
        j["storageGrid"] = grid;
    }

    return j.dump();
}

void PlacedItem::Deserialise(const nlohmann::json& j) {
    m_discovered = j.value("discovered", false);
    m_occupied = j.value("occupied", false);
    m_processStartUtc = j.value("processStartUtc", 0LL);
    m_processDurationSec = j.value("processDurationSec", 0LL);
    if (j.contains("loadedItem")) {
        auto& li = j["loadedItem"];
        m_loadedItem.plantName = li.value("plant", "");
        m_loadedItem.stage = static_cast<PlantStage>(li.value("stage", 0));
        m_loadedItem.quality = static_cast<HarvestQuality>(li.value("quality", 0));
    }
    if (j.contains("dryingSlots") && m_type == ItemType::DryingRack) {
        auto& slots = j["dryingSlots"];
        m_dryingSlots.clear();
        for (const auto& sj : slots) {
            DryingSlot slot;
            slot.occupied = sj.value("occupied", false);
            slot.processStartUtc = sj.value("processStartUtc", 0LL);
            slot.processDurationSec = sj.value("processDurationSec", 0LL);
            if (sj.contains("loadedItem")) {
                auto& li = sj["loadedItem"];
                slot.loadedItem.plantName = li.value("plant", "");
                slot.loadedItem.stage = static_cast<PlantStage>(li.value("stage", 0));
                slot.loadedItem.quality = static_cast<HarvestQuality>(li.value("quality", 0));
            }
            m_dryingSlots.push_back(slot);
        }
        while (m_dryingSlots.size() < 7) {
            m_dryingSlots.push_back(DryingSlot{});
        }
    }
    if (j.contains("storageGrid") && m_type == ItemType::StorageChest) {
        auto& grid = j["storageGrid"];
        m_storageGrid.clear();
        for (const auto& sg : grid) {
            StorageSlot slot;
            slot.occupied = sg.value("occupied", false);
            slot.isHarvest = sg.value("isHarvest", false);
            slot.placeableType = static_cast<ItemType>(sg.value("placeableType", static_cast<int>(ItemType::COUNT)));
            if (sg.contains("harvestItem")) {
                auto& hi = sg["harvestItem"];
                slot.harvestItem.plantName = hi.value("plant", "");
                slot.harvestItem.stage = static_cast<PlantStage>(hi.value("stage", 0));
                slot.harvestItem.quality = static_cast<HarvestQuality>(hi.value("quality", 0));
            }
            m_storageGrid.push_back(slot);
        }
        while (m_storageGrid.size() < 144) {
            m_storageGrid.push_back(StorageSlot{});
        }
    }
}

} // namespace World
} // namespace FloraPhilosophica
