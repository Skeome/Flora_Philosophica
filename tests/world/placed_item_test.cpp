#include "doctest/doctest.h"
#include "world/placed_item.h"
#include "world/item.h"
#include <chrono>

TEST_CASE("PlacedItem laboratory apparatus tests") {
    using namespace FloraPhilosophica::World;

    long long now = 1000000000LL; // fixed epoch starting time for testing

    SUBCASE("Drying Rack processes fresh herb to dried") {
        PlacedItem rack(ItemType::DryingRack, 0, 0, 64);
        
        HarvestItem freshHerb{ "Lavender", PlantStage::Fresh, HarvestQuality::Standard };
        HarvestItem driedHerb{ "Lavender", PlantStage::Dried, HarvestQuality::Standard };

        // Test loading invalid item
        CHECK_FALSE(rack.LoadHarvestItem(driedHerb, now));
        CHECK_FALSE(rack.IsProcessComplete(now));

        // Test loading valid item
        CHECK(rack.LoadHarvestItem(freshHerb, now));
        CHECK(rack.GetLoadedItemName() == "Fresh Lavender (Standard)");
        CHECK(rack.GetProgress(now) == 0.0f);

        // Test timing progress
        CHECK_FALSE(rack.IsProcessComplete(now + 3600)); // 1 hour elapsed
        CHECK(rack.GetProgress(now + 3600) == 0.5f);
        
        CHECK(rack.IsProcessComplete(now + 7200)); // 2 hours elapsed (DRYING_DURATION_SEC)
        CHECK(rack.GetProgress(now + 7200) == 1.0f);

        // Test unloading
        HarvestItem output;
        CHECK(rack.UnloadProcessedItem(output, now + 7200));
        CHECK(output.plantName == "Lavender");
        CHECK(output.stage == PlantStage::Dried);
        CHECK(output.quality == HarvestQuality::Standard);
    }

    SUBCASE("Mortar and Pestle processes dried herb instantly") {
        PlacedItem mortar(ItemType::MortarAndPestle, 0, 0, 64);
        
        HarvestItem freshHerb{ "Mugwort", PlantStage::Fresh, HarvestQuality::Standard };
        HarvestItem driedHerb{ "Mugwort", PlantStage::Dried, HarvestQuality::Standard };

        // Test loading invalid item
        CHECK_FALSE(mortar.LoadHarvestItem(freshHerb, now));

        // Test loading valid item
        CHECK(mortar.LoadHarvestItem(driedHerb, now));
        CHECK(mortar.IsProcessComplete(now)); // instant
        
        HarvestItem output;
        CHECK(mortar.UnloadProcessedItem(output, now));
        CHECK(output.plantName == "Mugwort");
        CHECK(output.stage == PlantStage::Ground);
    }

    SUBCASE("Maceration Jar processes ground herb to tincture") {
        PlacedItem jar(ItemType::MacerationJar, 0, 0, 64);
        
        HarvestItem freshHerb{ "Comfrey", PlantStage::Fresh, HarvestQuality::Standard };
        HarvestItem groundHerb{ "Comfrey", PlantStage::Ground, HarvestQuality::Standard };

        // Test loading invalid item
        CHECK_FALSE(jar.LoadHarvestItem(freshHerb, now));

        // Test loading valid item
        CHECK(jar.LoadHarvestItem(groundHerb, now));
        CHECK_FALSE(jar.IsProcessComplete(now));

        // Test timing progress (1 hour duration)
        CHECK_FALSE(jar.IsProcessComplete(now + 1800)); // 30 minutes
        CHECK(jar.IsProcessComplete(now + 3600)); // 1 hour

        // Test unloading
        HarvestItem output;
        CHECK(jar.UnloadProcessedItem(output, now + 3600));
        CHECK(output.plantName == "Comfrey");
        CHECK(output.stage == PlantStage::Tincture);
        CHECK(output.GetDisplayName() == "Tincture of Comfrey (Standard)");
    }

    SUBCASE("Alchemical display names for spirits and salts") {
        HarvestItem spirits{ "Wine", PlantStage::Spirits, HarvestQuality::Standard };
        HarvestItem salt{ "Tartar", PlantStage::Salt, HarvestQuality::Standard };

        CHECK(spirits.GetDisplayName() == "Spirits of Wine (Standard)");
        CHECK(salt.GetDisplayName() == "Salt of Tartar (Standard)");
    }
}
