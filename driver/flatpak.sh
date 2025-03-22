#!/bin/bash

# Builds and tests the flatpak.
# Run from this directory.

set -e

cd ..

flatpak-builder --force-clean --user --install-deps-from=flathub --repo=build/repo --install build/flatpak net.awalter.SuperHaxagon.yml
flatpak build-bundle build/repo release/net.awalter.SuperHaxagon.flatpak net.awalter.SuperHaxagon
flatpak run net.awalter.SuperHaxagon
