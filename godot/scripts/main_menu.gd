extends Control

# ─────────────────────────────────────────────────────────────────────────────
# MainMenu
# Composites three PNG layers (gradient, planets, UI sheet) and places
# invisible Button nodes over each wood button in the UI sheet art.
#
# All positions are expressed as fractions of the viewport size (1920×1080)
# so the menu scales correctly under any window/stretch setting.
# ─────────────────────────────────────────────────────────────────────────────

const MAIN_SCENE = "res://scenes/main.tscn"

# External links
const URL_PATREON  = "https://www.patreon.com/Astrust"
const URL_GOFUNDME = "https://www.gofund.me/2265dd08b"

@onready var planets_rect : TextureRect = $Planets
@onready var panel_ttao   : PanelContainer = $PanelTTAO
@onready var panel_credits: PanelContainer = $PanelCredits

func _ready() -> void:
	panel_ttao.hide()
	panel_credits.hide()
	# Subtle planet rotation — each planet TextureRect gets a slow spin
	# via shader once shaders are added. Nothing to wire up here yet.

# ── Button callbacks ──────────────────────────────────────────────────────────

func _on_btn_new_game_pressed() -> void:
	# TODO: show character/world name prompt before transitioning
	GameManager.save_game()
	get_tree().change_scene_to_file(MAIN_SCENE)

func _on_btn_load_game_pressed() -> void:
	GameManager.load_game()
	get_tree().change_scene_to_file(MAIN_SCENE)

func _on_btn_online_play_pressed() -> void:
	# Multiplayer is a future feature — inform the player
	_show_toast("Online Play is coming in a future update.")

func _on_btn_exit_game_pressed() -> void:
	GameManager.save_game()
	get_tree().quit()

func _on_btn_settings_pressed() -> void:
	# TODO: open settings panel (audio, display, GPS override)
	_show_toast("Settings coming soon.")

func _on_btn_ttao_pressed() -> void:
	panel_credits.hide()
	panel_ttao.visible = not panel_ttao.visible

func _on_btn_about_pressed() -> void:
	panel_ttao.hide()
	panel_credits.visible = not panel_credits.visible

func _on_btn_patreon_pressed() -> void:
	OS.shell_open(URL_PATREON)

func _on_btn_gofundme_pressed() -> void:
	OS.shell_open(URL_GOFUNDME)

func _on_btn_close_ttao_pressed() -> void:
	panel_ttao.hide()

func _on_btn_close_credits_pressed() -> void:
	panel_credits.hide()

# ── Helpers ───────────────────────────────────────────────────────────────────

func _show_toast(message: String) -> void:
	# Reuse the credits panel as a lightweight toast for now
	var lbl := $PanelCredits/VBox/CreditsLabel
	lbl.text = message
	panel_ttao.hide()
	panel_credits.show()
