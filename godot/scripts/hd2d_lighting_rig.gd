extends Node3D
class_name HD2DLightingRig

# ─────────────────────────────────────────────────────────────────────────────
# HD2DLightingRig
# Prototype-only lighting controller for the HD-2D pivot (Octopath-style:
# 3D world, Y-billboarded 4-directional sprites, real depth & dynamic light).
#
# Drives a DirectionalLight3D's color/energy and the scene's Environment
# ambient/fog tint from the SAME real-time planetary hour data already
# powering the ClockHUD and the main-menu orrery — so the world's lighting
# mood changes on the exact real-world planetary-hour cadence described in
# the GDD (§2.2 Atmospheric Overlays).
#
# STATUS: Standalone prototype. Not wired into main.tscn or cabin_main.tscn.
# Attach only inside hd2d_lighting_concept.tscn until the pivot is approved.
#
# Node references use @onready $Path (NOT @export). Godot 4 does not
# auto-resolve a hand-authored `foo = NodePath("../Bar")` line in a .tscn
# into an actual Node object for an @export var — that conversion only
# happens through the editor's inspector "assign" UI. $Path / get_node()
# always works regardless of how the scene was authored, and matches the
# convention used throughout the rest of this project's GDScript.
#
# Debug controls (this prototype scene only):
#   [P]  cycle through all 7 planets instantly, bypassing the real clock
#   [L]  return to Live — resume following the real-time planetary hour
#
# Planet color palette is reused verbatim from planet_orbit_animator.gd's
# PLANET_COLORS so the main-menu orrery and in-world lighting speak the
# same visual language.
# ─────────────────────────────────────────────────────────────────────────────

@onready var directional_light : DirectionalLight3D = $"../PlanetaryLight"
@onready var _world_env        : WorldEnvironment    = $"../WorldEnvironment"
@onready var debug_label       : Label               = $"../DebugHUD/Label"

# Real-time seconds to crossfade between planetary hours. Kept slow/dreamlike
# since in-fiction hours last 45–75 minutes — an instant snap would read as
# a bug, not a mood shift. (Debug [P] cycling still crossfades at this rate
# so you can watch the transition itself — hold off between presses.)
@export var transition_seconds : float = 8.0

# Chaldean order indices, matching PlanetaryHourCalculator.Planet:
# SATURN=0, JUPITER=1, MARS=2, SUN=3, VENUS=4, MERCURY=5, MOON=6
const PLANET_NAMES : Array[String] = ["Saturn","Jupiter","Mars","Sun","Venus","Mercury","Moon"]
const PLANET_GLYPHS : Array[String] = ["♄","♃","♂","☉","♀","☿","☽"]

# Reused verbatim from planet_orbit_animator.gd::PLANET_COLORS so the orrery
# and world lighting share one palette.
const PLANET_LIGHT_COLOR : Array[Color] = [
	Color(0.50, 0.00, 0.50),   # Saturn — Purple
	Color(0.25, 0.41, 0.88),   # Jupiter — Royal Blue
	Color(0.86, 0.08, 0.24),   # Mars — Crimson Red
	Color(1.00, 0.84, 0.00),   # Sun — Gold
	Color(0.31, 0.78, 0.47),   # Venus — Emerald Green
	Color(1.00, 0.65, 0.00),   # Mercury — Orange-Yellow
	Color(0.75, 0.75, 0.75),   # Moon — Silver
]

# Base light energy per planet — Sun brightest, Saturn/Moon dimmest.
# Day/night segment (see _poll_hour) multiplies this further.
const PLANET_LIGHT_ENERGY : Array[float] = [
	0.45,  # Saturn — leaden, muted
	0.85,  # Jupiter — indigo, moderate
	0.95,  # Mars — iron-red intensity
	1.40,  # Sun — brightest
	0.90,  # Venus — warm copper-green glow
	1.05,  # Mercury — quicksilver shimmer
	0.35,  # Moon — silvery, dim
]

var _current_color      : Color = Color.WHITE
var _current_energy     : float = 1.0
var _target_color       : Color = Color.WHITE
var _target_energy      : float = 1.0
var _current_planet_idx : int   = -1

# -1 = follow the live real-time planetary hour. 0-6 = forced debug override.
var _debug_planet_idx   : int   = -1

func _ready() -> void:
	if directional_light == null:
		push_warning("HD2DLightingRig: PlanetaryLight sibling not found.")
	if _world_env == null or _world_env.environment == null:
		push_warning("HD2DLightingRig: WorldEnvironment sibling or its Environment not found.")
	_poll_hour(true)

func _process(delta: float) -> void:
	# Poll once per second — matches clock_hud.gd's update cadence.
	if Engine.get_frames_drawn() % 60 == 0:
		_poll_hour(false)

	# Frame-rate-independent exponential smoothing toward the target mood.
	var t : float = 1.0 - exp(-delta / maxf(transition_seconds, 0.01))
	_current_color  = _current_color.lerp(_target_color, t)
	_current_energy = lerpf(_current_energy, _target_energy, t)

	if directional_light:
		directional_light.light_color  = _current_color
		directional_light.light_energy = _current_energy

	if _world_env and _world_env.environment:
		var env := _world_env.environment
		env.ambient_light_color  = _current_color
		env.ambient_light_energy = _current_energy * 0.35
		env.fog_light_color      = _current_color.lerp(Color.BLACK, 0.25)

func _unhandled_input(event: InputEvent) -> void:
	if not event is InputEventKey or not event.pressed or event.echo:
		return
	var key := event as InputEventKey
	if key.keycode == KEY_P:
		_debug_planet_idx = (_debug_planet_idx + 1) % 7
		_poll_hour(false)
		get_viewport().set_input_as_handled()
	elif key.keycode == KEY_L:
		_debug_planet_idx = -1
		_poll_hour(false)
		get_viewport().set_input_as_handled()

func _poll_hour(force_snap: bool) -> void:
	var planet_idx : int
	var is_day     : bool

	if _debug_planet_idx >= 0:
		# Debug override — bypass the real clock entirely.
		planet_idx = _debug_planet_idx
		is_day     = true
	else:
		if not is_instance_valid(GameManager):
			return
		var info : Dictionary = GameManager.get_planetary_hour()
		if info.is_empty():
			return
		planet_idx = clampi(int(info.get("ruling_planet", 3)), 0, 6)
		is_day     = int(info.get("hour_index", 0)) < 12

	var day_mult : float = 1.0 if is_day else 0.55
	_target_color  = PLANET_LIGHT_COLOR[planet_idx]
	_target_energy = PLANET_LIGHT_ENERGY[planet_idx] * day_mult

	if force_snap:
		_current_color  = _target_color
		_current_energy = _target_energy
	_current_planet_idx = planet_idx

	if debug_label:
		var mode_str : String = "DEBUG" if _debug_planet_idx >= 0 else ("Day" if is_day else "Night")
		debug_label.text = "%s %s Hour  ·  %s     [P] cycle planet   [L] live" % [
			PLANET_GLYPHS[planet_idx],
			PLANET_NAMES[planet_idx],
			mode_str
		]
