# macOS Unsigned Unix Application Build

> [!NOTE]  
> This information does not apply to the SuperHaxagon.app builds!
> For those builds, simply Ctrl-Click on the app, select "Open", then allow the app to run.

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

You can also download a signed (but not notarized) version of the application via
the Releases page.
