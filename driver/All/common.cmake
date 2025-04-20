# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/include")

include(driver/All/cmake/git.cmake)

configure_file(${CMAKE_CURRENT_LIST_DIR}/include/Driver/Tools/Configuration.hpp.in include/Driver/Tools/Configuration.hpp)
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/VERSION.txt.in VERSION.txt)
