# cmake

# This cmake file is totally and completely untested!
# If you try it out, please report your results in a new
# issue! I'd like to know if it works! Thanks!

# If you need to manually specify directories to find SFML,
# please check out the `-D<var>`s in the tools/linux/build.sh
# file.

message(STATUS "Attempting to build for macOS...")

set(PLATFORM_NAME "MACOS")


set(DRIVER_PLATFORM source/Driver/macOS/PlatformMacOS.cpp source/Driver/Common/PlatformIsFullyRandom.cpp)

include(cmake/common/sfml.cmake)
