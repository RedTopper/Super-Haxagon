# SuperHaxagon for macOS (SFML with Brew)

SuperHaxagon is a cross-platform, open source [Super Hexagon](http://superhexagon.com/) clone.

You can obtain a free copy of the source code licensed under the GPLv3 at:

https://github.com/RedTopper/Super-Haxagon

## Install

This build was created without signing and depends on `brew` libraries.

To run this build, please install `brew` along with the following components:

```bash
brew install openal-soft
brew install sfml@2
```

You will also need to place a copy of the `romfs` folder next to the application.

The application will look in the current working directory for the assets. If you
run the application from Finder, the current working directory will not be set 
properly and **the assets will not be found**. Instead, launch the app through 
Terminal with `./SuperHaxagon`.

