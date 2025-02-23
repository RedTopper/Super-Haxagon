# cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/Miyoo-TC.cmake ..

message(STATUS "Building for miyoo mini...")

set(PLATFORM_NAME "MIYOO MINI")

# The Miyoo Mini uses software rendering for triangles.
#
# I found that rendering at 1/4 the resolution helped speed things up,
# but I left it as an option for those who are adventurous to toggle
# the line below to use the entire screen for rendering!
#
# Also, I kinda like the pixel filter anyway. Ends up looking retro.
set(RENDER_QUARTER_RES TRUE)

set(DRIVER
    source/Driver/Common/SDL2/MusicSDL2.cpp
    source/Driver/Common/SDL2/SoundSDL2.cpp
    source/Driver/Common/SDL2/FontSDL2.cpp
    source/Driver/MiyooMini/ScreenMiyooMini.cpp
    source/Driver/MiyooMini/PlatformMiyooMini.cpp
    source/Driver/Common/PlatformIsFullyRandom.cpp
)

add_executable(SuperHaxagon ${DRIVER} ${SOURCES})

target_link_libraries(SuperHaxagon SDL2 SDL2_image SDL2_mixer SDL2_ttf GLESv2 EGL json-c neon)

target_compile_options(SuperHaxagon PRIVATE -Wall -Wextra -pedantic)

install(TARGETS SuperHaxagon RUNTIME DESTINATION ./SuperHaxagon)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION ./SuperHaxagon)
install(FILES
        ${CMAKE_SOURCE_DIR}/media/miyoo/config.json
        ${CMAKE_SOURCE_DIR}/media/miyoo/launch.sh
        ${CMAKE_SOURCE_DIR}/media/miyoo/icon.png
        DESTINATION ./SuperHaxagon
)
