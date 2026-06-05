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
find_package(raylib REQUIRED)

CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  VERSION 3.11.3
)

# --- Library Target ---
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

add_library(FloraPhilosophicaCore STATIC ${CORE_SOURCES})
target_link_libraries(FloraPhilosophicaCore PUBLIC raylib nlohmann_json::nlohmann_json)
target_include_directories(FloraPhilosophicaCore PUBLIC src)

# --- Executable Target ---
add_executable(FloraPhilosophica src/main.cpp)
target_link_libraries(FloraPhilosophica PRIVATE FloraPhilosophicaCore)

# --- Testing ---
option(BUILD_TESTING "Build the test suite" ON)
if(BUILD_TESTING)
  enable_testing()

  CPMAddPackage(
    NAME doctest
    GITHUB_REPOSITORY doctest/doctest
    VERSION 2.4.11
  )

  file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

  add_executable(FloraPhilosophicaTests ${TEST_SOURCES})
  target_link_libraries(FloraPhilosophicaTests PRIVATE FloraPhilosophicaCore doctest::doctest)
  target_include_directories(FloraPhilosophicaTests PRIVATE tests)

  add_test(NAME CoreTests COMMAND FloraPhilosophicaTests)
endif()