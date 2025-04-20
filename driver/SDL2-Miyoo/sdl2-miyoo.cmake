# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# cmake -DCMAKE_TOOLCHAIN_FILE=driver/Miyoo/cmake/MiyooMini-TC.cmake ..

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
    driver/All/source/PlatformIsFullyRandom.cpp
    driver/SDL2/source/MusicSDL2.cpp
    driver/SDL2/source/SoundSDL2.cpp
    driver/SDL2/source/FontSDL2.cpp
    driver/SDL2-Miyoo/source/ScreenMiyooMini.cpp
    driver/SDL2-Miyoo/source/PlatformMiyooMini.cpp
)

add_executable(${PROJECT_NAME} ${DRIVER} ${SOURCES})

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image SDL2_mixer SDL2_ttf GLESv2 EGL json-c neon)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)

if(EXISTS "/root/lib.tar.gz")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E tar x "/root/lib.tar.gz" WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    install(DIRECTORY ${CMAKE_BINARY_DIR}/lib DESTINATION ./Roms/PORTS/Games/${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ./Roms/PORTS/Games/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/romfs DESTINATION ./Roms/PORTS/Games/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/resources/ DESTINATION ./Roms/PORTS)

set(LICENSE_PATH "./Roms/PORTS/Games/${PROJECT_NAME}")
set(LICENSE_DRIVER "${CMAKE_SOURCE_DIR}/driver/SDL2/LICENSE.md")

set(README_PATH ".")
set(README_DRIVER "${CMAKE_SOURCE_DIR}/driver/SDL2-Miyoo/INSTALL.md")
