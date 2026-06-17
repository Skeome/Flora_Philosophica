extends CharacterBody2D

const SPEED = 80.0  # native 16px units per second

@onready var sprite: AnimatedSprite2D = $AnimatedSprite2D

var _move_target: Vector2 = Vector2.ZERO
var _has_target: bool = false
var _facing: String = "down"

func _ready() -> void:
	add_to_group("player")

func _physics_process(_delta: float) -> void:
	var direction := _get_keyboard_direction()

	if direction != Vector2.ZERO:
		_has_target = false
		velocity = direction.normalized() * SPEED
		_update_facing(direction)
		sprite.play("walk_" + _facing)
	elif _has_target:
		var to_target := _move_target - global_position
		if to_target.length() < 2.0:
			_has_target = false
			velocity = Vector2.ZERO
			sprite.play("idle_" + _facing)
		else:
			velocity = to_target.normalized() * SPEED
			_update_facing(to_target)
			sprite.play("walk_" + _facing)
	else:
		velocity = Vector2.ZERO
		sprite.play("idle_" + _facing)

	move_and_slide()

func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT and event.pressed:
			_move_target = get_global_mouse_position()
			_has_target = true
	if event is InputEventKey and event.pressed and not event.echo:
		if event.keycode == KEY_E:
			interact.emit(global_position)

func _get_keyboard_direction() -> Vector2:
	var dir := Vector2.ZERO
	if Input.is_key_pressed(KEY_W) or Input.is_key_pressed(KEY_UP):    dir.y -= 1
	if Input.is_key_pressed(KEY_S) or Input.is_key_pressed(KEY_DOWN):  dir.y += 1
	if Input.is_key_pressed(KEY_A) or Input.is_key_pressed(KEY_LEFT):  dir.x -= 1
	if Input.is_key_pressed(KEY_D) or Input.is_key_pressed(KEY_RIGHT): dir.x += 1
	return dir

func _update_facing(direction: Vector2) -> void:
	if abs(direction.x) > abs(direction.y):
		_facing = "right" if direction.x > 0 else "left"
	else:
		_facing = "down" if direction.y > 0 else "up"

signal interact(player_pos: Vector2)
