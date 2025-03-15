# cmake

message(STATUS "Building for windows...")

set(PLATFORM_NAME "WINDOWS")

set(DRIVER_PLATFORM source/Driver/Win/PlatformWin.cpp)

# SFML sets the warning level to /W4 and Visual Studio sets it to /W3.
# Clear the default flags (for ALL targets) then add /W4 back to our target later.
string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

include(cmake/common/sfml.cmake)

# GUI Entrypoint
target_link_options(SuperHaxagon PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:WinMainCRTStartup")

target_compile_options(SuperHaxagon PRIVATE "/W4")

# Also distribute OpenAL with the built binary
add_custom_command(TARGET SuperHaxagon POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:SuperHaxagon>
)

install(FILES $<TARGET_FILE_DIR:SuperHaxagon>/openal32.dll DESTINATION .)
