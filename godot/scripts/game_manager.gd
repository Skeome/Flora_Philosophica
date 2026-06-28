extends Node

# ─────────────────────────────────────────────────────────────────────────────
# GameManager
# Autoload singleton — persists across all scenes.
# Holds the core C++ extension objects so any scene can access them via:
#     GameManager.inventory
#     GameManager.clock
#     GameManager.room_manager
# ─────────────────────────────────────────────────────────────────────────────

var clock = null
var inventory = null
var room_manager = null

# Set by world.gd before a scene transition so the incoming scene knows
# where to place the player on _ready().
var pending_spawn: Vector2 = Vector2.ZERO

# Player's last saved world position. Restored on load_game().
var last_player_position: Vector2 = Vector2.ZERO

# Active hotbar slot — read by cabin.gd to load into a station on [E].
var selected_hotbar_slot: int = 0

# Medford, Oregon as default — will be replaced by GPS on mobile or Character Creation input
var observer_lat: float = 42.3265
var observer_lon: float = -122.8756

# Character data
var player_gender: int = 0  # 0: Masc, 1: Fem, 2: NB
var player_birth_data: String = ""

# Planetary stats and traits derived from Birth Chart
var player_stats: Dictionary = {
	"Saturn": 10,
	"Jupiter": 10,
	"Mars": 10,
	"Sun": 10,
	"Venus": 10,
	"Mercury": 10,
	"Moon": 10
}
var player_traits: Array = []
var sign_placements: Dictionary = {}
var active_aspects: Array = []

const SAVE_PATH = "user://save.json"

func _ready() -> void:
	clock        = PlanetaryHourCalculator.new()
	inventory    = ClassDB.instantiate("Inventory")
	room_manager = ClassDB.instantiate("RoomManager")
	load_game()

# Returns the current planetary hour Dictionary from the C++ clock.
# Keys: ruling_planet, day_ruler, hour_index, minutes_remaining,
#       planet_name, hour_start_utc, hour_end_utc
func get_planetary_hour() -> Dictionary:
	if clock == null:
		return {}
	var now := int(Time.get_unix_time_from_system())
	return clock.calculate_planetary_hour(observer_lat, observer_lon, now)

# ── Player position tracking ───────────────────────────────────────────────────
func set_player_position(pos: Vector2) -> void:
	last_player_position = pos

func get_saved_player_position() -> Vector2:
	return last_player_position

# ── Save / Load ───────────────────────────────────────────────────────────────
func save_game() -> void:
	# Pull the live player position before writing, if one exists in the tree.
	var tree := Engine.get_main_loop()
	if tree is SceneTree:
		var player := (tree as SceneTree).get_first_node_in_group("player")
		if player:
			last_player_position = player.global_position

	var data := {
		"inventory":         inventory.serialise(),
		"rooms":             room_manager.serialise(),
		"player_pos_x":      last_player_position.x,
		"player_pos_y":      last_player_position.y,
		"observer_lat":      observer_lat,
		"observer_lon":      observer_lon,
		"player_gender":     player_gender,
		"player_birth_data": player_birth_data,
		"timestamp":         int(Time.get_unix_time_from_system())
	}
	var file := FileAccess.open(SAVE_PATH, FileAccess.WRITE)
	if file:
		file.store_string(JSON.stringify(data))
		file.close()

func load_game() -> void:
	if not FileAccess.file_exists(SAVE_PATH):
		return
	var file := FileAccess.open(SAVE_PATH, FileAccess.READ)
	if not file:
		return
	var parsed = JSON.parse_string(file.get_as_text())
	file.close()
	if typeof(parsed) != TYPE_DICTIONARY:
		return
	if parsed.has("inventory"):
		inventory.deserialise(parsed["inventory"])
	if parsed.has("rooms"):
		room_manager.deserialise(parsed["rooms"])
	if parsed.has("player_pos_x") and parsed.has("player_pos_y"):
		last_player_position = Vector2(parsed["player_pos_x"], parsed["player_pos_y"])
	if parsed.has("observer_lat"):
		observer_lat = parsed["observer_lat"]
	if parsed.has("observer_lon"):
		observer_lon = parsed["observer_lon"]
	if parsed.has("player_gender"):
		player_gender = parsed["player_gender"]
	if parsed.has("player_birth_data"):
		player_birth_data = parsed["player_birth_data"]
	
	calculate_natal_chart()

func calculate_natal_chart() -> void:
	# Reset stats and traits
	player_stats = {
		"Saturn": 10,
		"Jupiter": 10,
		"Mars": 10,
		"Sun": 10,
		"Venus": 10,
		"Mercury": 10,
		"Moon": 10
	}
	player_traits = []
	sign_placements = {}
	active_aspects = []
	
	if player_birth_data == "":
		return
		
	# Parse birth timestamp (Format: YYYY-MM-DD HH:MM)
	var parts = player_birth_data.split(" ")
	if parts.size() < 2:
		return
	var date_parts = parts[0].split("-")
	var time_parts = parts[1].split(":")
	if date_parts.size() < 3 or time_parts.size() < 2:
		return
		
	var datetime_dict = {
		"year": date_parts[0].to_int(),
		"month": date_parts[1].to_int(),
		"day": date_parts[2].to_int(),
		"hour": time_parts[0].to_int(),
		"minute": time_parts[1].to_int(),
		"second": 0
	}
	var birth_timestamp = Time.get_unix_time_from_datetime_dict(datetime_dict)
	
	# Instantiate orbit calculator
	var orbit_calc = PlanetaryOrbitCalculator.new()
	var planets = ["Saturn", "Jupiter", "Mars", "Sun", "Venus", "Mercury", "Moon"]
	
	# 1. Placements (flat +2 points to the stat ruled by the sign the planet is in)
	var zodiac_signs = [
		{"name": "Aries", "ruler": "Mars"},
		{"name": "Taurus", "ruler": "Venus"},
		{"name": "Gemini", "ruler": "Mercury"},
		{"name": "Cancer", "ruler": "Moon"},
		{"name": "Leo", "ruler": "Sun"},
		{"name": "Virgo", "ruler": "Mercury"},
		{"name": "Libra", "ruler": "Venus"},
		{"name": "Scorpio", "ruler": "Mars"},
		{"name": "Sagittarius", "ruler": "Jupiter"},
		{"name": "Capricorn", "ruler": "Saturn"},
		{"name": "Aquarius", "ruler": "Saturn"},
		{"name": "Pisces", "ruler": "Jupiter"}
	]
	
	var longitudes = {}
	for p in planets:
		var pos = orbit_calc.get_geocentric_position(p, birth_timestamp, observer_lat, observer_lon)
		var lon = pos.get("lon", 0.0)
		longitudes[p] = lon
		
		var sign_idx = int(lon / 30.0) % 12
		var planetary_sign = zodiac_signs[sign_idx]
		sign_placements[p] = planetary_sign["name"]
		
		var ruler = planetary_sign["ruler"]
		if player_stats.has(ruler):
			player_stats[ruler] += 2
			
	# 2. Aspects (Conjunction 0, Sextile 60, Square 90, Trine 120, Opposition 180)
	var aspect_types = [
		{"name": "Conjunction", "angle": 0.0, "orb": 8.0, "type": "synergy", "desc": "Synergy: +5% effectiveness to both planets' actions"},
		{"name": "Sextile", "angle": 60.0, "orb": 6.0, "type": "harmony", "desc": "Harmony: +10% efficiency to both planets' actions"},
		{"name": "Square", "angle": 90.0, "orb": 8.0, "type": "tension", "desc": "Tension: +15% potency but +10% mental fatigue cost to both planets' actions"},
		{"name": "Trine", "angle": 120.0, "orb": 8.0, "type": "harmony", "desc": "Harmony: +10% efficiency to both planets' actions"},
		{"name": "Opposition", "angle": 180.0, "orb": 8.0, "type": "tension", "desc": "Tension: +15% potency but +10% mental fatigue cost to both planets' actions"}
	]
	
	for i in range(planets.size()):
		for j in range(i + 1, planets.size()):
			var p1 = planets[i]
			var p2 = planets[j]
			var lon1 = longitudes[p1]
			var lon2 = longitudes[p2]
			
			var diff = abs(lon1 - lon2)
			if diff > 180.0:
				diff = 360.0 - diff
				
			for asp in aspect_types:
				if abs(diff - asp["angle"]) <= asp["orb"]:
					var aspect_info = {
						"planet1": p1,
						"planet2": p2,
						"aspect": asp["name"],
						"type": asp["type"],
						"desc": asp["desc"],
						"angle_diff": diff
					}
					active_aspects.append(aspect_info)
					
					var trait_name = "%s %s %s" % [p1, asp["name"], p2]
					player_traits.append({
						"name": trait_name,
						"type": asp["type"],
						"desc": asp["desc"]
					})
					break

# Performs Al-Kindi's geometric temper calculation on the 4 humors (choler, sanguine, phlegm, melancholy).
# Humors are integers in range 0-28 (representing Jabir's subdivisions).
# Returns a Dictionary with percentages, parts, and degrees.
func calculate_spagyric_temper(choler: int, sanguine: int, phlegm: int, melancholy: int) -> Dictionary:
	var total = choler + sanguine + phlegm + melancholy
	var c_pct = 0.0
	var s_pct = 0.0
	var p_pct = 0.0
	var m_pct = 0.0
	if total > 0:
		c_pct = float(choler) / total * 100.0
		s_pct = float(sanguine) / total * 100.0
		p_pct = float(phlegm) / total * 100.0
		m_pct = float(melancholy) / total * 100.0
	else:
		c_pct = 25.0
		s_pct = 25.0
		p_pct = 25.0
		m_pct = 25.0
		
	# Convert element levels to parts of Hot, Cold, Wet, Dry geometrically
	# Walter of Odington intensities:
	# Choler (Fire): Hot in 4th, Dry in 3rd -> Hot parts = 2^(C/7), Dry parts = 2^(3/4 * C/7)
	# Sanguine (Air): Wet in 4th, Hot in 3rd -> Wet parts = 2^(S/7), Hot parts = 2^(3/4 * S/7)
	# Phlegm (Water): Cold in 4th, Wet in 3rd -> Cold parts = 2^(P/7), Wet parts = 2^(3/4 * P/7)
	# Melancholy (Earth): Dry in 4th, Cold in 3rd -> Dry parts = 2^(M/7), Cold parts = 2^(3/4 * M/7)
	
	var c_val = float(choler) / 7.0
	var s_val = float(sanguine) / 7.0
	var p_val = float(phlegm) / 7.0
	var m_val = float(melancholy) / 7.0
	
	var hot_parts = pow(2.0, c_val) + pow(2.0, 0.75 * s_val)
	var cold_parts = pow(2.0, p_val) + pow(2.0, 0.75 * m_val)
	var wet_parts = pow(2.0, s_val) + pow(2.0, 0.75 * p_val)
	var dry_parts = pow(2.0, m_val) + pow(2.0, 0.75 * c_val)
	
	var hc_ratio = 1.0
	var hc_degree = 0.0
	if cold_parts > 0.0:
		hc_ratio = hot_parts / cold_parts
	if hc_ratio >= 1.0:
		hc_degree = log(hc_ratio) / log(2.0)
	else:
		hc_degree = - (log(1.0 / hc_ratio) / log(2.0))
		
	var wd_ratio = 1.0
	var wd_degree = 0.0
	if dry_parts > 0.0:
		wd_ratio = wet_parts / dry_parts
	if wd_ratio >= 1.0:
		wd_degree = log(wd_ratio) / log(2.0)
	else:
		wd_degree = - (log(1.0 / wd_ratio) / log(2.0))
		
	return {
		"hot_cold_degree": hc_degree,
		"wet_dry_degree": wd_degree,
		"hot_parts": hot_parts,
		"cold_parts": cold_parts,
		"wet_parts": wet_parts,
		"dry_parts": dry_parts,
		"choler_pct": c_pct,
		"sanguine_pct": s_pct,
		"phlegm_pct": p_pct,
		"melancholy_pct": m_pct
	}
