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
