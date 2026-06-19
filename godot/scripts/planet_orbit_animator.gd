extends Control

# ─────────────────────────────────────────────────────────────────────────────
# PlanetOrbitAnimator
# Attach to the MainMenu root Control node.
#
# On _ready():
#   1. Calls PlanetaryOrbitCalculator.get_all_positions(now) once via C++
#      to get real geocentric ecliptic longitudes for all 7 planets.
#   2. Stores each planet's starting angle + mean daily motion.
#   3. Each frame, advances angles by (daily_motion / 86400) * delta_seconds
#      and repositions the planet TextureRect nodes around a visual orbit.
#
# Visual orbit radii are artistic (not true AU scale) so all seven planets
# are visible on screen simultaneously.
#
# Node structure expected under this Control:
#   Planets/Sun      (TextureRect or Sprite2D)
#   Planets/Moon     (TextureRect or Sprite2D)
#   Planets/Mercury  (TextureRect or Sprite2D)
#   Planets/Venus    (TextureRect or Sprite2D)
#   Planets/Mars     (TextureRect or Sprite2D)
#   Planets/Jupiter  (TextureRect or Sprite2D)
#   Planets/Saturn   (TextureRect or Sprite2D)
# ─────────────────────────────────────────────────────────────────────────────

# ── Visual orbit radii (fraction of the shorter viewport axis) ────────────────
# Geocentric (Ptolemaic) order: Earth is the observer at centre.
# Moon is closest, Saturn outermost. At runtime these are multiplied by
# min(size.x, size.y) / 2 so orbits scale with any viewport resolution.
const ORBIT_RADII = {
	"Moon":    0.22,
	"Mercury": 0.38,
	"Venus":   0.54,
	"Sun":     0.72,
	"Mars":    0.90,
	"Jupiter": 1.12,
	"Saturn":  1.35,
}

# Mean daily motions (degrees/day) — used as fallback if C++ unavailable.
# Values from Jean Meeus, Astronomical Algorithms.
const MEAN_DAILY_MOTION = {
	"Sun":      0.9856,
	"Moon":    13.1764,
	"Mercury":  4.0923,
	"Venus":    1.6021,
	"Mars":     0.5240,
	"Jupiter":  0.0831,
	"Saturn":   0.0335,
}

# Current ecliptic angle (degrees) for each planet, updated each frame
var _angles: Dictionary = {}

# Degrees per second of real time for each planet
var _deg_per_sec: Dictionary = {}

# Whether C++ backend loaded successfully
var _calc_ready: bool = false

# Reference to the calculator (keeps it alive)
var _calc = null

# Screen centre — recalculated every frame from the Control's actual size
var orbit_centre: Vector2 = Vector2.ZERO

# External links
const MAIN_SCENE = "res://scenes/main.tscn"
const URL_PATREON  = "https://www.patreon.com/Astrust"
const URL_GOFUNDME = "https://www.gofund.me/2265dd08b"

@onready var panel_ttao   : PanelContainer = $PanelTTAO
@onready var panel_credits: PanelContainer = $PanelCredits

# Original credits text (saved so toasts don't permanently overwrite it)
var _original_credits_text: String = ""

func _ready() -> void:
	panel_ttao.hide()
	panel_credits.hide()
	# Save original credits text
	_original_credits_text = $PanelCredits/VBox/CreditsLabel.text
	_init_positions()

func _init_positions() -> void:
	# Try to instantiate the C++ calculator
	if ClassDB.class_exists("PlanetaryOrbitCalculator"):
		_calc = PlanetaryOrbitCalculator.new()
		var now := int(Time.get_unix_time_from_system())
		var positions: Dictionary = _calc.get_all_positions(now)

		for planet in MEAN_DAILY_MOTION.keys():
			# Real ecliptic longitude as starting angle
			_angles[planet] = float(positions.get(planet, 0.0))
			# deg/day → deg/sec (static method — called on the class, not the instance)
			var daily: float = PlanetaryOrbitCalculator.get_mean_daily_motion(planet)
			_deg_per_sec[planet] = daily / 86400.0

		_calc_ready = true
		print("PlanetOrbitAnimator: real positions loaded.")
	else:
		# Fallback — start all at 0° and use mean motions only
		push_warning("PlanetOrbitAnimator: PlanetaryOrbitCalculator not found. Using mean motion fallback.")
		for planet in MEAN_DAILY_MOTION.keys():
			_angles[planet] = 0.0
			_deg_per_sec[planet] = MEAN_DAILY_MOTION[planet] / 86400.0

	_apply_positions()

func _process(delta: float) -> void:
	# Advance each planet angle by its real-time speed
	for planet in _angles.keys():
		_angles[planet] = fmod(_angles[planet] + _deg_per_sec[planet] * delta, 360.0)
	_apply_positions()

func _apply_positions() -> void:
	var planets_node := get_node_or_null("Planets")
	if planets_node == null:
		return

	# Recalculate centre and scale from the Control's current size so orbits
	# always stay centred on the gradient background, regardless of viewport.
	# Position centre at bottom third of the screen
	orbit_centre = Vector2(size.x / 2.0, size.y * 0.67)
	var scale_basis: float = min(size.x, size.y) / 2.0

	for planet in _angles.keys():
		var node := planets_node.get_node_or_null(planet)
		if node == null:
			continue

		# Node size for centering
		var node_size: Vector2 = Vector2.ZERO
		if node is Control:
			node_size = node.size
		elif node is Sprite2D:
			node_size = node.texture.get_size() if node.texture else Vector2.ZERO

		var radius_frac: float = ORBIT_RADII.get(planet, 0.35)
		var radius_px: float = radius_frac * scale_basis

		if radius_px <= 0.0:
			continue

		var angle_rad: float = deg_to_rad(_angles[planet])

		# Ecliptic longitude 0° = right (3 o'clock).
		# Godot's y-axis points down, so we negate y to get
		# counter-clockwise motion matching real sky motion.
		var offset := Vector2(
			cos(angle_rad) * radius_px,
			-sin(angle_rad) * radius_px
		)

		node.position = orbit_centre + offset - node_size * 0.5

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
	# Restore credits text before showing
	$PanelCredits/VBox/CreditsLabel.text = _original_credits_text
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
	# Show toast message
	var lbl := $PanelCredits/VBox/CreditsLabel
	lbl.text = message
	panel_ttao.hide()
	panel_credits.show()
