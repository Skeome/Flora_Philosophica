#include "placed_item.h"
#include "item.h"
#include "raymath.h"
#include "nlohmann/json.hpp"
#include <cmath>

namespace FloraPhilosophica {
namespace World {

PlacedItem::PlacedItem(ItemType type, int tileX, int tileY, int tileSize)
    : m_type(type)
    , m_tileX(tileX)
    , m_tileY(tileY)
    , m_tileSize(tileSize)
    , m_discovered(false)
{}

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
        case ItemType::MortarAndPestle: fillColor = Color{ 180, 170, 155, 255 }; break; // cream stone
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
}

void PlacedItem::Update(float deltaTime) {
    // Per-frame updates — placeholder for animation states and timer overlays
    // Will be expanded when apparatus logic is added in Step 4
    (void)deltaTime;
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
            return InteractionResult::OpenApparatus;

        case ItemType::CompostBin:
            return InteractionResult::OpenCompost;

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
// Serialise
// Saves the item's grid position and discovered state.
// The type is stored as its integer enum value.
// ─────────────────────────────────────────────────────────────────────────────
std::string PlacedItem::Serialise() const {
    nlohmann::json j = {
        { "type",       static_cast<int>(m_type) },
        { "tileX",      m_tileX                  },
        { "tileY",      m_tileY                  },
        { "discovered", m_discovered             }
    };
    return j.dump();
}

} // namespace World
} // namespace FloraPhilosophica
