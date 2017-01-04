<p align="center"><img style="text-align:center" src="./media/rendersmall.png" alt="Banner Render" title="Banner Render"/></p>

# ![Icon](./resource/icon.png "Icon") Super-Haxagon

Super Haxagon is a [Super Hexagon](http://superhexagon.com/) clone for the 3DS. It runs full speed on the old 3DS, new 3DS, and in [citra-emu/citra](https://github.com/citra-emu/citra). You'll need ctrulib, citro3d, and sf2dlib to build this application.
When downloading the source, make sure to clone with `--recursive` to grab all of the included dependencies. Finally, remember
to delete `main.c` from the fonts folder so that this program  will actually compile.

### FAQ

#### Q: How do I download this?
A: Downloads are available on the [releases page](https://github.com/RedInquisitive/Super-Haxagon/releases).

#### Q: How do I download the default music?
A: See the readme in the [sdmc folder](https://github.com/RedInquisitive/Super-Haxagon/tree/master/sdmc).

#### Q: How can I create my own levels?
A: You can use [Haxa-Editor](https://github.com/RedInquisitive/Haxa-Editor/tree/master)! Follow the directions in the README.md of that repository for more details.

#### Q: I made this really cool level, can I show you?
A: Of course you can! If it's really good I'll add it to the game! Please follow the guidelines in [the contributing file](https://github.com/RedInquisitive/Super-Haxagon/blob/master/CONTRIBUTING.md) before you submit your level, though!

#### Q: Can I contribute?
A: Yup, follow the directions [here](https://github.com/RedInquisitive/Super-Haxagon/blob/master/CONTRIBUTING.md)!

#### Q: Can I use my own music/levels?
A: Yes! See the readme in the [sdmc folder](https://github.com/RedInquisitive/Super-Haxagon/tree/master/sdmc) for more details.

#### Q: It says "AWW SNAP!" whenever I launch the game or press B on the main menu!
A: OK, that's not a question, but please look in the root of your SD card for the file "haxapanic.txt" and read it. If it does not answer your question, file a bug report here.

#### Q: Why is there no audio at all?
A: You need to dump your DSP firmware! Normally this is covered when installing A9LH through [3ds.guide](https://3ds.guide/installing-arm9loaderhax#section-vi---finalizing-setup)'s "Section VI - Finalizing setup". If it is not, Launch the Homebrew Launcher from the home menu and select “DSP Dump”.

#### Q: Why did you rewrite the whole thing?
A: Because!

#### Q: Can I play this on my PC?
A: You should buy [Super Hexagon](http://superhexagon.com/) and support the original creator!

#### Q: But I want to play YOUR version on PC!
A: Have you ever heard of [citra-emu/citra](https://github.com/citra-emu/citra)? (Make sure to enable "scaled resolution" for the best experience)!

![Big Citra Picture](./media/haxagon.png "Super Haxagon but in Citra")

### Credits
 * Audio from [Open Hexagon](http://vittorioromeo.info/projects.html)
 * ...And more audio by [Kevin MacLeod](http://incompetech.com/) under [Creative Commons: By Attribution 3.0](http://creativecommons.org/licenses/by/3.0/)
 * Font engine by [flarn2006/SF2DBmpFont](https://github.com/flarn2006/SF2DBmpFont)
 * 2D engine by [xerpi/sf2dlib](https://github.com/xerpi/sf2dlib)
 * 3D engine by [fincs/citro3D](https://github.com/fincs/citro3d)
 * Smea for [the CTR User library](https://github.com/smealum/ctrulib)
 * Actual font: [Bump IT UP](http://fontstruct.com/fontstructions/show/155156/bump_it_up)
 * and all of the people on the [contributors page](https://github.com/RedInquisitive/Super-Haxagon/graphs/contributors) (seriously, thanks!)

### Features

 * Sweet 3D banner
 * Dynamic level loading from both ROMFS and SDMC!
 * Open source AND readable code
 * Shadows
 * Different sided center shapes (from 3 sides to a lot of sides)!
 * Sound using ndsp
  * Supports uncompressed WAV files
 * Walls that move and collide with the player
  * Can collide with the player
  * You will stop moving when you hit the side of walls
 * A functional game over screen
  * Really cool death animation
  * Sound effects
 * Main menu
  * Plays sound effects and animations
 * Support by the community!
 * And much more!
 
### Notes

Contributing to the project will make a better game for everyone! Before you work on a feature, please make sure I know. If you are implementing a new feature, I'd be happy to acccept it! 

# Screenshots

![Title Screen](./media/testmenu_20161225_2335_00002.png "One of the many modes")
![Game Over](./media/testmenu_20161225_2348_00000.png "Game over! Go! Game over! Go!")
![Walls](./media/testmenu_20161225_2336_00003.png "The walls move twoards you!")
![Shapes](./media/testmenu_20161225_2337_00006.png "All shapes and sizes!")
![Shapes](./media/testmenu_20161225_2340_00011.png "Super Pentagon?")

# The Original

![Humble Beginnings](./media/scr_2_MERGED.png "Humble Arrow")
![Humble Beginnings](./media/scr_1_MERGED.png "The First Test")
