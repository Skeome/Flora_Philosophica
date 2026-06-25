extends Control

const MAIN_SCENE = "res://scenes/main.tscn"

@onready var opt_gender: OptionButton = $VBox/OptionGender
@onready var edit_loc: LineEdit = $VBox/EditLocation
@onready var edit_birth: LineEdit = $VBox/EditBirth

func _ready() -> void:
	pass

func _on_btn_start_pressed() -> void:
	GameManager.player_gender = opt_gender.selected
	
	var loc_text = edit_loc.text.strip_edges()
	if loc_text != "":
		var parts = loc_text.split(",")
		if parts.size() == 2:
			GameManager.observer_lat = parts[0].to_float()
			GameManager.observer_lon = parts[1].to_float()
			
	GameManager.player_birth_data = edit_birth.text.strip_edges()
	GameManager.calculate_natal_chart()
	
	GameManager.save_game()
	get_tree().change_scene_to_file(MAIN_SCENE)

func _on_btn_back_pressed() -> void:
	get_tree().change_scene_to_file("res://scenes/main_menu.tscn")
