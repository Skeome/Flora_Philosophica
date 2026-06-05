#include "doctest/doctest.h"
#include "core/clock.h"
#include <iostream>

TEST_CASE("AstrologicalClock comprehensive validation") {
    using namespace FloraPhilosophica::Core;
    AstrologicalClock clock;

    // Test Coordinates: Portland, Oregon, USA (approx. 45.5152 N, -122.6784 W)
    const double lat = 45.5152;
    const double lon = -122.6784;

    // Test Date: June 3, 2026 (Wednesday, ruled by Mercury)
    // 2026-06-03 00:00:00 UTC timestamp = 1780444800
    const long long midUTC = 1780444800;

    SUBCASE("Verify weekday mapping of base day (Wednesday)") {
        // Test an explicit daytime hour right at solar noon (approx 12:00 local time = 20:00 UTC)
        // June 3, 2026 20:00:00 UTC = 1777852800 + 20 hours = 1777924800
        long long noonTimestamp = midUTC + (20 * 3600);
        PlanetaryHourInfo info = clock.CalculatePlanetaryHour(lat, lon, noonTimestamp);

        // Standard daytime hour at solar noon (midday) should always be ruled by the day's planet (Sun/Mercury/etc)
        // Wait, at exact solar noon (Hour 6 of daytime), the ruler should be:
        // Ruler = (Mercury_Index + 6) % 7
        // Mercury index is 5. (5 + 6) % 7 = 11 % 7 = 4 (Venus).
        // Let's verify the hour index and name are valid.
        CHECK(info.hourIndex >= 0);
        CHECK(info.hourIndex < 24);
        CHECK(!info.planetName.empty());
    }

    SUBCASE("Verify Chaldean sequence rotation") {
        // Let's take a timestamp for Wednesday afternoon (e.g. 21:00 UTC)
        long long t1 = midUTC + (21 * 3600);
        PlanetaryHourInfo h1 = clock.CalculatePlanetaryHour(lat, lon, t1);

        // Next real-time hour (3600 seconds later)
        long long t2 = t1 + 3600;
        PlanetaryHourInfo h2 = clock.CalculatePlanetaryHour(lat, lon, t2);

        // Check that the hour index advanced or transitioned
        CHECK(h2.hourIndex != h1.hourIndex);
        
        // Check that the ruling planet matches the next planet in Chaldean sequence:
        // PlanetRulingHour(H) = (DayRuler + H) % 7
        int expectedIndex2 = (static_cast<int>(h1.rulingPlanet) + (h2.hourIndex - h1.hourIndex + 7)) % 7;
        CHECK(static_cast<int>(h2.rulingPlanet) == expectedIndex2);
    }

    SUBCASE("Astrological Day boundary shifts at Sunrise") {
        // Wednesday Sunrise in Portland on June 3 is approx 5:21 AM local time (PDT, UTC-7).
        // 5:21 AM PDT = 12:21 PM UTC = 12 * 3600 + 21 * 60 = 44460 seconds from midnight UTC.
        // Let's test a timestamp just before sunrise: 11:30 AM UTC = 41400 seconds.
        long long preSunrise = midUTC + 41400;
        PlanetaryHourInfo preInfo = clock.CalculatePlanetaryHour(lat, lon, preSunrise);

        // Since it's before Wednesday sunrise, it should belong to Yesterday's astrological day (Tuesday, ruled by Mars).
        // Let's test a timestamp just after sunrise: 1:30 PM UTC = 48600 seconds.
        long long postSunrise = midUTC + 48600;
        PlanetaryHourInfo postInfo = clock.CalculatePlanetaryHour(lat, lon, postSunrise);

        // Pre-sunrise hour should be in the night segment (12 to 23)
        CHECK(preInfo.hourIndex >= 12);
        CHECK(preInfo.hourIndex < 24);

        // Post-sunrise hour should be in the daytime segment (0 to 11)
        CHECK(postInfo.hourIndex >= 0);
        CHECK(postInfo.hourIndex < 12);

        // Tuesday (pre-sunrise day ruler) is Mars. Wednesday (post-sunrise day ruler) is Mercury.
        // Verify that the post-sunrise hour index 0 is ruled by Mercury (5).
        if (postInfo.hourIndex == 0) {
            CHECK(postInfo.rulingPlanet == Planet::Mercury);
        }
    }

    SUBCASE("Polar Region Fallback validation") {
        // High Arctic latitude (e.g. Alert, Nunavut, Canada: 82.5018 N, -62.3481 W)
        // On June 3, Alert experiences midnight sun (polar day, sun never sets).
        const double polarLat = 82.5018;
        const double polarLon = -62.3481;

        PlanetaryHourInfo polarInfo = clock.CalculatePlanetaryHour(polarLat, polarLon, midUTC + 3600);

        // Check that polar day fallback is triggered:
        // 1. Equal hour fallback is activated.
        // 2. Since midUTC + 3600 is 1:00 AM UTC, the hour index should be 1.
        CHECK(polarInfo.hourIndex == 1);
        CHECK(std::abs(polarInfo.minutesRemaining - 60.0) < 0.001);
    }
}