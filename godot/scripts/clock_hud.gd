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

@onready var panel_schedule: PanelContainer = $PanelSchedule
@onready var schedule_list: VBoxContainer = $PanelSchedule/VBox/ScrollContainer/ScheduleList

func get_daily_schedule() -> Array:
	var schedule = []
	var info = GameManager.get_planetary_hour()
	if info.is_empty(): return schedule
	
	var lat = GameManager.observer_lat
	var lon = GameManager.observer_lon
	
	var query_time = int(Time.get_unix_time_from_system())
	var current_info = info
	while current_info["hour_index"] > 0:
		query_time = int(current_info["hour_start_utc"]) - 10
		current_info = GameManager.clock.calculate_planetary_hour(lat, lon, query_time)
		
	query_time = int(current_info["hour_start_utc"]) + 10
	for i in range(24):
		var h_info = GameManager.clock.calculate_planetary_hour(lat, lon, query_time)
		schedule.append(h_info)
		query_time = int(h_info["hour_end_utc"]) + 10
		
	return schedule

func _on_btn_schedule_pressed() -> void:
	for child in schedule_list.get_children():
		child.queue_free()
		
	var schedule = get_daily_schedule()
	var tz_offset = Time.get_time_zone_from_system()["bias"] * 60
	
	for i in range(schedule.size()):
		var info = schedule[i]
		var glyph = PLANET_GLYPHS.get(info["ruling_planet"], "?")
		
		var start_dict = Time.get_time_dict_from_unix_time(info["hour_start_utc"] + tz_offset)
		var end_dict = Time.get_time_dict_from_unix_time(info["hour_end_utc"] + tz_offset)
		
		var time_str = "%02d:%02d - %02d:%02d" % [start_dict.hour, start_dict.minute, end_dict.hour, end_dict.minute]
		var segment = "Day" if info["hour_index"] < 12 else "Night"
		
		var text = "%s %s (%s) | %s" % [glyph, info["planet_name"], segment, time_str]
		var lbl = Label.new()
		lbl.text = text
		schedule_list.add_child(lbl)
		
	panel_schedule.visible = true

func _on_btn_close_schedule_pressed() -> void:
	panel_schedule.visible = false

@onready var panel_calendar: PanelContainer = $PanelCalendar
@onready var calendar_grid: GridContainer = $PanelCalendar/VBox/CalendarGrid

func _on_btn_calendar_pressed() -> void:
	for child in calendar_grid.get_children():
		child.queue_free()
		
	for i in range(35):
		var lbl = Label.new()
		if i < 3 or i > 33:
			lbl.text = ""
		else:
			lbl.text = str(i - 2)
		lbl.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
		calendar_grid.add_child(lbl)
		
	panel_schedule.hide()
	panel_calendar.show()

func _on_btn_close_calendar_pressed() -> void:
	panel_calendar.hide()
