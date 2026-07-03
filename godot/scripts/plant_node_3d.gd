extends Area3D
class_name PlantNode3D

# ─────────────────────────────────────────────────────────────────────────────
# PlantNode3D
# HD-2D 3D version of PlantNode.
# Harvestable plant placed in the overworld. Quality is determined by
# the planetary hour at the moment of harvest.
# Uses Sprite3D + Label3D and distance-based interaction (< 2.0 units).
# ─────────────────────────────────────────────────────────────────────────────

@export var plant_name: String = "Adder's Tongue"
@export var respawn_time: float = 300.0

@onready var sprite: Sprite3D = $Sprite3D
@onready var label: Label3D = $Label3D

var is_harvested: bool = false
var respawn_timer: float = 0.0
var plant_data = null

# ─────────────────────────────────────────────────────────────────────────────
# Lifecycle
# ─────────────────────────────────────────────────────────────────────────────

func _ready() -> void:
	plant_data = PlantDatabase.get_plant(plant_name)
	if plant_data == null:
		push_error("PlantNode3D: Could not find plant data for: " + plant_name)
	_update_visuals()

func _process(delta: float) -> void:
	if is_harvested:
		respawn_timer -= delta
		if respawn_timer <= 0.0:
			_respawn()

# ─────────────────────────────────────────────────────────────────────────────
# Interaction
# ─────────────────────────────────────────────────────────────────────────────

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("interact") and not is_harvested:
		var player = get_tree().get_first_node_in_group("player")
		if player and global_position.distance_to(player.global_position) < 2.0:
			harvest()

# ─────────────────────────────────────────────────────────────────────────────
# Harvest
# ─────────────────────────────────────────────────────────────────────────────

func harvest() -> void:
	if is_harvested: return
	var info = GameManager.get_planetary_hour()
	if info.is_empty(): return
	var day_ruler = info["day_ruler"]
	var hour_ruler = info["ruling_planet"]
	var target_ruler = plant_data.ruler
	var quality = HarvestItem.QUALITY_STANDARD
	if day_ruler == target_ruler and hour_ruler == target_ruler:
		quality = HarvestItem.QUALITY_CELESTIAL
	elif hour_ruler == target_ruler:
		quality = HarvestItem.QUALITY_PRISTINE
	else:
		var hour_is_opposite = hour_ruler in _get_opposite_planets(target_ruler)
		var day_is_opposite = day_ruler in _get_opposite_planets(target_ruler)
		if day_is_opposite and hour_is_opposite:
			quality = HarvestItem.QUALITY_DEBASED
		elif hour_is_opposite:
			quality = HarvestItem.QUALITY_STRESSED
	var item = HarvestItem.new()
	item.plant_name = plant_name
	item.stage = HarvestItem.STAGE_FRESH
	item.quality = quality
	GameManager.inventory.add_harvest_item(item)
	print("Harvested %s: %s" % [plant_name, item.get_display_name()])
	is_harvested = true
	respawn_timer = respawn_time
	_update_visuals()

# ─────────────────────────────────────────────────────────────────────────────
# Respawn & Visuals
# ─────────────────────────────────────────────────────────────────────────────

func _respawn() -> void:
	is_harvested = false
	respawn_timer = 0.0
	_update_visuals()

func _update_visuals() -> void:
	if is_harvested:
		sprite.modulate.a = 0.2
		label.text = "Harvested (Respawning...)"
	else:
		sprite.modulate.a = 1.0
		label.text = plant_name

# ─────────────────────────────────────────────────────────────────────────────
# Planetary Oppositions
# ─────────────────────────────────────────────────────────────────────────────

func _get_opposite_planets(planet: int) -> Array:
	match planet:
		0: return [3, 6]
		1: return [5]
		2: return [4]
		3: return [0]
		4: return [2]
		5: return [1]
		6: return [0]
		_: return []
