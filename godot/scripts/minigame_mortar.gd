extends Control

signal minigame_finished(time_bonus: float)

const COMPRESSION_RATIO := 5.0 # 1 sec active grinding = 5 sec game time bonus
var herb_name: String = "Herb"
var start_progress: float = 0.0
var total_duration: float = 3600.0

var _progress: float = 0.0
var _pressing: bool  = false
var _done:     bool  = false
var _awarded_bonus: float = 0.0

@onready var _title: Label       = $Panel/VBox/Title
@onready var _bowl:  Control     = $Panel/VBox/Bowl
@onready var _bar:   ProgressBar = $Panel/VBox/Bar
@onready var _hint:  Label       = $Panel/VBox/Hint

func _ready() -> void:
	_title.text   = "Grinding: " + herb_name
	_bar.max_value = 100.0
	_progress      = start_progress
	_bar.value     = _progress * 100.0
	_hint.text     = "Hold to grind (adds quality & speed)"

func _process(delta: float) -> void:
	if _done:
		return

	if _pressing:
		var bonus = COMPRESSION_RATIO * delta
		_awarded_bonus += bonus
		
		var added_prog = bonus / total_duration
		_progress = minf(_progress + added_prog, 1.0)
		
		if _progress >= 1.0:
			_finish()
			return
	
	_bar.value = _progress * 100.0

	if _pressing and not _done:
		_bowl.position.x = randf_range(-1.5, 1.5)
	else:
		_bowl.position.x = 0.0

func _unhandled_input(event: InputEvent) -> void:
	if _done:
		return
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if not event.pressed and _pressing:
			_finish()
		_pressing = event.pressed
		get_viewport().set_input_as_handled()
	elif event is InputEventScreenTouch:
		if not event.pressed and _pressing:
			_finish()
		_pressing = event.pressed
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_cancel") and not _done:
		_finish()

func _finish() -> void:
	if _done:
		return
	_done            = true
	_pressing        = false
	_bowl.position.x = 0.0
	
	if _progress >= 1.0:
		_bar.value = 100.0
		_hint.text = "Process Complete ✓"
	else:
		_hint.text = "Session Ended"
		
	await get_tree().create_timer(1.0).timeout
	emit_signal("minigame_finished", _awarded_bonus)
