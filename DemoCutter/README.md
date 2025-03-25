
Standalone demo cutter for dm_15 and dm_16 files based on jamme/jomme source code.

Code is lifted and adapted from https://github.com/entdark/jaMME/ and https://github.com/entdark/jk2mp/

All credit for the basic demo cutting functionality goes to entdark. Lots of functions from the Jedi Knight source code releases included as well. 

# DemoCutter

This tool can help you cut out smaller parts ouf of longer demos. 

## Usage

DemoHighlightFinder is a command line (CLI) tool. You can most comfortably run it using a .bat script. A .bat script is a normal text file with a .bat file ending. 

### Example #1: Cutting cut the demo segment between 10 and 20 seconds of the original demo
Here is a simple working example of how to cut the demo segment between 10 and 20 seconds of an imaginary demo called *gigaowned.dm_15* and save that segment as *gigaowned_cutout.dm_15*.

1. Put the demo and *DemoCutter.exe* in a fresh folder.
2. Create a new .txt text file and rename it to *randomScriptName.bat* (or any other filename with .bat ending). If you cannot see and change the file ending, [enable file extension display](#showFileExt).
3. Open this .bat file in a text editor and type ```DemoCutter.exe "gigaowned.dm_15" "gigaowned_cutout" 10000 20000``` in the first line.
4. Replace ``gigaowned.dm_15`` with the actual name of your own demo.
5. Replace ``gigaowned_cutout`` with the actual name of (not yet existing) cut demo you want. (Don't add *.dm_15* for the output filename, it's added automatically based on the input demo)
6. Replace ``10000`` and ``20000`` with the amount of milliseconds you wish to start and end the cutting. 10000 milliseconds means 10 seconds as the start time here, 20000 milliseconds here means 20 seconds as the end time.
7. Type ```pause``` in the second line. *(This is optional and allows you to see the output instead of closing after it's done)*
8. Save and double click your .bat file.  This will run the analysis of your demo.
9. You should now have *gigaowned_cutout.dm_15* (or a demo of your chosen name as per point 5) in your folder. 
10. You are done.

### Cut points

There are various ways to provide in- and out- cut times to DemoCutter.

#### Types of cut point times

By default, the times you give to DemoCutter.exe are millisecond amounts and determine the time into the actual demo. 0 is the start of the input demo, 10000 is 10 seconds from the start of the input demo etc. This is referred to as "demo time".

DemoCutter supports different kinds of input times:

- Demo time: Default, no need to specify, however can specify by adding a "d" before the time, e.g. *d10000*
- Game time: This is the time you see ingame in the corner when cg_drawTimer 1 is on. You can specify this with "g" in front of the time, e.g. "g10000" would mean at 10 seconds ingame time, regardless how far into the demo it is.
- Game time (paused): This applies only to NWH demos. Normally the game time specifier will skip over pauses and ignore them. This needs to be a separate type because the game time is reset after a pause ends. If you wish to specify a time DURING a pause, use the "gp" prefix, e.g. "gp15000" would be at the game time of 15 seconds **while paused**. Note that this is a bit fucky and won't work reliably if multiple pauses ended up sharing the same game time due to being shortly after one another. 
- Server time: This can cut based on serverTime. You can see this time with cg_drawSnapshot 1 ingame. It is unaffected by pauses and more of a nice reference time. You can specify a servertime with the "s" prefix, e.g. "s10000" will hit when the *time* value in the cg_drawsnapshot 1 display hits 10000 (10 seconds). 

DemoCutter accepts two times. An in-time and out-time. In-time is the time the cut begins, out-time is the time the cut ends. You can mix different kinds of time types with no issue. You can specify an in-demotime and an out-servertime or out-gametime, etc. The only requirement is that the out-time is after the in-time, otherwise you end up with an empty output demo.

#### Time formats

Up until now, you've learned how to cut based on millisecond timings. However since this isn't very comfortable, there are other ways to specify the time. Here is all time formats that are supported:

- Milliseconds: To specify in milliseconds, simply use the amount of milliseconds, nothing else needs to be specified.
- Minutes and seconds: To specify a time in minutes and seconds, write it like *34:21* for 34 minutes and 21 seconds.
- Minutes, seconds and milliseconds: To specify a time in minutes, seconds and milliseconds, write it like *34:21.246* for 34 minutes, 21 seconds and 246 milliseconds.

This applies to all types of time (demo time, game time, game time (paused), server time etc).

For example: *g23:02* would do the cut at 23 minutes and 2 seconds into the game, where as *d01:23* would cut at 1 minute and 23 seconds into the demo. See part above for the different types of time specifiers.

#### Multi-game demos

If you have a demo that spans across multiple games, and you wish to specify a game time or server time, you will run into trouble, because gameTime is reset when a new round starts and serverTime can be reset at least during map reloads/map changes. 

There is a workaround, but unless you specifically remember the amount of new round restarts, there might be try-and-error to get the desired result. 

Let's say you started the demo recording during the first game and kept it running throughout the second game. You now want to cut the segment between 4:30 minutes and 5:15 minutes from the second game (assuming the second game was directly after the first game with no additional restarts). So you specify the following time points: *g4:30skip1 g5:15skip1*.

Basically you add *skip* at the end of the time and then you add the amount of map-restarts to skip.


## FAQ
### <a name="showFileExt"></a>I cannot change the textfile extension to .bat because I don't see the .txt extension

If you use Windows 10, in Explorer, go to the View tab and look for the "File name extensions" checkbox in the "Show/Hide" subcategory and tick it.

On older Windows versions, go into the folder and press the Alt button on your keyboard. This will bring up some menu options on top. Go to Tools->Folder Options. Navigate to the "View" tab and search for the "Hide extensions for known file types" checkbox. Untick it. Press the "Apply" button at the bottom and then "OK".
