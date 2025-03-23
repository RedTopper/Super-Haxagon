# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

configure_file(${CMAKE_CURRENT_LIST_DIR}/include/Driver/Tools/Configuration.hpp.in include/Driver/Tools/Configuration.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/include")
