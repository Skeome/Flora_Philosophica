extends Control

# ─────────────────────────────────────────────────────────────────────────────
# PlanetOrbitAnimator
# Attach to the MainMenu root Control node.
#
# Drives the "dance of the spheres" — true geocentric apparent motion for
# Mercury, Venus, Mars, Jupiter, and Saturn, including real retrograde loops.
# Sun and Moon use simplified models (the Sun never retrogrades from Earth's
# perspective; the Moon's loop pattern is dominated by monthly motion, not a
# classic planetary retrograde).
#
# On _ready():
#   1. Samples real geocentric ecliptic longitude/latitude at the 2000-01-01
#      epoch and at "now" via the C++ PlanetaryOrbitCalculator.
#   2. Fast-forwards from epoch to now over FAST_FORWARD_DURATION seconds by
#      re-sampling the real calculator at proportionally advancing timestamps
#      each frame — NOT a synthetic circular sweep — so the genuine
#      prograde/retrograde loop shapes appear during the fast-forward.
#   3. After the duration, continues sampling at real wall-clock time, so the
#      animation seamlessly becomes "live."
#
# Player location (GameManager.observer_lat/observer_lon) is passed through
# to the calculator on every sample, so the displayed sky is keyed to the
# actual player rather than a hardcoded location.
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

const PLANET_NAMES := ["Sun", "Moon", "Mercury", "Venus", "Mars", "Jupiter", "Saturn"]

# ── Visual scale ──────────────────────────────────────────────────────────────
# Geocentric longitude/latitude are projected as a simple polar mapping
# around an "Earth" point: angle = longitude, radius = base_radius scaled by
# a mild per-planet multiplier (so retrograde loops are visible at a size
# that reads on screen — true relative distances would make Saturn's loop
# imperceptibly small next to Mercury's).
#
# The Sun's geocentric longitude is, by construction, Earth's heliocentric
# longitude + 180° — a single smooth annual sweep with no latitude wobble
# and no possibility of retrograde (the Sun can never appear to reverse
# direction as seen from Earth). It therefore traces a perfect circle here,
# distinct from the looping planets.
# Ordered by the Chaldean sequence used throughout the rest of the game
# (clock.cpp day-ruler order): Saturn, Jupiter, Mars, Sun, Venus, Mercury, Moon.
# Saturn outermost, Moon innermost — the Sun sits in the middle of that
# sequence, not bundled with the inner planets.
const RADIUS_SCALE := {
	"Moon":    0.22,
	"Mercury": 0.38,
	"Venus":   0.54,
	"Sun":     0.72,
	"Mars":    0.90,
	"Jupiter": 1.12,
	"Saturn":  1.35,
}

const PLANET_COLORS := {
	"Sun": Color(1.0, 0.84, 0.0),      # Gold
	"Moon": Color(0.75, 0.75, 0.75),   # Silver
	"Venus": Color(0.31, 0.78, 0.47),  # Emerald Green
	"Mars": Color(0.86, 0.08, 0.24),   # Crimson Red
	"Jupiter": Color(0.25, 0.41, 0.88),# Royal Blue
	"Saturn": Color(0.5, 0.0, 0.5),    # Purple
	"Mercury": Color(1.0, 0.65, 0.0),  # Orange-Yellow
}

# Latitude is amplified visually — real ecliptic latitudes are only a few
# degrees, which would be an imperceptible wobble at the radii above. This
# multiplier exaggerates the loop's vertical extent for legibility while
# preserving its true timing and direction. Per-planet values prevent inner
# planets (like the Moon) from wildly overlapping the Chaldean order.
const LATITUDE_VISUAL_GAIN := {
	"Moon":    2.0,
	"Mercury": 8.0,
	"Venus":   10.0,
	"Sun":     0.0,
	"Mars":    12.0,
	"Jupiter": 16.0,
	"Saturn":  18.0,
}

const EPOCH_UNIX_TIMESTAMP: int = 946684800  # 2000-01-01 00:00:00 UTC
const FAST_FORWARD_DURATION: float = 60.0    # seconds

var _calc = null
var _calc_ready: bool = false

var _ff_elapsed: float = 0.0
var _ff_active: bool = true
var _ff_start_ts: int = EPOCH_UNIX_TIMESTAMP
var _ff_end_ts: int = EPOCH_UNIX_TIMESTAMP

# Cached last-sampled position per planet: { "lon": float, "lat": float }
var _positions: Dictionary = {}

const MAX_TRAIL_LENGTH: int = 1500
var _trails: Dictionary = {}
var _last_sample_ts: int = 0

@export var orbit_centre: Vector2 = Vector2.ZERO

# ── External links ────────────────────────────────────────────────────────────
const MAIN_SCENE = "res://scenes/main.tscn"
const URL_PATREON  = "https://www.patreon.com/"
const URL_GOFUNDME = "https://www.gofundme.com/"

@onready var panel_ttao   : PanelContainer = $PanelTTAO
@onready var panel_credits: PanelContainer = $PanelCredits

var original_credits_text: String = ""

func _ready() -> void:
	panel_ttao.hide()
	panel_credits.hide()
	original_credits_text = $PanelCredits/VBox/CreditsLabel.text
	$Planets.draw.connect(_on_planets_draw)
	_ff_end_ts = int(Time.get_unix_time_from_system())
	_ff_start_ts = EPOCH_UNIX_TIMESTAMP
	_last_sample_ts = EPOCH_UNIX_TIMESTAMP
	_ff_active = true
	_ff_elapsed = 0.0

	if ClassDB.class_exists("PlanetaryOrbitCalculator"):
		_calc = PlanetaryOrbitCalculator.new()
		_calc_ready = true
	else:
		push_warning("PlanetOrbitAnimator: PlanetaryOrbitCalculator not found.")
		_calc_ready = false

	_sample_positions(_ff_start_ts)
	_apply_positions()

func _process(delta: float) -> void:
	if not _calc_ready:
		return

	var target_ts: int

	if _ff_active:
		_ff_elapsed += delta
		var t: float = clampf(_ff_elapsed / FAST_FORWARD_DURATION, 0.0, 1.0)
		# Ease-out so the sweep settles smoothly into real-time speed.
		var eased_t: float = 1.0 - pow(1.0 - t, 3.0)
		target_ts = _ff_start_ts + int(float(_ff_end_ts - _ff_start_ts) * eased_t)

		if t >= 1.0:
			_ff_active = false
	else:
		target_ts = int(Time.get_unix_time_from_system())

	var time_step: int = 86400  # 24-hour physical steps
	if target_ts > _last_sample_ts:
		while _last_sample_ts + time_step < target_ts:
			_last_sample_ts += time_step
			_sample_positions(_last_sample_ts)
			_apply_positions()
		
		_last_sample_ts = target_ts
		_sample_positions(_last_sample_ts)
		_apply_positions()
	elif target_ts < _last_sample_ts:
		_last_sample_ts = target_ts
		_sample_positions(_last_sample_ts)
		_apply_positions()
	var planets_node := get_node_or_null("Planets")
	if planets_node:
		planets_node.queue_redraw()

func _sample_positions(unix_ts: int) -> void:
	if not _calc_ready:
		return
	var lat: float = GameManager.observer_lat if GameManager else 0.0
	var lon: float = GameManager.observer_lon if GameManager else 0.0
	_positions = _calc.get_all_geocentric_positions(unix_ts, lat, lon)

func _apply_positions() -> void:
	var planets_node := get_node_or_null("Planets")
	if planets_node == null:
		return

	var scale_basis: float = min(size.x, size.y) / 2.0
	if orbit_centre == Vector2.ZERO:
		orbit_centre = Vector2(size.x / 2.0, size.y * 0.67)

	for planet_name in PLANET_NAMES:
		var node := planets_node.get_node_or_null(planet_name)
		if node == null:
			continue

		var radius_frac: float = RADIUS_SCALE.get(planet_name, 0.5)
		if not _positions.has(planet_name):
			continue

		var pos: Dictionary = _positions[planet_name]
		var lon_deg: float = float(pos.get("lon", 0.0))
		var lat_deg: float = float(pos.get("lat", 0.0))

		var lon_rad: float = deg_to_rad(lon_deg)
		var radius_px: float = radius_frac * scale_basis

		# Base orbital position from longitude (the "ring")
		var base_offset := Vector2(
			cos(lon_rad) * radius_px,
			-sin(lon_rad) * radius_px
		)

		# Latitude perturbs the radial distance slightly, producing the
		# visible loop-within-a-ring shape characteristic of true apparent
		# retrograde motion, exaggerated by LATITUDE_VISUAL_GAIN for legibility.
		var offset: Vector2 = base_offset
		var lat_gain: float = LATITUDE_VISUAL_GAIN.get(planet_name, 0.0)
		if lat_gain > 0.0:
			var lat_offset_px: float = lat_deg * lat_gain
			var radial_dir: Vector2 = base_offset.normalized()
			offset = base_offset + radial_dir * lat_offset_px

		var node_size: Vector2 = Vector2.ZERO
		if node is Control:
			node_size = node.size
		elif node is Sprite2D:
			node_size = node.texture.get_size() if node.texture else Vector2.ZERO

		var center_pos: Vector2 = orbit_centre + offset
		node.position = center_pos - node_size * 0.5
		
		if not _trails.has(planet_name):
			_trails[planet_name] = PackedVector2Array()
			_trails[planet_name].append(center_pos)
		else:
			var trail: PackedVector2Array = _trails[planet_name]
			if trail.size() > 0:
				if trail[trail.size() - 1].distance_squared_to(center_pos) > 4.0:
					trail.append(center_pos)
			else:
				trail.append(center_pos)
				
			if trail.size() > MAX_TRAIL_LENGTH:
				var excess: int = trail.size() - MAX_TRAIL_LENGTH
				trail = trail.slice(excess)
			_trails[planet_name] = trail

func _on_planets_draw() -> void:
	var planets_node := get_node_or_null("Planets")
	if not planets_node: return
	
	for planet_name in PLANET_NAMES:
		if not _trails.has(planet_name):
			continue
		var trail: PackedVector2Array = _trails[planet_name]
		var count: int = trail.size()
		if count < 2:
			continue
			
		var base_color: Color = PLANET_COLORS.get(planet_name, Color.WHITE)
		var colors := PackedColorArray()
		colors.resize(count)
		
		for i in range(count):
			var alpha: float = float(i) / float(count - 1)
			var c = base_color
			c.a = alpha * 0.8
			colors[i] = c
			
		planets_node.draw_polyline_colors(trail, colors, 2.0, true)

# ── Button callbacks ──────────────────────────────────────────────────────────

func _on_btn_new_game_pressed() -> void:
	GameManager.save_game()
	get_tree().change_scene_to_file(MAIN_SCENE)

func _on_btn_load_game_pressed() -> void:
	GameManager.load_game()
	get_tree().change_scene_to_file(MAIN_SCENE)

func _on_btn_online_play_pressed() -> void:
	_show_toast("Online Play is coming in a future update.")

func _on_btn_exit_game_pressed() -> void:
	GameManager.save_game()
	get_tree().quit()

func _on_btn_settings_pressed() -> void:
	_show_toast("Settings coming soon.")

func _on_btn_ttao_pressed() -> void:
	panel_credits.hide()
	panel_ttao.visible = not panel_ttao.visible

func _on_btn_about_pressed() -> void:
	panel_ttao.hide()
	$PanelCredits/VBox/CreditsLabel.text = original_credits_text
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
	var lbl := $PanelCredits/VBox/CreditsLabel
	lbl.text = message
	panel_ttao.hide()
	panel_credits.show()
