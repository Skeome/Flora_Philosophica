extends Node

class_name RecipeDB

# Utility to generate the full spagyric/alchemical pipeline for a given plant
static func generate_pipeline_for_plant() -> Array[Recipe]:
	var recipes: Array[Recipe] = []
	
	# 1. Fresh -> Dried (Drying Rack)
	var r1 = Recipe.new()
	r1.set_required_station(ItemDB.ITEM_DRYING_RACK)
	r1.set_inputs([{"stage": HarvestItem.STAGE_FRESH}])
	r1.set_outputs([{"stage": HarvestItem.STAGE_DRIED}])
	r1.set_base_processing_time(43200.0) # 12 hours
	recipes.append(r1)
	
	# 2. Dried -> Ground (Mortar and Pestle)
	var r2 = Recipe.new()
	r2.set_required_station(ItemDB.ITEM_MORTAR_AND_PESTLE)
	r2.set_inputs([{"stage": HarvestItem.STAGE_DRIED}])
	r2.set_outputs([{"stage": HarvestItem.STAGE_GROUND}])
	r2.set_base_processing_time(3600.0) # 1 hour
	recipes.append(r2)
	
	# 3. Ground -> Tincture + Residue (Maceration Jar)
	var r3 = Recipe.new()
	r3.set_required_station(ItemDB.ITEM_MACERATION_JAR)
	r3.set_inputs([{"stage": HarvestItem.STAGE_GROUND}])
	r3.set_outputs([
		{"stage": HarvestItem.STAGE_TINCTURE},
		{"stage": HarvestItem.STAGE_SPENT} # Residue
	])
	r3.set_base_processing_time(604800.0) # 7 days
	recipes.append(r3)
	
	# 4. Residue -> Calx Black (Furnace)
	var r4 = Recipe.new()
	r4.set_required_station(ItemDB.ITEM_FURNACE)
	r4.set_inputs([{"stage": HarvestItem.STAGE_SPENT}])
	r4.set_outputs([{"stage": HarvestItem.STAGE_CALX_BLACK}])
	r4.set_base_processing_time(9000.0) # 2.5 hours (calcination)
	recipes.append(r4)
	
	# 5. Calx Black -> Powdered Calx Black (Mortar and Pestle)
	var r5 = Recipe.new()
	r5.set_required_station(ItemDB.ITEM_MORTAR_AND_PESTLE)
	r5.set_inputs([{"stage": HarvestItem.STAGE_CALX_BLACK}])
	r5.set_outputs([{"stage": HarvestItem.STAGE_CALX_BLACK_GROUND}])
	r5.set_base_processing_time(3600.0)
	recipes.append(r5)
	
	# 6. Powdered Calx Black -> Calx Grey (Leaching Dish)
	var r6 = Recipe.new()
	r6.set_required_station(ItemDB.ITEM_LEACHING_DISH)
	r6.set_inputs([{"stage": HarvestItem.STAGE_CALX_BLACK_GROUND}])
	r6.set_outputs([{"stage": HarvestItem.STAGE_CALX_GREY}])
	r6.set_base_processing_time(3600.0)
	recipes.append(r6)
	
	# 7. Calx Grey -> Calx White (Furnace)
	# Skipped "Light Grey" to align with existing PlantStage enums
	var r7 = Recipe.new()
	r7.set_required_station(ItemDB.ITEM_FURNACE)
	r7.set_inputs([{"stage": HarvestItem.STAGE_CALX_GREY}])
	r7.set_outputs([{"stage": HarvestItem.STAGE_CALX_WHITE}])
	r7.set_base_processing_time(9000.0) # 2.5 hours
	recipes.append(r7)
	
	# 8. Calx White -> Salt Standard (Furnace)
	var r8 = Recipe.new()
	r8.set_required_station(ItemDB.ITEM_FURNACE)
	r8.set_inputs([{"stage": HarvestItem.STAGE_CALX_WHITE}])
	r8.set_outputs([{"stage": HarvestItem.STAGE_SALT}])
	r8.set_base_processing_time(9000.0) # 2.5 hours
	recipes.append(r8)
	
	# 9. Salt Standard -> Purified Salt (Leaching Dish)
	var r9 = Recipe.new()
	r9.set_required_station(ItemDB.ITEM_LEACHING_DISH)
	r9.set_inputs([{"stage": HarvestItem.STAGE_SALT}])
	r9.set_outputs([{"stage": HarvestItem.STAGE_SALT_PURIFIED}])
	r9.set_base_processing_time(3600.0)
	recipes.append(r9)
	
	return recipes
