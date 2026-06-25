extends Node2D

# ─────────────────────────────────────────────────────────────────────────────
# Cabin
# Scene manager for cabin_main.tscn (and any cabin sub-room scenes).
#
# Responsibilities:
#   - Door transitions (Area2D nodes named Door_*)
#   - Scanning Stations/ children to find the nearest interactive StationNode
#   - Delegating [E] interactions to the appropriate StationNode
#   - Launching and cleaning up mini-game overlays on MinigameLayer
#   - Setting camera limits for the enclosed interior space
# ─────────────────────────────────────────────────────────────────────────────

const INTERACT_RANGE: float = 28.0  # Native-resolution pixels

@onready var _stations:    Node2D      = $Stations
@onready var _mg_layer:    CanvasLayer = $MinigameLayer
@onready var _hint_label:  Label       = $HUD/InteractionHint

var _active_mg: Node        = null   # Currently running mini-game
var _active_st: StationNode = null   # Station that owns the active mini-game
var _nearest:   StationNode = null   # Nearest station within interact range

# ─────────────────────────────────────────────────────────────────────────────

func _ready() -> void:
	_hint_label.visible = false

	# Wire door transitions — same convention as world.gd
	for child in get_children():
		if child is Area2D and child.name.begins_with("Door_"):
			child.body_entered.connect(_on_door_entered.bind(child))

	# Apply player spawn position from a previous scene transition
	var player := _get_player()
	if player and GameManager.pending_spawn != Vector2.ZERO:
		player.global_position  = GameManager.pending_spawn
		GameManager.pending_spawn = Vector2.ZERO

	# Clamp the Camera2D to the cabin bounds so the player can't see outside
	if player:
		var cam := player.get_node_or_null("Camera2D") as Camera2D
		if cam:
			cam.limit_left   = 0
			cam.limit_top    = 0
			cam.limit_right  = 480
			cam.limit_bottom = 320

# ─────────────────────────────────────────────────────────────────────────────

func _on_door_entered(body: Node2D, door: Area2D) -> void:
	if not body.is_in_group("player"):
		return
	var target: String  = door.get_meta("target_scene", "")
	var spawn:  Vector2 = door.get_meta("spawn_point",  Vector2.ZERO)
	if target.is_empty():
		push_warning("Cabin door '%s' has no target_scene metadata." % door.name)
		return
	GameManager.save_game()
	GameManager.pending_spawn = spawn
	get_tree().change_scene_to_file(target)

# ─────────────────────────────────────────────────────────────────────────────

func _process(_delta: float) -> void:
	if _active_mg:
		return   # All input goes to the mini-game while it is open
	_scan_nearest_station()

func _unhandled_input(event: InputEvent) -> void:
	if _active_mg:
		return
	if event.is_action_pressed("interact") and _nearest:
		_open_station(_nearest)
		get_viewport().set_input_as_handled()

# ── Station scanning ──────────────────────────────────────────────────────────

func _scan_nearest_station() -> void:
	var player := _get_player()
	if not player:
		_hint_label.visible = false
		return

	_nearest = null
	var best := INTERACT_RANGE

	for child in _stations.get_children():
		if not child is StationNode:
			continue
		var d: float = child.global_position.distance_to(player.global_position)
		if d < best:
			best     = d
			_nearest = child

	if _nearest:
		_hint_label.text    = "[E]  %s" % _nearest.get_interact_label()
		_hint_label.visible = true
	else:
		_hint_label.visible = false

# ── Mini-game lifecycle ───────────────────────────────────────────────────────

func _open_station(station: StationNode) -> void:
	var inv          = GameManager.inventory
	var selected_slot: int = GameManager.selected_hotbar_slot

	var mg: Node = station.interact(inv, selected_slot)
	if mg == null:
		return   # Immediate action (item collected, nothing to do)

	# Lock player movement while mini-game is running
	var player := _get_player()
	if player and player.has_method("set_input_locked"):
		player.set_input_locked(true)

	_active_st = station
	_active_mg = mg
	_mg_layer.add_child(mg)
	mg.minigame_finished.connect(_on_mg_finished)

func _on_mg_finished(time_bonus: float) -> void:
	if _active_st:
		_active_st.award_time_bonus(time_bonus)
		_active_st = null

	if _active_mg:
		_active_mg.queue_free()
		_active_mg = null

	var player := _get_player()
	if player and player.has_method("set_input_locked"):
		player.set_input_locked(false)

	if time_bonus < 0.0:
		_hint_label.text    = "💥  Batch destroyed by overheating."
		_hint_label.visible = true
		await get_tree().create_timer(2.5).timeout
		_hint_label.visible = false

# ── Helpers ───────────────────────────────────────────────────────────────────

func _get_player() -> Node2D:
	return get_tree().get_first_node_in_group("player") as Node2D
