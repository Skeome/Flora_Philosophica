extends CanvasLayer

# ─────────────────────────────────────────────────────────────────────────────
# PauseMenu
# Self-contained menu overlay. Add as a child of the World scene (main.tscn)
# so it's present whenever the player is in the game world.
#
# Toggled by the built-in "ui_cancel" action (Escape by default).
#
# IMPORTANT: this does NOT pause the SceneTree. Flora Philosophica's planetary
# clock and laboratory timers are tied to real-world time and are designed to
# keep running whether or not the player has this menu open — pausing the
# tree would be inconsistent with that design. This overlay only blocks game
# input while open; the world keeps simulating underneath it.
#
# Save/Load reuses GameManager.save_game() / load_game() — the same calls
# used by room transitions — so this is a direct test of that system.
# ─────────────────────────────────────────────────────────────────────────────

const MAIN_MENU_SCENE = "res://scenes/main_menu.tscn"

@onready var status_label: Label = $Panel/VBox/SaveStatus
@onready var dim: ColorRect = $Dim
@onready var panel: PanelContainer = $Panel

var _status_timer: float = 0.0

func _ready() -> void:
	visible = false
	status_label.text = ""
	# Block clicks from passing through to the world while open.
	dim.mouse_filter = Control.MOUSE_FILTER_STOP
	panel.mouse_filter = Control.MOUSE_FILTER_STOP

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel"):
		_toggle_menu()
		get_viewport().set_input_as_handled()

func _process(delta: float) -> void:
	if _status_timer > 0.0:
		_status_timer -= delta
		if _status_timer <= 0.0:
			status_label.text = ""

func _toggle_menu() -> void:
	visible = not visible
	# Tell the player script to stop responding to movement input while
	# the menu is open, without touching SceneTree.paused.
	var player := get_tree().get_first_node_in_group("player")
	if player and player.has_method("set_input_locked"):
		player.set_input_locked(visible)

func _on_btn_resume_pressed() -> void:
	_toggle_menu()

func _on_btn_save_pressed() -> void:
	GameManager.save_game()
	_show_status("Game saved.")

func _on_btn_load_pressed() -> void:
	GameManager.load_game()
	_show_status("Game loaded.")
	# Reposition the player to the loaded save position.
	# GameManager.load_game() restores inventory/rooms; player position
	# restoration happens via the player reading GameManager state on
	# re-entry, or — for an in-place load — by re-applying saved position
	# directly here if GameManager exposes it.
	var player := get_tree().get_first_node_in_group("player")
	if player and GameManager.has_method("get_saved_player_position"):
		player.global_position = GameManager.get_saved_player_position()

func _on_btn_settings_pressed() -> void:
	_show_status("Settings coming soon.")

func _on_btn_main_menu_pressed() -> void:
	GameManager.save_game()
	get_tree().change_scene_to_file(MAIN_MENU_SCENE)

func _show_status(message: String) -> void:
	status_label.text = message
	_status_timer = 2.0
