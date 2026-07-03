extends Sprite3D
class_name BillboardSprite

# ─────────────────────────────────────────────────────────────────────────────
# BillboardSprite
# Utility for 4-directional sprite selection based on camera angle.
# Attach to any Sprite3D that needs direction-dependent textures.
# Plants use a single texture — this is for NPCs and static characters.
# The player uses AnimatedSprite3D with its own facing logic instead.
# ─────────────────────────────────────────────────────────────────────────────

@export var texture_down: Texture2D
@export var texture_up: Texture2D
@export var texture_left: Texture2D
@export var texture_right: Texture2D

func _process(_delta: float) -> void:
	var cam = get_viewport().get_camera_3d()
	if cam == null:
		return
	var to_cam = cam.global_position - global_position
	to_cam.y = 0
	if to_cam.length_squared() < 0.001:
		return
	var angle = atan2(to_cam.x, to_cam.z)
	if angle > -PI / 4 and angle <= PI / 4:
		if texture_down: texture = texture_down
	elif angle > PI / 4 and angle <= 3 * PI / 4:
		if texture_right: texture = texture_right
	elif angle > -3 * PI / 4 and angle <= -PI / 4:
		if texture_left: texture = texture_left
	else:
		if texture_up: texture = texture_up
