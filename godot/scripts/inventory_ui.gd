extends Control

## ─────────────────────────────────────────────────────────────────────────────
## InventoryUI  ·  godot/scripts/inventory_ui.gd
## Attached to:  Canvas (Control) — child of the InventoryUI CanvasLayer (per-scene instance).
##
## Renders the always-visible hotbar (10 slots) at the screen bottom and a
## togglable inventory panel (9 cols × 4 rows = 36 grid slots) opened with [I].
## Total: 46 slots  (10 + 36 = Inventory::TOTAL_SLOTS ✓)
##
## Design principles:
##   • All drawing is done in _draw() via Control.draw_* calls — no extra nodes.
##   • Mouse is handled in _gui_input(); keyboard in _unhandled_input().
##   • Only the InfoBox PanelContainer uses child Label nodes (for autowrap text).
##   • Nothing in the C++ backend is touched for presentation concerns.
## ─────────────────────────────────────────────────────────────────────────────

# ── Layout constants ───────────────────────────────────────────────────────────
const SLOT_SIZE := 26          # square pixels per slot
const GAP       := 2           # gap between adjacent slots
const SS        := SLOT_SIZE + GAP   # slot step = 28 px

const HOTBAR_N  := 10          # ten hotbar slots  (indices 0–9)
const GRID_COLS := 9           # nine columns per grid row
const GRID_ROWS := 4           # four rows         (9 × 4 = 36 grid slots)
const GRID_N    := GRID_COLS * GRID_ROWS   # 36

const PAD     := 8             # inner padding inside the panel
const TITLE_H := 18            # panel title bar height
const INFO_W  := 200           # info column width

# Native viewport dimensions — must match project.godot
const VP_W := 640.0
const VP_H := 360.0

# ── Quality display colours (indexed by HarvestQuality enum 0–4) ──────────────
const Q_COL : Array[Color] = [
	Color(1.00, 0.84, 0.00),   # 0 Celestial — gold
	Color(0.92, 0.92, 0.92),   # 1 Pristine  — near-white
	Color(0.48, 0.48, 0.48),   # 2 Standard  — grey
	Color(0.95, 0.50, 0.00),   # 3 Stressed  — orange
	Color(0.72, 0.10, 0.10),   # 4 Debased   — red
]

const Q_NAME  : Array[String] = ["Celestial","Pristine","Standard","Stressed","Debased"]
const P_NAME  : Array[String] = ["Saturn","Jupiter","Mars","Sun","Venus","Mercury","Moon"]
const P_GLYPH : Array[String] = ["♄","♃","♂","☉","♀","☿","☽"]

# Short abbreviations drawn inside each slot (PlantStage enum 0–21)
const STAGE_AB : Dictionary = {
	0: "F",    # Fresh
	1: "D",    # Dried
	2: "G",    # Ground
	3: "Spt",  # Spent
	4: "Sp",   # Spirits
	5: "Sa",   # Salt
	6: "T",    # Tincture
	7: "CK",   # Calx Black
	8: "CKg",  # Calx Black Ground
	9: "CG",   # Calx Grey
	10: "CGg", # Calx Grey Ground
	11: "CW",  # Calx White
	12: "PSa", # Salt Purified
	13: "CLG", # Calx Light Grey
	14: "CLg", # Calx Light Grey Ground
	15: "TB",  # Tincture Basic
	16: "TS",  # Tincture Spagyric
	17: "El",  # Elixir
	18: "TM",  # Tincture Magistery
	19: "TE",  # Tincture Ens
	20: "PE",  # Primum Ens
	21: "St",  # Plant Stone
}

# Short abbreviations for station types (ItemType enum 0–18)
const STATION_AB : Dictionary = {
	0: "Fire", 1: "Rack", 2: "Mort", 3: "Mac.",
	4: "Comp", 5: "Bnch", 6: "Alem", 7: "Flsk",
	8: "Glas", 9: "Dist", 10: "Sox", 11: "Peli",
	12: "Ret",  13: "Terr", 14: "Bksh", 15: "Chst",
	16: "Mail", 17: "Furn", 18: "Leac",
}

# Full names for the InfoBox title (mirrors ItemType order in item.h)
const STATION_NAME : Array[String] = [
	"Fireplace","Drying Rack","Mortar & Pestle","Maceration Jar","Compost Bin",
	"Work Bench","Copper Alembic","Glass Flask","Glassblowing Station",
	"Distillation Train","Soxhlet Extractor","Pelican Flask","Retort Train",
	"Terrarium","Bookshelf","Storage Chest","Mailbox Post","Furnace","Leaching Dish",
]

# ── Runtime state ─────────────────────────────────────────────────────────────
var _open     := false   # inventory panel visible?
var _sel      := 0       # selected hotbar index (0–9); mirrors GameManager
var _info_idx := -1      # slot index shown in InfoBox; -1 = nothing selected
var _drag_src := -1      # drag source slot index; -1 = not dragging

# ── Computed layout (assigned once in _ready) ─────────────────────────────────
var _hbar_x0  : float    # hotbar strip left edge
var _hbar_y   : float    # hotbar strip top edge
var _panel    : Rect2    # full panel bounding rect
var _grid_org : Vector2  # grid top-left corner
var _info_org : Vector2  # info column top-left corner

# ── Node references ───────────────────────────────────────────────────────────
@onready var _info_box : PanelContainer = $InfoBox
@onready var _lbl_name : Label          = $InfoBox/VBox/LblName
@onready var _lbl_det  : Label          = $InfoBox/VBox/LblDetail
@onready var _lbl_prop : Label          = $InfoBox/VBox/Scroll/LblProp

var _font : Font = null

# ─────────────────────────────────────────────────────────────────────────────
func _ready() -> void:
	set_process(true)
	set_process_unhandled_input(true)

	# Load the Archemy alchemical font; fall back to system font if absent.
	# Archemy is in .gitignore (proprietary) — must be placed manually.
	if ResourceLoader.exists("res://assets/fonts/Archemy.otf"):
		_font = load("res://assets/fonts/Archemy.otf") as Font
	if _font == null:
		_font = ThemeDB.fallback_font

	# ── Hotbar geometry — centred near the bottom of the screen ───────────
	var hbar_w := HOTBAR_N * SS - GAP
	_hbar_x0   = (VP_W - float(hbar_w)) * 0.5
	_hbar_y    = VP_H - float(SLOT_SIZE) - 5.0

	# ── Panel geometry ────────────────────────────────────────────────────
	var grid_w := GRID_COLS * SS - GAP   # 250 px
	var grid_h := GRID_ROWS * SS - GAP   # 110 px
	var pw := PAD + grid_w + PAD + INFO_W + PAD   # 474 px
	var ph := TITLE_H + PAD + grid_h + PAD        # 144 px
	_panel    = Rect2((VP_W - float(pw)) * 0.5, 22.0, float(pw), float(ph))
	_grid_org = _panel.position + Vector2(float(PAD), float(TITLE_H + PAD))
	_info_org = _grid_org + Vector2(float(grid_w + PAD), 0.0)

	# Position the InfoBox node (size/position are set here, not in .tscn)
	_info_box.visible  = false
	_info_box.position = _info_org
	_info_box.size     = Vector2(float(INFO_W), float(grid_h))


# ── Keyboard input ────────────────────────────────────────────────────────────
func _unhandled_input(event: InputEvent) -> void:
	if not event is InputEventKey or not event.pressed or event.echo:
		return
	var key := event as InputEventKey
	match key.keycode:
		KEY_I:
			_toggle_panel()
			get_viewport().set_input_as_handled()
		# Number keys 1–9 select hotbar slots 0–8; 0 selects slot 9
		KEY_1: _set_sel(0)
		KEY_2: _set_sel(1)
		KEY_3: _set_sel(2)
		KEY_4: _set_sel(3)
		KEY_5: _set_sel(4)
		KEY_6: _set_sel(5)
		KEY_7: _set_sel(6)
		KEY_8: _set_sel(7)
		KEY_9: _set_sel(8)
		KEY_0: _set_sel(9)


func _set_sel(i: int) -> void:
	_sel = i
	if is_instance_valid(GameManager):
		GameManager.selected_hotbar_slot = i


func _toggle_panel() -> void:
	_open = not _open
	if not _open:
		_info_box.visible = false


# ── Mouse input ───────────────────────────────────────────────────────────────
func _gui_input(event: InputEvent) -> void:
	if not event is InputEventMouseButton:
		return
	var mbe := event as InputEventMouseButton
	if mbe.button_index != MOUSE_BUTTON_LEFT:
		return

	var mpos := get_local_mouse_position()

	if mbe.pressed:
		var idx := _slot_at(mpos)
		if idx >= 0:
			_drag_src = idx
			# Hotbar click → select that slot immediately
			if idx < HOTBAR_N:
				_set_sel(idx)
			# Update info panel with clicked slot
			_info_idx = idx
			_update_info(idx)
			if _open:
				_info_box.visible = true
			accept_event()
			return
		# Block clicks that land inside the open panel but miss all slots
		if _open and _panel.has_point(mpos):
			accept_event()

	else:  # mouse button released
		if _drag_src >= 0:
			var target := _slot_at(get_local_mouse_position())
			if target >= 0 and target != _drag_src:
				# Swap the two slots
				var inv = _get_inv()
				if inv:
					inv.swap_slots(_drag_src, target)
					_info_idx = target
					_update_info(target)
			_drag_src = -1
			accept_event()


# ── Drawing ───────────────────────────────────────────────────────────────────
func _process(_dt: float) -> void:
	queue_redraw()


func _draw() -> void:
	if _open:
		# Semi-transparent dim over the game world while panel is open
		draw_rect(Rect2(Vector2.ZERO, Vector2(VP_W, VP_H)), Color(0, 0, 0, 0.58))
		_draw_panel()
	_draw_hotbar()


func _draw_hotbar() -> void:
	var inv = _get_inv()
	if inv == null:
		return

	# Strip background behind the ten hotbar slots
	var strip_x := _hbar_x0 - 4.0
	var strip_w := float(HOTBAR_N * SS - GAP + 8)
	draw_rect(Rect2(strip_x, _hbar_y - 3.0, strip_w, float(SLOT_SIZE) + 6.0),
			  Color(0.12, 0.09, 0.06, 0.90))
	draw_rect(Rect2(strip_x, _hbar_y - 3.0, strip_w, float(SLOT_SIZE) + 6.0),
			  Color(0.55, 0.39, 0.27, 0.85), false, 1.5)

	for i in range(HOTBAR_N):
		_draw_slot(_hbar_rect(i), inv.get_slot(i), i == _sel, i)


func _draw_panel() -> void:
	var inv = _get_inv()
	if inv == null:
		return

	# Panel background and border
	draw_rect(_panel, Color(0.14, 0.10, 0.07, 0.96))
	draw_rect(_panel, Color(0.55, 0.39, 0.27, 1.0), false, 1.5)

	# "Inventory" title (top-left)
	draw_string(_font,
				_panel.position + Vector2(float(PAD), float(TITLE_H) - 3.0),
				"Inventory",
				HORIZONTAL_ALIGNMENT_LEFT, -1, 11, Color(1.0, 0.84, 0.0))

	# [I] close hint (top-right)
	draw_string(_font,
				_panel.position + Vector2(_panel.size.x - 26.0, float(TITLE_H) - 3.0),
				"[I]",
				HORIZONTAL_ALIGNMENT_LEFT, -1, 9, Color(0.50, 0.50, 0.50))

	# Horizontal rule under the title
	var div_y := _panel.position.y + float(TITLE_H)
	draw_line(Vector2(_panel.position.x + 2.0, div_y),
			  Vector2(_panel.position.x + _panel.size.x - 2.0, div_y),
			  Color(0.42, 0.28, 0.18, 0.80), 1.0)

	# Grid slots — inventory indices 10 through 45
	for i in range(GRID_N):
		_draw_slot(_grid_rect(i), inv.get_slot(10 + i), (10 + i) == _info_idx, 10 + i)


func _draw_slot(rect: Rect2, slot, selected: bool, slot_idx: int) -> void:
	# Slot background
	draw_rect(rect, Color(0.18, 0.13, 0.09, 0.90))

	if not slot.occupied:
		# Empty slot — selection gold or dim brown border
		var empty_bc := Color(1.0, 0.84, 0.0) if selected else Color(0.35, 0.25, 0.17, 0.70)
		draw_rect(rect, empty_bc, false, 2.0 if selected else 1.0)
		# Number hint for hotbar slots (1–9, 0 = slot 10)
		if slot_idx < HOTBAR_N:
			draw_string(_font,
						rect.position + Vector2(2.0, float(SLOT_SIZE) - 3.0),
						str((slot_idx + 1) % 10),
						HORIZONTAL_ALIGNMENT_LEFT, -1, 7, Color(0.35, 0.35, 0.35))
		return

	# Occupied slot — choose border colour by quality or station type
	var bc: Color
	if selected:
		bc = Color(1.0, 0.84, 0.0)
	elif slot.is_herb and slot.herb != null:
		bc = Q_COL[clampi(int(slot.herb.quality), 0, 4)]
	else:
		bc = Color(0.65, 0.55, 0.35)   # warm tan for station items
	draw_rect(rect, bc, false, 2.0 if selected else 1.5)

	if slot.is_herb and slot.herb != null:
		# Stage abbreviation — top-left corner
		var ab := str(STAGE_AB.get(int(slot.herb.stage), "?"))
		draw_string(_font, rect.position + Vector2(2.0, 9.0),
					ab, HORIZONTAL_ALIGNMENT_LEFT, -1, 7, Color(0.85, 0.85, 0.85, 0.80))
		# Plant name truncated to 6 characters — bottom
		var nm := str(slot.herb.plant_name).left(6)
		draw_string(_font, rect.position + Vector2(1.0, float(SLOT_SIZE) - 2.0),
					nm, HORIZONTAL_ALIGNMENT_LEFT, -1, 7, Color.WHITE)
		# Quality dot — bottom-right
		draw_circle(rect.position + Vector2(float(SLOT_SIZE) - 3.5, float(SLOT_SIZE) - 3.5),
					2.5, Q_COL[clampi(int(slot.herb.quality), 0, 4)])
	else:
		# Station / non-herb item
		var ab := str(STATION_AB.get(int(slot.station), "???"))
		draw_string(_font, rect.position + Vector2(1.0, float(SLOT_SIZE) - 2.0),
					ab, HORIZONTAL_ALIGNMENT_LEFT, -1, 7, Color(0.90, 0.85, 0.60))
		# Quantity badge — top-right (only when > 1)
		if slot.quantity > 1:
			draw_string(_font,
						rect.position + Vector2(float(SLOT_SIZE) - 12.0, 9.0),
						str(slot.quantity),
						HORIZONTAL_ALIGNMENT_LEFT, -1, 7, Color(1.0, 0.84, 0.0))


# ── Slot geometry helpers ──────────────────────────────────────────────────────
func _hbar_rect(i: int) -> Rect2:
	return Rect2(Vector2(_hbar_x0 + float(i) * SS, _hbar_y),
				 Vector2(SLOT_SIZE, SLOT_SIZE))


func _grid_rect(i: int) -> Rect2:
	var col : int = i % GRID_COLS
	var row : int = int(float(i) / float(GRID_COLS))   # explicit float division avoids INTEGER_DIVISION warning
	return Rect2(
		_grid_org + Vector2(float(col) * SS, float(row) * SS),
		Vector2(SLOT_SIZE, SLOT_SIZE)
	)


func _slot_at(pos: Vector2) -> int:
	## Returns the global inventory slot index (0–9 hotbar, 10–45 grid) at pos,
	## or -1 if pos is over an empty area.
	for i in range(HOTBAR_N):
		if _hbar_rect(i).has_point(pos):
			return i
	if _open:
		for i in range(GRID_N):
			if _grid_rect(i).has_point(pos):
				return 10 + i
	return -1


# ── InfoBox content ───────────────────────────────────────────────────────────
func _update_info(slot_idx: int) -> void:
	var inv = _get_inv()
	if inv == null:
		return

	var slot = inv.get_slot(slot_idx)

	if not slot.occupied:
		_lbl_name.text = "Empty"
		_lbl_det.text  = ""
		_lbl_prop.text = "Select an item to view its properties."
		return

	if slot.is_herb and slot.herb != null:
		_lbl_name.text = slot.herb.plant_name

		# Stage and quality on one line
		var sname : String = HarvestItem.get_stage_name(slot.herb.stage)
		var qidx  : int    = clampi(int(slot.herb.quality), 0, 4)
		_lbl_det.text = sname + "  ·  " + Q_NAME[qidx]

		# Culpeper data from the C++ PlantDatabase
		var plant = PlantDatabase.get_plant(slot.herb.plant_name)
		if plant:
			var ridx : int = clampi(int(plant.ruler), 0, 6)
			_lbl_prop.text = (
				P_GLYPH[ridx] + " " + P_NAME[ridx] +
				"  ·  " + plant.element +
				"\n\n" + plant.properties
			)
		else:
			_lbl_prop.text = "(No plant data found)"
	else:
		# Station / placeable item
		var tidx : int = clampi(int(slot.station), 0, STATION_NAME.size() - 1)
		_lbl_name.text = STATION_NAME[tidx]
		_lbl_det.text  = "Qty: " + str(slot.quantity)
		_lbl_prop.text = "Place this station in a room to use it."


# ── Convenience ───────────────────────────────────────────────────────────────
func _get_inv():
	## Returns GameManager.inventory or null if GameManager is unavailable.
	if not is_instance_valid(GameManager):
		return null
	return GameManager.inventory
