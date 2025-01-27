#!/bin/bash
# Run from repository root
# ex: ./tools/linux/flatpak.sh
flatpak-builder --force-clean --user --install-deps-from=flathub --repo=repo --install build/flatpak net.awalter.SuperHaxagon.yml
flatpak build-bundle repo build/net.awalter.SuperHaxagon.flatpak net.awalter.SuperHaxagon
flatpak run net.awalter.SuperHaxagon
