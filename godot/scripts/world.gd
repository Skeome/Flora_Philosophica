extends Node2D

# ─────────────────────────────────────────────────────────────────────────────
# World
# Root script for the exterior map scene.
# Owns the transition zone system — Area2D nodes named "Door_*" placed in the
# editor trigger scene changes when the player enters them.
#
# Convention for door node names:
#   Door_CabinMain    → loads scenes/cabin_main.tscn
#   Door_Garden       → loads scenes/garden.tscn
#
# Each Door_* node must have:
#   - An Area2D with a CollisionShape2D (rectangle over the doorway)
#   - A metadata key "target_scene" (String)  e.g. "res://scenes/cabin_main.tscn"
#   - A metadata key "spawn_point" (Vector2)  where the player appears on arrival
# ─────────────────────────────────────────────────────────────────────────────

func _ready() -> void:
	# Connect all Door_* Area2D nodes found in this scene
	for child in get_children():
		if child is Area2D and child.name.begins_with("Door_"):
			child.body_entered.connect(_on_door_entered.bind(child))

func _on_door_entered(body: Node2D, door: Area2D) -> void:
	if not body.is_in_group("player"):
		return

	var target: String = door.get_meta("target_scene", "")
	var spawn: Vector2  = door.get_meta("spawn_point", Vector2.ZERO)

	if target == "":
		push_warning("Door '%s' has no target_scene metadata." % door.name)
		return

	# Save before transitioning
	GameManager.save_game()

	# Store spawn point so the target scene knows where to place the player
	GameManager.pending_spawn = spawn

	get_tree().change_scene_to_file(target)
