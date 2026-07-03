# ─────────────────────────────────────────────────────────────────────────────
# Player3D
# HD-2D character controller with WASD and click-to-move input,
# 4-directional sprite animation, and smooth camera follow.
# ─────────────────────────────────────────────────────────────────────────────
extends CharacterBody3D

signal interact(player_pos: Vector3)

# ─────────────────────────────────────────────────────────────────────────────
# Constants
# ─────────────────────────────────────────────────────────────────────────────
const SPEED := 3.0
const GRAVITY := 9.8
const CAMERA_OFFSET := Vector3(0, 4.2, 6.5)
const CAMERA_SMOOTH_FACTOR := 4.0
const CLICK_ARRIVE_THRESHOLD := 0.15

# ─────────────────────────────────────────────────────────────────────────────
# Node references
# ─────────────────────────────────────────────────────────────────────────────
@onready var sprite: AnimatedSprite3D = $AnimatedSprite3D
@onready var camera: Camera3D = $"../DioramaCamera"

# ─────────────────────────────────────────────────────────────────────────────
# State
# ─────────────────────────────────────────────────────────────────────────────
enum Facing { DOWN, UP, LEFT, RIGHT }

var _facing: Facing = Facing.DOWN
var _input_locked: bool = false
var _click_target: Vector3 = Vector3.INF  # INF = no active click target


# ─────────────────────────────────────────────────────────────────────────────
# Lifecycle
# ─────────────────────────────────────────────────────────────────────────────
func _ready() -> void:
	add_to_group("player")
	sprite.play("idle_down")


func _process(delta: float) -> void:
	# Smooth camera follow (camera is NOT a child of the player)
	if camera:
		var target_pos := global_position + CAMERA_OFFSET
		camera.global_position = camera.global_position.lerp(
			target_pos, 1.0 - exp(-delta * CAMERA_SMOOTH_FACTOR)
		)


func _physics_process(delta: float) -> void:
	# --- Gravity ---
	if not is_on_floor():
		velocity.y -= GRAVITY * delta

	# --- Input ---
	var move_dir := Vector3.ZERO

	if not _input_locked:
		# WASD input (overrides click-to-move)
		var input_vec := Vector2.ZERO
		if Input.is_action_pressed("ui_up"):
			input_vec.y -= 1.0
		if Input.is_action_pressed("ui_down"):
			input_vec.y += 1.0
		if Input.is_action_pressed("ui_left"):
			input_vec.x -= 1.0
		if Input.is_action_pressed("ui_right"):
			input_vec.x += 1.0

		if input_vec != Vector2.ZERO:
			# Keyboard input cancels any click target
			_click_target = Vector3.INF
			input_vec = input_vec.normalized()
			move_dir = Vector3(input_vec.x, 0.0, input_vec.y)
		elif _click_target != Vector3.INF:
			# Click-to-move
			var to_target := _click_target - global_position
			to_target.y = 0.0
			if to_target.length() < CLICK_ARRIVE_THRESHOLD:
				_click_target = Vector3.INF
			else:
				move_dir = to_target.normalized()

		# Interact
		if Input.is_action_just_pressed("interact"):
			interact.emit(global_position)

	# --- Apply movement ---
	if move_dir != Vector3.ZERO:
		velocity.x = move_dir.x * SPEED
		velocity.z = move_dir.z * SPEED
		_update_facing(move_dir)
		_play_walk_animation()
	else:
		velocity.x = move_toward(velocity.x, 0.0, SPEED)
		velocity.z = move_toward(velocity.z, 0.0, SPEED)
		_play_idle_animation()

	move_and_slide()


# ─────────────────────────────────────────────────────────────────────────────
# Input events
# ─────────────────────────────────────────────────────────────────────────────
func _unhandled_input(event: InputEvent) -> void:
	if _input_locked:
		return

	if event is InputEventMouseButton and event.pressed and event.button_index == MOUSE_BUTTON_LEFT:
		_handle_click(event.position)


func _handle_click(screen_pos: Vector2) -> void:
	if not camera:
		return

	var ray_origin := camera.project_ray_origin(screen_pos)
	var ray_dir := camera.project_ray_normal(screen_pos)

	# Intersect with Y = 0 plane
	if abs(ray_dir.y) < 0.001:
		return  # Ray is nearly parallel to the ground plane

	var t := -ray_origin.y / ray_dir.y
	if t < 0.0:
		return  # Intersection is behind the camera

	_click_target = ray_origin + ray_dir * t
	_click_target.y = 0.0


# ─────────────────────────────────────────────────────────────────────────────
# Facing & animation helpers
# ─────────────────────────────────────────────────────────────────────────────
func _update_facing(dir: Vector3) -> void:
	# Determine dominant axis
	if abs(dir.x) >= abs(dir.z):
		_facing = Facing.LEFT if dir.x < 0.0 else Facing.RIGHT
	else:
		_facing = Facing.UP if dir.z < 0.0 else Facing.DOWN


func _facing_suffix() -> String:
	match _facing:
		Facing.DOWN:  return "down"
		Facing.UP:    return "up"
		Facing.LEFT:  return "left"
		Facing.RIGHT: return "right"
	return "down"


func _play_walk_animation() -> void:
	var anim_name := "walk_" + _facing_suffix()
	if sprite.animation != anim_name:
		sprite.play(anim_name)


func _play_idle_animation() -> void:
	var anim_name := "idle_" + _facing_suffix()
	if sprite.animation != anim_name:
		sprite.play(anim_name)


# ─────────────────────────────────────────────────────────────────────────────
# Public API
# ─────────────────────────────────────────────────────────────────────────────
func set_input_locked(locked: bool) -> void:
	_input_locked = locked
	if locked:
		_click_target = Vector3.INF
		velocity.x = 0.0
		velocity.z = 0.0
		_play_idle_animation()
