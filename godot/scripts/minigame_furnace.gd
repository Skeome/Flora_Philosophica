extends Control

# ─────────────────────────────────────────────────────────────────────────────
# MinigameFurnace
# Furnace / Calcination mini-game.
#
# The player taps the Bellows button to raise temperature.
# Heat decays naturally; remaining in the green zone awards compressed
# game-time at GAME_SECS_PER_REAL_SEC to 1 (120× by default).
#
# Failure state: exceeding EXPLOSION_MAX destroys the batch, matching the
# GDD rule — "Significant heat excess: No warning. Operation destroyed."
#
# Signal: minigame_finished(time_bonus: float)
#   time_bonus > 0   → game-seconds of processing time awarded
#   time_bonus == -1 → batch destroyed; station.award_time_bonus(-1) resets it
# ─────────────────────────────────────────────────────────────────────────────

signal minigame_finished(time_bonus: float)

const HEAT_PER_PUFF            := 18.0   # Heat added per bellows press (0–100)
const HEAT_DECAY               := 7.0    # Heat lost per real second
const GREEN_MIN                := 35.0   # Lower bound of safe zone
const GREEN_MAX                := 70.0   # Upper bound of safe zone
const DANGER_MIN               := 70.0   # Warning zone begins here
const EXPLOSION_MAX            := 100.0  # Instant batch destruction
const GAME_SECS_PER_REAL_SEC   := 120.0  # 1 real second in green = 2 game minutes
const MAX_SESSION_SEC          := 90.0   # Session auto-closes after this long

var process_name: String = "Calcination"

var _heat:    float = 15.0
var _awarded: float = 0.0   # game-seconds earned so far this session
var _elapsed: float = 0.0   # real seconds elapsed this session
var _done:    bool  = false
var _dead:    bool  = false

@onready var _title:   Label       = $Panel/VBox/Title
@onready var _flame:   ColorRect   = $Panel/VBox/Flame
@onready var _bar:     ProgressBar = $Panel/VBox/HeatBar
@onready var _status:  Label       = $Panel/VBox/Status
@onready var _tlabel:  Label       = $Panel/VBox/TimeLabel
@onready var _bellows: Button      = $Panel/VBox/Bellows

# ─────────────────────────────────────────────────────────────────────────────

func _ready() -> void:
	_title.text    = process_name
	_bar.max_value = 100.0
	_bar.min_value = 0.0
	_bellows.pressed.connect(_on_puff)
	_refresh()

func _process(delta: float) -> void:
	if _done or _dead:
		return

	_elapsed += delta
	_heat     = maxf(_heat - HEAT_DECAY * delta, 0.0)

	if _heat >= GREEN_MIN and _heat <= GREEN_MAX:
		_awarded += GAME_SECS_PER_REAL_SEC * delta

	if _heat >= EXPLOSION_MAX:
		_explode()
		return

	if _elapsed >= MAX_SESSION_SEC:
		_finish()
		return

	_refresh()

func _on_puff() -> void:
	if _done or _dead:
		return
	_heat = minf(_heat + HEAT_PER_PUFF, 100.0)
	_refresh()
	if _heat >= EXPLOSION_MAX:
		_explode()

func _refresh() -> void:
	_bar.value = _heat

	# Colour-coded status
	if _heat < GREEN_MIN:
		_status.text        = "Too cold — pump the bellows!"
		_bar.modulate       = Color(0.4, 0.5, 1.0)
	elif _heat <= GREEN_MAX:
		_status.text        = "Perfect heat ✓ — processing"
		_bar.modulate       = Color(0.2, 1.0, 0.3)
	elif _heat < EXPLOSION_MAX:
		_status.text        = "⚠ Getting too hot — ease off!"
		_bar.modulate       = Color(1.0, 0.45, 0.0)
	else:
		_status.text        = "💥 OVERHEATING"
		_bar.modulate       = Color(1.0, 0.0, 0.0)

	var game_mins := int(_awarded / 60.0)
	_tlabel.text = "Time processed: %d min" % game_mins

	# Flame colour shifts cool→warm with temperature
	var t := clampf(_heat / 100.0, 0.0, 1.0)
	_flame.color = Color(1.0, lerpf(0.05, 0.55, t), 0.0, lerpf(0.1, 0.9, t))

func _explode() -> void:
	if _dead:
		return
	_dead           = true
	_bellows.disabled = true
	_status.text    = "💥  Overheated!  Batch destroyed."
	_bar.modulate   = Color(1.0, 0.0, 0.0)
	_flame.color    = Color(1.0, 0.0, 0.0, 1.0)
	await get_tree().create_timer(2.0).timeout
	emit_signal("minigame_finished", -1.0)

func _finish() -> void:
	if _done:
		return
	_done             = true
	_bellows.disabled = true
	var mins := int(_awarded / 60.0)
	_status.text = "Session complete — +%d min processed" % mins
	await get_tree().create_timer(1.0).timeout
	emit_signal("minigame_finished", _awarded)
