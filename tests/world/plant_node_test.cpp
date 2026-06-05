#include "doctest/doctest.h"
#include "world/plant_node.h"
#include "core/clock.h"

TEST_CASE("PlantNode harvest quality calculation") {
    using namespace FloraPhilosophica::World;
    using namespace FloraPhilosophica::Core;

    // St. John's Wort is ruled by the Sun (☉)
    PlantNode node("St. John's Wort", { 100.0f, 100.0f });

    SUBCASE("Pristine Harvest: Matching Day and Hour") {
        // Sun day, Sun hour
        HarvestQuality quality = node.Harvest(Planet::Sun, Planet::Sun);
        CHECK(quality == HarvestQuality::Pristine);
    }

    SUBCASE("Standard Harvest: Different Day, Matching Hour") {
        // Reset node (or create new one)
        PlantNode node2("St. John's Wort", { 100.0f, 100.0f });
        // Moon day, Sun hour
        HarvestQuality quality = node2.Harvest(Planet::Moon, Planet::Sun);
        CHECK(quality == HarvestQuality::Standard);
    }

    SUBCASE("Standard Harvest: Different Day, Different Hour") {
        PlantNode node3("St. John's Wort", { 100.0f, 100.0f });
        // Mars day, Venus hour
        HarvestQuality quality = node3.Harvest(Planet::Mars, Planet::Venus);
        CHECK(quality == HarvestQuality::Standard);
    }

    SUBCASE("Debased Harvest: Opposite Planetary Hour") {
        PlantNode node4("St. John's Wort", { 100.0f, 100.0f });
        // Saturn is the opposite of Sun in our implementation
        HarvestQuality quality = node4.Harvest(Planet::Sun, Planet::Saturn);
        CHECK(quality == HarvestQuality::Debased);
    }
}

TEST_CASE("PlantNode respawn mechanics") {
    using namespace FloraPhilosophica::World;
    using namespace FloraPhilosophica::Core;

    PlantNode node("Nettle", { 0, 0 });
    
    CHECK(node.IsActive() == true);
    
    node.Harvest(Planet::Mars, Planet::Mars);
    CHECK(node.IsActive() == false);
    
    // Simulate time passing (halfway)
    node.Update(150.0f); 
    CHECK(node.IsActive() == false);
    
    // Simulate time passing (complete)
    node.Update(151.0f);
    CHECK(node.IsActive() == true);
}
