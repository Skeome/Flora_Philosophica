extends Area2D

# ─────────────────────────────────────────────────────────────────────────────
# PlantNode
# Handles harvesting plants based on planetary alignment.
# ─────────────────────────────────────────────────────────────────────────────

@export var plant_name: String = "Adder's Tongue"
@export var respawn_time: float = 300.0 # 5 minutes

@onready var sprite: Sprite2D = $Sprite2D
@onready var label: Label = $Label

var is_harvested: bool = false
var respawn_timer: float = 0.0
var plant_data = null

func _ready() -> void:
	# Load plant data from C++ database
	plant_data = PlantDatabase.get_plant(plant_name)
	if plant_data == null:
		push_error("PlantNode: Could not find plant data for: " + plant_name)
	
	_update_visuals()

func _process(delta: float) -> void:
	if is_harvested:
		respawn_timer -= delta
		if respawn_timer <= 0.0:
			_respawn()

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("interact") and not is_harvested:
		if overlaps_body(get_tree().get_first_node_in_group("player")):
			harvest()

func harvest() -> void:
	if is_harvested: return
	
	var info = GameManager.get_planetary_hour()
	if info.is_empty(): return
	
	var day_ruler = info["day_ruler"]
	var hour_ruler = info["ruling_planet"]
	var target_ruler = plant_data.ruler
	
	var quality = HarvestItem.QUALITY_STANDARD
	
	# Logic mirrored from C++ PlantNode
	# Celestial: matching day AND hour
	if day_ruler == target_ruler and hour_ruler == target_ruler:
		quality = HarvestItem.QUALITY_CELESTIAL
	# Pristine: matching hour only
	elif hour_ruler == target_ruler:
		quality = HarvestItem.QUALITY_PRISTINE
	else:
		# Check for opposite planets
		var hour_is_opposite = hour_ruler in _get_opposite_planets(target_ruler)
		var day_is_opposite = day_ruler in _get_opposite_planets(target_ruler)
		
		# Debased: BOTH day AND hour are opposite
		if day_is_opposite and hour_is_opposite:
			quality = HarvestItem.QUALITY_DEBASED
		# Stressed: hour is opposite only
		elif hour_is_opposite:
			quality = HarvestItem.QUALITY_STRESSED
	
	# Create the item
	var item = HarvestItem.new()
	item.plant_name = plant_name
	item.stage = HarvestItem.STAGE_FRESH
	item.quality = quality
	
	# Add to inventory
	GameManager.inventory.add_harvest_item(item)
	
	# Feedback
	print("Harvested %s: %s" % [plant_name, item.get_display_name()])
	
	# State change
	is_harvested = true
	respawn_timer = respawn_time
	_update_visuals()

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

func _get_opposite_planets(planet: int) -> Array:
	# 0:Saturn, 1:Jupiter, 2:Mars, 3:Sun, 4:Venus, 5:Mercury, 6:Moon
	match planet:
		0: return [3, 6] # Saturn -> Sun, Moon
		1: return [5]    # Jupiter -> Mercury
		2: return [4]    # Mars -> Venus
		3: return [0]    # Sun -> Saturn
		4: return [2]    # Venus -> Mars
		5: return [1]    # Mercury -> Jupiter
		6: return [0]    # Moon -> Saturn
		_: return []
