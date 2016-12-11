# ![Icon](./resource/icon.png "Icon") Super-Haxagon

<div style="text-align:center"><img src="./media/rendersmall.png" alt="Banner Render" title="Banner Render"/></div>

This is a Super Hexagon clone for the Nintendo 3DS. This should run on the o3ds, n3ds, and Citra homebrew environments. The Makefile also creates a CFW compatible CIA too, for those who are into that kind of stuff. A banner is included.

### Download

Downloads are available from the [releases page](https://github.com/RedInquisitive/Super-Haxagon/releases).

Contributing to the project will help create a better game for everyone! If you would like to help, feel free to create a pull request. I know for a fact I messed up somewhere in the code here. If you are implementing a new feature, I'd be happy to acccept it! 

### Notes

In order for audio to work you must dump your DSP firmware. Normally, this is covered when installing A9LH through [3ds.guide](https://3ds.guide/installing-arm9loaderhax#section-vi---finalizing-setup)'s "Section VI - Finalizing setup". If it is not, Launch the Homebrew Launcher from the home menu and select “DSP Dump”.

### Features

 * Sweet 3D banner
 * 6 selectable levels
 * Open source AND readable code
 * Sound using ndsp
  * Supports uncompressed WAV files
 * Level Editor written in Java
  * Can run user code to generate patterns
  * Can append other pattern files
  * Can edit patterns and create patterns directly
 * Walls that move and collide with the player
  * Can collide with the player
  * You will stop moving when you hit the side of walls
 * A functional game over screen
  * Really cool death animation
  * Sound effects
 * Level patterns dynamically loaded from a .LEVE file.
  * 8 distinct patterns (so far!)
 * Main menu
  * Plays sound effects and animations
 * Support by the community!
  * Audio from [Open Hexagon](http://vittorioromeo.info/projects.html)
  * Fonts via [flarn2006/SF2DBmpFont](https://github.com/flarn2006/SF2DBmpFont)
  * Render from [xerpi/sf2dlib](https://github.com/xerpi/sf2dlib)
  * And many more!

# Screenshots

![Game Over](./media/scr_7_MERGED.png "NOT AGAIN!")
![Walls](./media/scr_6_MERGED.png "The walls move twoards you!")
![Spirallll!](./media/scr_4_MERGED.png "Spiralllll!")
![Music](./media/scr_5_MERGED.png "Music!")
![Title Screen](./media/scr_3_MERGED.png "One of the many modes")
![Humble Beginnings](./media/scr_2_MERGED.png "Human Arrow")
![Humble Beginnings](./media/scr_1_MERGED.png "Screenshot")

