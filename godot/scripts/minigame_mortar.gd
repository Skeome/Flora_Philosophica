extends Control

# ─────────────────────────────────────────────────────────────────────────────
# MinigameMortar
# Mortar & Pestle mini-game.
#
# The player holds the mouse button (or keeps a finger pressed) to grind.
# Progress fills over GRIND_DURATION seconds of continuous pressure.
# Releasing causes mild regression so the player must sustain the effort.
#
# Because PlacedItem treats M&P as instant (process_duration_sec = 0),
# this mini-game IS the transformation — the herb is only ground once the
# player completes it. No time bonus is emitted; the station handles the
# immediate unload on its next interact() call.
#
# Signal: minigame_finished(time_bonus: float)  — always 0.0 for M&P.
# ─────────────────────────────────────────────────────────────────────────────

signal minigame_finished(time_bonus: float)

const GRIND_DURATION := 4.0   # Seconds of sustained holding to finish
const DECAY_SPEED    := 25.0  # Progress units lost per second when released

var herb_name: String = "Herb"

var _progress: float = 0.0
var _pressing: bool  = false
var _done:     bool  = false

@onready var _title: Label       = $Panel/VBox/Title
@onready var _bowl:  Control     = $Panel/VBox/Bowl
@onready var _bar:   ProgressBar = $Panel/VBox/Bar
@onready var _hint:  Label       = $Panel/VBox/Hint

# ─────────────────────────────────────────────────────────────────────────────

func _ready() -> void:
	_title.text   = "Grinding: " + herb_name
	_bar.max_value = 100.0
	_bar.value     = 0.0
	_hint.text     = "Hold to grind"

func _process(delta: float) -> void:
	if _done:
		return

	if _pressing:
		_progress = minf(_progress + (100.0 / GRIND_DURATION) * delta, 100.0)
		if _progress >= 100.0:
			_finish()
			return
	else:
		_progress = maxf(_progress - DECAY_SPEED * delta, 0.0)

	_bar.value = _progress

	# Subtle bowl shake while grinding
	if _pressing and not _done:
		_bowl.position.x = randf_range(-1.5, 1.5)
	else:
		_bowl.position.x = 0.0

func _unhandled_input(event: InputEvent) -> void:
	if _done:
		return
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		_pressing = event.pressed
		get_viewport().set_input_as_handled()
	elif event is InputEventScreenTouch:
		_pressing = event.pressed
		get_viewport().set_input_as_handled()

func _finish() -> void:
	if _done:
		return
	_done            = true
	_pressing        = false
	_bowl.position.x = 0.0
	_bar.value       = 100.0
	_hint.text       = "Done ✓"
	await get_tree().create_timer(0.6).timeout
	emit_signal("minigame_finished", 0.0)
