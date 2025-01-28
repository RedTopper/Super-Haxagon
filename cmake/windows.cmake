# cmake

message(STATUS "Building for windows...")

# Place a copy of SFML for Visual Studio in the libraries folder below
# or define SFML_DIR with -DSFML_DIR at the commandline
if(NOT DEFINED SFML_DIR)
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libraries/SFML/lib/cmake/SFML")
endif()

set(SFML_STATIC_LIBRARIES TRUE)

set(DRIVER_PLATFORM source/Driver/Win/PlatformWin.cpp)

include(cmake/sfml.cmake)

# Always link static on Windows
target_compile_definitions(SuperHaxagon PRIVATE SFML_STATIC)

# GUI Entrypoint
target_link_options(SuperHaxagon PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:WinMainCRTStartup")

# Also distribute OpenAL with the built binary
add_custom_command(TARGET SuperHaxagon POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_DIR}/../../../bin/openal32.dll $<TARGET_FILE_DIR:SuperHaxagon>)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/openal32.dll DESTINATION .)