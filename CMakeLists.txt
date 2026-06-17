cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(FloraPhilosophica)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CPM.cmake Bootstrap ---
set(CPM_DOWNLOAD_VERSION 0.40.4)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
    ${CPM_DOWNLOAD_LOCATION}
  )
endif()
include(${CPM_DOWNLOAD_LOCATION})

# --- Dependencies ---
CPMAddPackage(
    NAME godot-cpp
    GITHUB_REPOSITORY godotengine/godot-cpp
    GIT_TAG godot-4.5-stable
)

CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  VERSION 3.11.3
)

# --- GDExtension Target ---
# Explicitly listing sources to avoid missing new files
set(GDE_SOURCES
    "src/core/register_types.cpp"
    "src/core/clock.cpp"
    "src/world/item.cpp"
    "src/world/inventory.cpp"
    "src/world/placed_item.cpp"
    "src/world/room_manager.cpp"
    "src/alchemy/plant_db.cpp"
)

add_library(flora_philosophica MODULE ${GDE_SOURCES})

target_link_libraries(flora_philosophica 
    PUBLIC 
        godot::cpp 
        nlohmann_json::nlohmann_json
)

target_include_directories(flora_philosophica PUBLIC src)

# Set output properties to match .gdextension manifest
set_target_properties(flora_philosophica PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/godot/bin"
    PREFIX "lib"
    SUFFIX ".so"
)

target_compile_options(flora_philosophica PRIVATE -fPIC)

if(WIN32)
    set_target_properties(flora_philosophica PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(flora_philosophica PROPERTIES SUFFIX ".dylib")
endif()
