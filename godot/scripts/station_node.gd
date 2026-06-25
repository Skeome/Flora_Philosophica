extends Node2D

class_name StationNode

# ─────────────────────────────────────────────────────────────────────────────
# StationNode
# Visual + logical wrapper around a C++ PlacedItem apparatus.
# One instance exists per station in the cabin scene.
#
# Interaction flow (driven by cabin.gd):
#   1. cabin.gd detects the nearest StationNode within INTERACT_RANGE
#   2. Player presses [E] → cabin.gd calls station.interact(inventory, slot)
#   3. interact() returns:
#        null  — action was immediate (item collected, M&P was empty, etc.)
#        Node  — a mini-game scene to launch; cabin.gd parents it to MinigameLayer
#   4. Mini-game emits minigame_finished(time_bonus: float) when done
#   5. cabin.gd calls station.award_time_bonus(time_bonus)
#
# Time compression:
#   _time_bonus_sec tracks game-seconds awarded by mini-game sessions.
#   _effective_now() injects this offset into all PlacedItem time queries,
#   so processing can complete earlier than real-time without touching C++.
# ─────────────────────────────────────────────────────────────────────────────

# Preloaded mini-game scenes (must exist before this script is parsed)
const MINIGAME_MORTAR  := preload("res://scenes/minigame_mortar.tscn")
const MINIGAME_FURNACE := preload("res://scenes/minigame_furnace.tscn")

# ── Item type constants (mirrors ItemType enum in item.h) ────────────────────
const TYPE_FIREPLACE        := 0
const TYPE_DRYING_RACK      := 1
const TYPE_MORTAR_PESTLE    := 2
const TYPE_MACERATION_JAR   := 3
const TYPE_COMPOST_BIN      := 4
const TYPE_FURNACE          := 17
const TYPE_LEACHING_DISH    := 18

# ── Exports ───────────────────────────────────────────────────────────────────
@export_enum(
	"Fireplace:0","DryingRack:1","MortarAndPestle:2","MacerationJar:3",
	"CompostBin:4","WorkBench:5","CopperAlembic:6","GlassFlask:7",
	"GlassblowingStation:8","DistillationTrain:9","SoxhletExtractor:10",
	"PelicanFlask:11","RetortTrain:12","Terrarium:13","Bookshelf:14",
	"StorageChest:15","MailboxPost:16","Furnace:17","LeachingDish:18"
) var item_type: int = TYPE_MORTAR_PESTLE

@export var display_name: String = "Mortar & Pestle"

# ── Scene refs ────────────────────────────────────────────────────────────────
@warning_ignore("unused_private_class_variable")
@onready var _sprite:   Sprite2D    = $Sprite2D
@onready var _name_lbl: Label       = $NameLabel
@onready var _stat_lbl: Label       = $StatusLabel
@onready var _prog_bar: ProgressBar = $ProgressBar

# ── State ─────────────────────────────────────────────────────────────────────
var _placed_item:       PlacedItem  = null
var _has_item:          bool        = false
var _time_bonus_sec:    float       = 0.0
var _loaded_herb_name:  String      = ""

# ─────────────────────────────────────────────────────────────────────────────

func _ready() -> void:
	_placed_item = PlacedItem.new()
	_placed_item.init(item_type, 0, 0)
	_name_lbl.text = display_name
	_refresh_display()

func _process(_delta: float) -> void:
	_refresh_display()

# ── Public API (called by cabin.gd) ──────────────────────────────────────────

func get_item_type() -> int:
	return item_type

func get_interact_label() -> String:
	if not _has_item:
		return "Use " + display_name
	if _placed_item.is_process_complete(_effective_now()):
		return "Collect from " + display_name
	return "Work at " + display_name

## Returns a mini-game Node to launch, or null if the action was immediate.
func interact(inventory, selected_slot: int) -> Node:
	var now := _effective_now()

	# ── Collect completed item ────────────────────────────────────────────
	if _has_item and _placed_item.is_process_complete(now):
		var result = _placed_item.unload_processed_item(now)
		_has_item         = false
		_time_bonus_sec   = 0.0
		_loaded_herb_name = ""
		if result:
			inventory.add_harvest_item(result)
			# Maceration also yields spent residue (the "compost bin trap" material)
			if item_type == TYPE_MACERATION_JAR:
				var spent := HarvestItem.new()
				spent.plant_name = result.plant_name
				@warning_ignore("int_as_enum_without_cast", "int_as_enum_without_match")
				spent.stage      = HarvestItem.STAGE_SPENT
				spent.quality    = result.quality
				inventory.add_harvest_item(spent)
		return null

	# ── Open mini-game for in-progress station ────────────────────────────
	if _has_item:
		return _create_minigame()

	# ── Load from selected hotbar slot ────────────────────────────────────
	var slot = inventory.get_slot(selected_slot)
	if slot.occupied and slot.is_herb:
		var real_now := int(Time.get_unix_time_from_system())
		if _placed_item.load_harvest_item(slot.herb, real_now):
			_loaded_herb_name = slot.herb.plant_name
			_has_item         = true
			inventory.clear_slot(selected_slot)
			return _create_minigame()

	return null

## Called by cabin.gd after a mini-game session ends.
## time_bonus > 0  → game-seconds of processing time to credit
## time_bonus == -1 → batch was destroyed (overheating)
func award_time_bonus(time_bonus: float) -> void:
	if time_bonus < 0.0:
		_has_item         = false
		_time_bonus_sec   = 0.0
		_loaded_herb_name = ""
		_placed_item = PlacedItem.new()
		_placed_item.init(item_type, 0, 0)
	else:
		_time_bonus_sec += time_bonus

# ── Private helpers ───────────────────────────────────────────────────────────

## Returns the current real-time unix timestamp plus any mini-game bonus.
func _effective_now() -> int:
	return int(Time.get_unix_time_from_system()) + int(_time_bonus_sec)

func _create_minigame() -> Node:
	match item_type:
		TYPE_MORTAR_PESTLE:
			var mg: Node = MINIGAME_MORTAR.instantiate()
			mg.herb_name = _loaded_herb_name
			return mg
		TYPE_FURNACE:
			var mg: Node = MINIGAME_FURNACE.instantiate()
			mg.process_name = "Calcination — " + _loaded_herb_name
			return mg
		_:
			# Passive stations (Drying Rack, Maceration Jar, etc.)
			# show a progress UI but have no active mini-game yet.
			return null

func _refresh_display() -> void:
	if not _has_item:
		_stat_lbl.text    = "Empty"
		_prog_bar.visible = false
		return

	var now  := _effective_now()
	var prog := _placed_item.get_progress(now)

	if prog < 0.0:
		_stat_lbl.text    = "Ready to load"
		_prog_bar.visible = false
	elif prog >= 1.0:
		_stat_lbl.text    = _loaded_herb_name + " — Ready ✓"
		_prog_bar.visible = true
		_prog_bar.value   = 100.0
	else:
		_stat_lbl.text    = _loaded_herb_name + " — Processing..."
		_prog_bar.visible = true
		_prog_bar.value   = prog * 100.0
