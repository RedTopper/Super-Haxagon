# macOS Driver

This driver supports macOS with SFML2 as a backend.

## Building

### With brew

You'll need the following tools:

 * xcode https://developer.apple.com/xcode/
 * cmake https://cmake.org/download/ with the command-line tools installed
 * brew https://brew.sh/

Then, install SFML's dependencies:

```bash
brew install openal-soft
brew install sfml@2
```

Finally, run the following commands from the repo root:

```bash
mkdir build
cd build
cmake -DCMAKE_FIND_FRAMEWORK=LAST ..
make -j4
```

This should build a command-line version of SuperHaxagon linked to the brew dependencies.
Run from the terminal with `./SuperHaxagon`

### As a bundle/.app:

You'll need the following tools:

* xcode https://developer.apple.com/xcode/
* cmake https://cmake.org/download/ with the command-line tools installed

Finally, run the following commands from the repo root:

```bash
mkdir build
cd build
cmake  -DBUILD_BUNDLE=TRUE ..
make -j4
```

This should generate a .app bundle you can copy into your `Applications` folder. 
Please note, this build will not be signed. To sign, please generate a certificate
via xcode and then run the following codesign commands:

```bash
security find-identity
codesign --deep --force -s "<certificate>" --options runtime SuperHaxagon.app -v
```

Where <certificate> is a certificate from the `security` command.

## License

This driver follows the license scheme of the parent driver at `driver/SFML`.
