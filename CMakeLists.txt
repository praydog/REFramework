cmake_minimum_required(VERSION 3.1)

# fixes stuff with ninja generator
set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo;Debug")

project(REFRAMEWORK)

option(SINGLE_BUILD "Build configurations in single project" ON)
option(DEVELOPER_MODE "Adds DeveloperTools option to menu" ON)
option(BAREBONES "Barebones build without game-specific features/hooks" OFF)

option(RE2 "RE2 build" OFF)
option(RE3 "RE3 build" OFF)
option(RE7 "RE7 build" OFF)
option(RE8 "RE8 build" OFF)
option(DMC5 "DMC5 build" OFF)
option(MHRISE "MHRISE build" OFF)

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Disable exceptions
# string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Statically compile runtime
string(REGEX REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
string(REGEX REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

add_subdirectory(dependencies)

# Easy way to have multiple configurations in a single project.
macro (add_game game_name)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin/${game_name})
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${game_name})
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib/${game_name})
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

    set(${game_name} ON)
    set(GAME_NAME ${game_name})

    add_subdirectory(src ${CMAKE_BINARY_DIR}/${game_name})

    set(${game_name} OFF)
endmacro()

if (SINGLE_BUILD)
    add_game(MHRISE)
    add_game(DMC5)
    add_game(RE2)
    add_game(RE3)
    add_game(RE7)
    add_game(RE8)
else()
    set(GAME_NAME GAME)
    add_subdirectory(src)
endif()
