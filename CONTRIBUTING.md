# How to contribute to Super Haxagon
First of all, thanks for considering a contribution to the open-source clone of the game Super Hexagon!
In order to be as organized as possible, please follow these guidelines before contributing to the game:

Contributions can include bug reports, new features, additions to the wiki, fixes for existing bugs, or 
refactoring of existing code. If you are considering doing any one of these things, continue reading!

## Bug Reports

Bug reports for Super Haxagon require different things depending on the type of bug report. Here are some guidelines:

#### For the "AWW SNAP!" screen:
This occurs when a known issue is encountered in code. The file haxapanic.txt at the root of your SD card will tell you
information about what happened and how to solve it. Most of the time, AWW SNAP! errors occur during the loading of  
levels from the SD card. If you are a level developer, follow the instructions in haxapanic.txt. 

If you still have no idea what to do, create a new issue and upload your levels.haxagon,  levels.xml, and haxapanic.txt file,
and I'll work with you personally to resolve your issue.

#### For crashes or freezes:
If your 3DS locks up during runtime, and there is no information regarding the crash left on your SD card, please be
AS DETAILED AS POSSIBLE when filing a bug report. Unfortunately, "it broke" is not detailed enough. Something like "in the
ROMFS level "HEXAGONER", colliding with a wall causes the console to crash after playing for 30 minutes" would be better.

#### For anything with Citra:
Super Haxagon should always be playable on Citra! If it is not, please provide a screenshot, a description of the problem,
and any relevant output from the console.  

#### For something that’s impossible:
It happens! I might have created a level that’s actually impossible. For a glitch like this, take a screenshot in Citra
of the impossible pattern, or record a video on your phone. I'll try to reproduce it and re-configure the levels
as needed. Any detailed descriptions, level names, pattern names, or anything else would also be appreciated! 

## Pull requests

Anything that requires a pull request should be discussed in the issues tab before working on code.
This assures that everyone knows what people are working on so there are no merge conflicts, 
double-contributions, or rewrites of existing code that works fine. Here is a simple guideline for
all pull requests:

1. Create an issue! Tell everyone what you are working on before you do it!
2. Wait for me to respond! If I'm no longer available, I'll never be able to merge your pull request :cry:
3. Do it!  (and try to follow the current coding style)!
