#ifndef FLORA_PHILOSOPHIA_CORE_CLOCK_H
#define FLORA_PHILOSOPHIA_CORE_CLOCK_H

#include <string>

namespace FloraPhilosophia {
namespace Core {

// The Seven Planetary Rulers, ordered precisely according to the Chaldean Sequence.
// This specific ordering is crucial as it allows us to calculate ruling planets 
// using simple arithmetic shift: (WeekdayRuler + HourIndex) % 7.
enum class Planet {
    Saturn = 0,
    Jupiter = 1,
    Mars = 2,
    Sun = 3,
    Venus = 4,
    Mercury = 5,
    Moon = 6
};

// Represents the calculated planetary hour state for a given timestamp.
struct PlanetaryHourInfo {
    Planet rulingPlanet;        // Ruling planet of the current hour
    int hourIndex;              // 0 to 11 for day hours, 12 to 23 for night hours
    double minutesRemaining;    // Minutes remaining in the current planetary hour
    std::string planetName;     // String representation of the ruling planet
    long long hourStartUtc;     // Unix timestamp when this hour began
    long long hourEndUtc;       // Unix timestamp when this hour ends
};

class AstrologicalClock {
public:
    AstrologicalClock();

    // Calculates the planetary hour based on coordinates and a Unix UTC timestamp
    PlanetaryHourInfo CalculatePlanetaryHour(double latitude, double longitude, long long utcTimestamp);

    // Get string representation of a planet
    static std::string GetPlanetName(Planet planet);

private:
    // Internal struct to hold Sunrise and Sunset limits in minutes from UTC midnight
    struct SolarTimes {
        double sunriseUTC;      // Minutes from UTC midnight
        double sunsetUTC;       // Minutes from UTC midnight
        bool isPolar;           // True if polar day or night (no sunrise/sunset)
    };

    // Internal struct to hold absolute Unix timestamps for a specific day's boundaries
    struct DayLimits {
        long long sunriseUnix;  // Sunrise Unix timestamp
        long long sunsetUnix;   // Sunset Unix timestamp
        bool isPolar;           // True if polar day or night (no sunrise/sunset)
    };

    // Computes solar times (sunrise/sunset) for a specific Julian Date
    SolarTimes CalculateSolarLimits(double latitude, double longitude, double julianDate);

    // Calculates sunrise and sunset Unix timestamps for a target day relative to a base timestamp
    // dayOffset: -1 for yesterday, 0 for today, 1 for tomorrow
    DayLimits GetDayLimits(double latitude, double longitude, long long utcTimestamp, int dayOffset);
};

} // namespace Core
} // namespace FloraPhilosophia

#endif // FLORA_PHILOSOPHIA_CORE_CLOCK_H