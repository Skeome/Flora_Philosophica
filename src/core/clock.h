#ifndef FLORA_PHILOSOPHICA_CORE_CLOCK_H
#define FLORA_PHILOSOPHICA_CORE_CLOCK_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class PlanetaryHourCalculator : public RefCounted {
    GDCLASS(PlanetaryHourCalculator, RefCounted)

public:
    enum Planet {
        SATURN = 0,
        JUPITER = 1,
        MARS = 2,
        SUN = 3,
        VENUS = 4,
        MERCURY = 5,
        MOON = 6
    };

    PlanetaryHourCalculator();
    ~PlanetaryHourCalculator();

    Dictionary calculate_planetary_hour(double latitude, double longitude, int64_t utc_timestamp);
    static String get_planet_name(Planet planet);

protected:
    static void _bind_methods();

private:
    struct SolarTimes {
        double sunriseUTC;      // Minutes from UTC midnight
        double sunsetUTC;       // Minutes from UTC midnight
        bool isPolar;           // True if polar day or night (no sunrise/sunset)
    };

    struct DayLimits {
        int64_t sunriseUnix;    // Sunrise Unix timestamp
        int64_t sunsetUnix;     // Sunset Unix timestamp
        bool isPolar;           // True if polar day or night (no sunrise/sunset)
    };

    SolarTimes calculate_solar_limits(double latitude, double longitude, double julianDate);
    DayLimits get_day_limits(double latitude, double longitude, int64_t utcTimestamp, int dayOffset);
};

} // namespace godot

VARIANT_ENUM_CAST(PlanetaryHourCalculator::Planet);

#endif // FLORA_PHILOSOPHICA_CORE_CLOCK_H
