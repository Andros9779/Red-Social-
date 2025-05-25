# CMakeLists.txt for red_social project
# Configures build for core library, CLI executable, unit tests, and Qt6 GUI

# Specify minimum required CMake version
cmake_minimum_required(VERSION 3.18)
# Define the project name
project(red_social)

# Habilitar automoc, autouic y autorcc para Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find and link the nlohmann_json library for JSON support
find_package(nlohmann_json REQUIRED)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)

# Add project-specific include directory
# Directorios de cabeceras propios
include_directories(include)

# Gather all source files for the core library
# Fuente núcleo (toda la lógica de estructuras)
file(GLOB SRC "src/*.cpp")

# Exclude GUI main files from the core library sources
# Excluir main_qt.cpp del core (lo compilará solo gui_qt)
list(FILTER SRC EXCLUDE REGEX ".*/main_qt\\.cpp$")
# Excluir main_gui.cpp del core para que solo lo compile el target gui
list(FILTER SRC EXCLUDE REGEX ".*/main_gui\\.cpp$")
# Exclude LoginDialog.cpp from the core library sources
list(FILTER SRC EXCLUDE REGEX ".*/LoginDialog\\.cpp$")
# Exclude RegisterDialog.cpp from the core library sources
list(FILTER SRC EXCLUDE REGEX ".*/RegisterDialog\\.cpp$")
# Exclude TimelineWidget.cpp from the core library sources
list(FILTER SRC EXCLUDE REGEX ".*/TimelineWidget\\.cpp$")

# Build core library from collected sources
# Librería core reutilizable por ejecutables y tests
add_library(core ${SRC})
target_link_libraries(core PUBLIC nlohmann_json::nlohmann_json)

# Define the CLI executable
# Ejecutable principal
add_executable(red_social main.cpp)
target_link_libraries(red_social PRIVATE core nlohmann_json::nlohmann_json)

# Enable CTest for running unit tests
# ---------- Tests -------------------------------------------------
enable_testing()

# Test executable for HashTable
# Test de HashTable
add_executable(test_hash tests/test_hash.cpp)
target_link_libraries(test_hash PRIVATE core nlohmann_json::nlohmann_json)
add_test(NAME test_hash COMMAND test_hash)

# Test executable for Graph class
# Test de Graph
add_executable(test_graph tests/test_graph.cpp)
target_link_libraries(test_graph PRIVATE core nlohmann_json::nlohmann_json)

add_test(NAME test_graph COMMAND test_graph)

# Configure Qt6 Widgets for GUI application
# ---------- GUI con Qt6 Widgets -----------------------------
# ---------- GUI con Qt6 Widgets -----------------------------
# Ruta correcta al Qt Online Installer en macOS
set(Qt6_DIR "/Users/jdds/Qt/6.9.0/macos/lib/cmake/Qt6" CACHE PATH "Qt6 CMake dir" FORCE)
list(APPEND CMAKE_PREFIX_PATH "/Users/jdds/Qt/6.9.0/macos/lib/cmake")

find_package(Qt6 6.9 REQUIRED COMPONENTS Widgets)

add_executable(gui_qt
    src/main_qt.cpp
    src/LoginDialog.cpp
    src/RegisterDialog.cpp
    src/TimelineWidget.cpp
    include/LoginDialog.h
    include/RegisterDialog.h
    include/TimelineWidget.h
)
target_link_libraries(gui_qt
    PRIVATE
      core
      Qt6::Widgets
)