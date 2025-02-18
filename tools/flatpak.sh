#!/bin/bash
# Run from repository root
# ex: ./tools/flatpak.sh

set -e

flatpak-builder --force-clean --user --install-deps-from=flathub --repo=build/repo --install build/flatpak net.awalter.SuperHaxagon.yml
flatpak build-bundle build/repo build/net.awalter.SuperHaxagon.flatpak net.awalter.SuperHaxagon
flatpak run net.awalter.SuperHaxagon
