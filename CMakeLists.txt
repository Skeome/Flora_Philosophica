cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(FloraPhilosophia)

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
# Find Raylib (installed system-wide)
find_package(raylib REQUIRED)

# Fetch JSON library via CPM
CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  VERSION 3.11.3
)

# --- Library Target ---
# Collect all implementation files (excluding main.cpp)
file(GLOB_RECURSE CORE_SOURCES 
    "src/core/*.cpp" 
    "src/world/*.cpp" 
    "src/lab/*.cpp" 
    "src/alchemy/*.cpp" 
    "src/combat/*.cpp" 
    "src/economy/*.cpp" 
    "src/idle/*.cpp" 
    "src/ui/*.cpp"
)

add_library(FloraPhilosophiaCore STATIC ${CORE_SOURCES})
target_link_libraries(FloraPhilosophiaCore PUBLIC raylib nlohmann_json::nlohmann_json)
target_include_directories(FloraPhilosophiaCore PUBLIC src)

# --- Executable Target ---
add_executable(FloraPhilosophia src/main.cpp)
target_link_libraries(FloraPhilosophia PRIVATE FloraPhilosophiaCore)

# --- Testing ---
option(BUILD_TESTING "Build the test suite" ON)
if(BUILD_TESTING)
  enable_testing()
  
  # Fetch doctest unit-testing library via CPM
  CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY doctest/doctest
    VERSION 2.4.11
  )
  
  # Collect test files
  file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
  
  add_executable(FloraPhilosophiaTests ${TEST_SOURCES})
  target_link_libraries(FloraPhilosophiaTests PRIVATE FloraPhilosophiaCore doctest::doctest)
  target_include_directories(FloraPhilosophiaTests PRIVATE tests)
  
  add_test(NAME CoreTests COMMAND FloraPhilosophiaTests)
endif()