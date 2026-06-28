extends Control

signal minigame_finished(time_bonus: float)

const TEMP_MAX            := 600.0
const TEMP_START          := 20.0
const TARGET_RAMP_DEG_MIN := 4.0   # 4 degrees per minute
const TARGET_RAMP_DEG_SEC := TARGET_RAMP_DEG_MIN / 60.0
const HEAT_DECAY          := 5.0    # temp loss per sec
const HEAT_PER_PUFF       := 15.0

var process_name: String = "Calcination"
var start_progress: float = 0.0
var total_duration: float = 9000.0 # 150 minutes

var _temp: float = TEMP_START
var _target_temp: float = TEMP_START
var _elapsed: float = 0.0
var _awarded_bonus: float = 0.0
var _done: bool = false
var _dead: bool = false

@onready var _title:   Label       = $Panel/VBox/Title
@onready var _flame:   ColorRect   = $Panel/VBox/Flame
@onready var _bar:     ProgressBar = $Panel/VBox/HeatBar
@onready var _status:  Label       = $Panel/VBox/Status
@onready var _tlabel:  Label       = $Panel/VBox/TimeLabel
@onready var _bellows: Button      = $Panel/VBox/Bellows

func _ready() -> void:
	_title.text    = process_name
	_bar.max_value = TEMP_MAX
	_bar.min_value = 0.0
	_bellows.pressed.connect(_on_puff)
	
	_target_temp = TEMP_START + (start_progress * (TEMP_MAX - TEMP_START))
	_temp = _target_temp
	_refresh()

func _process(delta: float) -> void:
	if _done or _dead:
		return
		
	_elapsed += delta
	_awarded_bonus += delta 
	
	_target_temp = minf(_target_temp + (TARGET_RAMP_DEG_SEC * delta), TEMP_MAX)
	_temp = maxf(_temp - (HEAT_DECAY * delta), TEMP_START)
	
	if _temp >= TEMP_MAX:
		_explode()
		return
		
	var progress = start_progress + (_elapsed / total_duration)
	if progress >= 1.0:
		_finish_success()
		return

	_refresh()

func _on_puff() -> void:
	if _done or _dead:
		return
	_temp += HEAT_PER_PUFF
	if _temp >= TEMP_MAX:
		_explode()
	else:
		_refresh()

func _refresh() -> void:
	_bar.value = _temp
	
	var diff = _temp - _target_temp
	
	if diff < -20.0:
		_status.text  = "Too cold — pump the bellows!"
		_bar.modulate = Color(0.4, 0.5, 1.0)
	elif diff <= 20.0:
		_status.text  = "On target ✓"
		_bar.modulate = Color(0.2, 1.0, 0.3)
	elif _temp < TEMP_MAX:
		_status.text  = "⚠ Heating too fast!"
		_bar.modulate = Color(1.0, 0.45, 0.0)
		
	_tlabel.text = "Target: %.1f°C | Actual: %.1f°C" % [_target_temp, _temp]

	var t := clampf(_temp / TEMP_MAX, 0.0, 1.0)
	_flame.color = Color(1.0, lerpf(0.05, 0.55, t), 0.0, lerpf(0.1, 0.9, t))

func _explode() -> void:
	if _dead:
		return
	_dead = true
	_bellows.disabled = true
	_status.text = "💥 Vitrified! Batch destroyed."
	_bar.modulate = Color(1.0, 0.0, 0.0)
	_flame.color = Color(1.0, 0.0, 0.0, 1.0)
	await get_tree().create_timer(2.0).timeout
	emit_signal("minigame_finished", -1.0)

func _finish_success() -> void:
	if _done:
		return
	_done = true
	_bellows.disabled = true
	_status.text = "Process complete ✓"
	await get_tree().create_timer(1.0).timeout
	emit_signal("minigame_finished", _awarded_bonus)

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel") and not _done and not _dead:
		_done = true
		_status.text = "Session Ended"
		emit_signal("minigame_finished", _awarded_bonus)
