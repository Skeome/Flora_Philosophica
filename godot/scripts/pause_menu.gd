extends CanvasLayer

# ─────────────────────────────────────────────────────────────────────────────
# PauseMenu
# Handles standard pause buttons on the left, and character sheet tabs on the right.
# ─────────────────────────────────────────────────────────────────────────────

const MAIN_MENU_SCENE = "res://scenes/main_menu.tscn"

@onready var status_label: Label = $Panel/HBox/VBoxMenu/SaveStatus
@onready var dim: ColorRect = $Dim
@onready var panel: PanelContainer = $Panel

# Tab Buttons
@onready var btn_tab_natal: Button = %BtnTabNatal
@onready var btn_tab_fire: Button = %BtnTabFire

# Tab Panels
@onready var natal_panel: VBoxContainer = %NatalPanel
@onready var fire_panel: VBoxContainer = %FirePanel

# Natal Panel Controls
@onready var stats_grid: GridContainer = %StatsGrid
@onready var placements_text: Label = %PlacementsText
@onready var traits_text: Label = %TraitsText

# Fire Panel Controls
@onready var opt_plant: OptionButton = %OptPlant
@onready var sheet_grid: GridContainer = %SheetGrid
@onready var result_text: Label = %ResultText

var _status_timer: float = 0.0
var _initialized_plants: bool = false

func _ready() -> void:
	visible = false
	status_label.text = ""
	dim.mouse_filter = Control.MOUSE_FILTER_STOP
	panel.mouse_filter = Control.MOUSE_FILTER_STOP
	
	# Connect Tab Buttons
	btn_tab_natal.pressed.connect(_on_tab_natal_pressed)
	btn_tab_fire.pressed.connect(_on_tab_fire_pressed)
	
	# Connect Plant Selection
	opt_plant.item_selected.connect(_on_plant_selected)

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
	
	# Unlock or lock movement input
	var player := get_tree().get_first_node_in_group("player")
	if player and player.has_method("set_input_locked"):
		player.set_input_locked(visible)
		
	if visible:
		_update_tabs()

func _on_btn_resume_pressed() -> void:
	_toggle_menu()

func _on_btn_save_pressed() -> void:
	GameManager.save_game()
	_show_status("Game saved.")

func _on_btn_load_pressed() -> void:
	GameManager.load_game()
	_show_status("Game loaded.")
	var player := get_tree().get_first_node_in_group("player")
	if player and GameManager.has_method("get_saved_player_position"):
		player.global_position = GameManager.get_saved_player_position()
	_update_tabs()

func _on_btn_settings_pressed() -> void:
	_show_status("Settings coming soon.")

func _on_btn_main_menu_pressed() -> void:
	GameManager.save_game()
	get_tree().change_scene_to_file(MAIN_MENU_SCENE)

func _show_status(message: String) -> void:
	status_label.text = message
	_status_timer = 2.0

# ── Tab Switching ─────────────────────────────────────────────────────────────

func _on_tab_natal_pressed() -> void:
	btn_tab_natal.button_pressed = true
	btn_tab_fire.button_pressed = false
	natal_panel.visible = true
	fire_panel.visible = false

func _on_tab_fire_pressed() -> void:
	btn_tab_natal.button_pressed = false
	btn_tab_fire.button_pressed = true
	natal_panel.visible = false
	fire_panel.visible = true
	if not _initialized_plants:
		_populate_plants()

# ── Tab Content Rendering ──────────────────────────────────────────────────────

func _update_tabs() -> void:
	# 1. Update Natal Chart Tab
	_update_natal_chart()
	
	# 2. Update Plants dropdown if on Fire tab
	if fire_panel.visible:
		_populate_plants()

func _update_natal_chart() -> void:
	# Clear previous stats children
	for child in stats_grid.get_children():
		child.queue_free()
		
	# Populate RPG Planetary Stats
	var planets = ["Saturn", "Jupiter", "Mars", "Sun", "Venus", "Mercury", "Moon"]
	for p in planets:
		var val = GameManager.player_stats.get(p, 10)
		
		# Name Label
		var lbl_name = Label.new()
		lbl_name.text = p + ":"
		lbl_name.theme_override_fonts.font = btn_tab_natal.theme_override_fonts.font
		stats_grid.add_child(lbl_name)
		
		# Value Label
		var lbl_val = Label.new()
		lbl_val.text = str(val)
		lbl_val.theme_override_fonts.font = btn_tab_natal.theme_override_fonts.font
		stats_grid.add_child(lbl_val)
		
	# Placements display
	if GameManager.player_birth_data == "":
		placements_text.text = "No birth data loaded. Please define birth data during Character Selection."
	else:
		var placements_str = ""
		for p in planets:
			var sign_name = GameManager.sign_placements.get(p, "Unknown")
			placements_str += "  • %s is placed in %s (+2 to its ruler)\n" % [p, sign_name]
		placements_text.text = placements_str
		
	# Passive Traits display
	if GameManager.player_traits.size() == 0:
		traits_text.text = "No aspect passive traits active in natal chart."
	else:
		var traits_str = ""
		for t in GameManager.player_traits:
			traits_str += "  • %s\n    %s\n" % [t["name"], t["desc"]]
		traits_text.text = traits_str

func _populate_plants() -> void:
	opt_plant.clear()
	var all_plants = PlantDatabase.get_all_plants()
	for i in range(all_plants.size()):
		var p: PlantData = all_plants[i]
		opt_plant.add_item(p.name, i)
	_initialized_plants = true
	if all_plants.size() > 0:
		opt_plant.selected = 0
		_on_plant_selected(0)

func _on_plant_selected(index: int) -> void:
	var plant_name = opt_plant.get_item_text(index)
	var plant: PlantData = PlantDatabase.get_plant(plant_name)
	if plant == null:
		return
		
	# Perform calculations
	var data = GameManager.calculate_spagyric_temper(
		plant.choler, plant.sanguine, plant.phlegm, plant.melancholy
	)
	
	# Clear grid
	for child in sheet_grid.get_children():
		child.queue_free()
		
	# Add headers
	var headers = ["Fraction", "Alchemical Element", "Weight %", "Classical Qualities"]
	for h in headers:
		var lbl = Label.new()
		lbl.text = h
		lbl.theme_override_fonts.font = btn_tab_natal.theme_override_fonts.font
		lbl.modulate = Color(0.93, 0.72, 0.51)
		sheet_grid.add_child(lbl)
		
	# Row 1: Phlegm
	_add_grid_row("Phlegm", "Water", "%.1f%%" % data["phlegm_pct"], "Cold (Dominant) & Wet (Secondary)")
	# Row 2: Sanguine / White Spirit
	_add_grid_row("White Spirit", "Air", "%.1f%%" % data["sanguine_pct"], "Wet (Dominant) & Hot (Secondary)")
	# Row 3: Choler / Red Spirit
	_add_grid_row("Red Spirit", "Fire", "%.1f%%" % data["choler_pct"], "Hot (Dominant) & Dry (Secondary)")
	# Row 4: Residue / Melancholy
	_add_grid_row("Residue", "Earth", "%.1f%%" % data["melancholy_pct"], "Dry (Dominant) & Cold (Secondary)")

	# Final calculations display text
	var hc_deg: float = data["hot_cold_degree"]
	var wd_deg: float = data["wet_dry_degree"]
	
	var hc_text = ""
	if hc_deg >= 0.0:
		hc_text = "Hot in the %.2f degree" % hc_deg
	else:
		hc_text = "Cold in the %.2f degree" % abs(hc_deg)
		
	var wd_text = ""
	if wd_deg >= 0.0:
		wd_text = "Wet in the %.2f degree" % wd_deg
	else:
		wd_text = "Dry in the %.2f degree" % abs(wd_deg)
		
	# Determine Jabir values
	var jabir_hc = int(round(abs(hc_deg) * 7.0))
	var jabir_wd = int(round(abs(wd_deg) * 7.0))
	
	result_text.text = (
		"Sub-Total Weight: 100.0%\n\n" +
		"Derived Humoral Balance: %s, %s.\n\n" % [hc_text, wd_text] +
		"Primary Quality Parts: [Hot: %.2f, Cold: %.2f] [Wet: %.2f, Dry: %.2f]\n" % [
			data["hot_parts"], data["cold_parts"], data["wet_parts"], data["dry_parts"]
		] +
		"Jabir Subdivision Values: %s (%d/28), %s (%d/28)" % [
			"Hot" if hc_deg >= 0.0 else "Cold", jabir_hc,
			"Wet" if wd_deg >= 0.0 else "Dry", jabir_wd
		]
	)

func _add_grid_row(frac: String, elem: String, wt: String, qual: String) -> void:
	var font = btn_tab_natal.theme_override_fonts.font
	
	var l1 = Label.new(); l1.text = frac; l1.theme_override_fonts.font = font; sheet_grid.add_child(l1)
	var l2 = Label.new(); l2.text = elem; l2.theme_override_fonts.font = font; sheet_grid.add_child(l2)
	var l3 = Label.new(); l3.text = wt;   l3.theme_override_fonts.font = font; sheet_grid.add_child(l3)
	var l4 = Label.new(); l4.text = qual; l4.theme_override_fonts.font = font; sheet_grid.add_child(l4)
