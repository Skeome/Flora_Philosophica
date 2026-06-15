extends Node2D

var calculator: PlanetaryHourCalculator
@onready var clock_label = %ClockLabel

func _ready():
	calculator = PlanetaryHourCalculator.new()
	update_clock()
	
	# Update every second
	var timer = Timer.new()
	add_child(timer)
	timer.wait_time = 1.0
	timer.timeout.connect(update_clock)
	timer.start()

func update_clock():
	# Using sample coordinates (you can make these configurable later)
	var lat = 42.3265
	var lon = -122.8756
	var now = int(Time.get_unix_time_from_system())
	
	var info = calculator.calculate_planetary_hour(lat, lon, now)
	
	var text = "Ruling Planet: %s\n" % info["planet_name"]
	text += "Day Ruler: %s\n" % PlanetaryHourCalculator.get_planet_name(info["day_ruler"])
	text += "Hour: %d/24\n" % (info["hour_index"] + 1)
	text += "Next change in: %d min" % int(info["minutes_remaining"])
	
	clock_label.text = text
