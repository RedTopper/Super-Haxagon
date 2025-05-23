# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

if (NOT DEFINED README_PATH)
    set(README_PATH .)
endif()

if (NOT DEFINED VERSION_PATH)
    set(VERSION_PATH "${README_PATH}")
endif()

if (NOT DEFINED LICENSE_PATH)
    set(LICENSE_PATH "licenses")
endif()

if (NOT DEFINED README_DRIVER)
    message(FATAL_ERROR "Driver does not provide a README.md with README_DRIVER!")
endif()

if (NOT DEFINED LICENSE_DRIVER)
    message(FATAL_ERROR "Driver does not provide a LICENSE.libs.md with LICENSE_DRIVER!")
endif()

install(FILES "${README_DRIVER}" RENAME "README.md" DESTINATION ${README_PATH})
install(FILES "${LICENSE_DRIVER}" RENAME "LICENSE.libs.md" DESTINATION ${LICENSE_PATH})
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.md" DESTINATION ${LICENSE_PATH})
install(FILES "${CMAKE_SOURCE_DIR}/CREDITS.md" DESTINATION ${LICENSE_PATH})
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/VERSION.txt" RENAME "v${CMAKE_PROJECT_VERSION}.txt" DESTINATION ${VERSION_PATH})
