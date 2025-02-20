#!/bin/bash
# The monkeyx/retro_builder:arm64 requires qeumu-aarch64-static for arm emulation.
# This will not spin up a VM, but instead natively interprate the binaries inside
# the docker container live. This is very similar to the "chroot" method, just
# using Docker as the chroot instead.

# With that said, I am using "podman" for building these images. Things should work
# mostly the same, with the command `docker` swapped for `podman`. This .sh script
# will set up the environment for podman.

# This script requires root to enable the kernel flags for ARM emulation, but the
# actual build will not require it.

sudo podman run --rm --privileged multiarch/qemu-user-static --reset -p yes

# At this point you may need to reboot your machine.
# Don't re-run this script, just re-run the below command
# Re-running the command to set up qemu-user-static seems to break things.

podman run --rm --arch arm64 ubi9/ubi uname -m

# If aarch64 printed out, you are good to go!
