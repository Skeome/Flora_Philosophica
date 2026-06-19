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

# Medford, Oregon as default — will be replaced by GPS on mobile
var observer_lat: float = 42.3265
var observer_lon: float = -122.8756

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

# ── Save / Load ───────────────────────────────────────────────────────────────
func save_game() -> void:
	var data := {
		"inventory": inventory.serialise(),
		"rooms":     room_manager.serialise(),
		"timestamp": int(Time.get_unix_time_from_system())
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
