extends CanvasLayer

# ─────────────────────────────────────────────────────────────────────────────
# ClockHUD
# Displays the current planetary hour in the top-left corner.
# Attach to a CanvasLayer node. Add a Label child named "ClockLabel".
#
# Node structure:
#   ClockHUD (CanvasLayer)   ← this script
#   └── ClockLabel (Label)
# ─────────────────────────────────────────────────────────────────────────────

@onready var clock_label: Label = $ClockLabel

# Planet glyphs using Unicode — rendered with Archemy font later
const PLANET_GLYPHS = {
	0: "♄",  # Saturn
	1: "♃",  # Jupiter
	2: "♂",  # Mars
	3: "☉",  # Sun
	4: "♀",  # Venus
	5: "☿",  # Mercury
	6: "☽",  # Moon
}

func _ready() -> void:
	_update_clock()

func _process(_delta: float) -> void:
	# Update once per second is enough
	if Engine.get_frames_drawn() % 60 == 0:
		_update_clock()

func _update_clock() -> void:
	var info: Dictionary = GameManager.get_planetary_hour()
	if info.is_empty():
		return

	var glyph: String = PLANET_GLYPHS.get(info["ruling_planet"], "?")
	var segment: String = "Day" if info["hour_index"] < 12 else "Night"
	var day_glyph: String = PLANET_GLYPHS.get(info["day_ruler"], "?")
	var mins: int = int(info["minutes_remaining"])

	clock_label.text = "%s  %s Hour\n%s %s\n%d min remaining" % [
		glyph,
		info["planet_name"],
		day_glyph,
		segment,
		mins
	]
