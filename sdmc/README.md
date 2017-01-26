# The SD card

### The default music for the game can be downloaded from the folder above!
(that is `/sdmc/3ds/data/haxagon/`)!

### Music format
 * WAV fle
 * unsigned 16 bit PCM
 * must be mono
 * all metadada must be removed from the file
  * you can do this by pressing clear when exporting from audacity
  
### Other Information

Music, save files, and extra add-on levels now exist in the SD card instead of the romfs. Your custom music will be loaded from this location instead.

For example, a level with the custom music name "test.wav" will be loaded from sdmc:/3ds/data/haxagon/test.wav

### Here's a list of files that exist on the SD card:

| File Name | Description |
|-----------|-------------|
|`sdmc:/3ds/data/haxagon/<NAME>.wav`     | A background music file |
|`sdmc:/3ds/data/haxagon/levels.haxagon` | A level file loaded by the game when B is pressed  on the  main menu |
|`sdmc:/3ds/data/haxagon/dataromfs.db`   | A database of the scores for built in levels |
|`sdmc:/3ds/data/haxagon/datasdmc.db`    | A database of the scores for SDMC based custom levels.<br>Warning: deleting a level from the SD card will also delete the score here! |
