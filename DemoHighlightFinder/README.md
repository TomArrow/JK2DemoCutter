
Standalone demo cutter for dm_15 and dm_16 files based on jamme/jomme source code.

Code is lifted and adapted from https://github.com/entdark/jaMME/ and https://github.com/entdark/jk2mp/

All credit for the basic demo cutting functionality goes to entdark. Lots of functions from the Jedi Knight source code releases included as well. 


# DemoHighlightFinder

This tool can help you find highlights in JK2 .dm_15 demos, which can currently be

- **Defrag Runs** (currently supports Razor Defrag and OC Defrag demos)
- **Killing sprees** (3+ kills with either 3,5 or 7 seconds of distance between them)
- **CTF flag returns**
- **CTF captures**
- **Laughter** (3+ chat messages that express strong laughter in a short timeframe, to find funny moments)

## Usage

DemoHighlightFinder is a command line (CLI) tool. You can most comfortably run it using a .bat script. A .bat script is a normal text file with a .bat file ending. 

### Example #1: Finding CTF Flag Returns (Flag Carrier Kills)
Here is a simple working example of how to find CTF returns in an imaginary demo called *gigaowned.dm_15*.

1. Put the demo, *DemoCutter.exe* and *DemoHighlightFinder.exe* in a fresh folder.
2. Create a new .txt text file and rename it to *randomScriptName.bat* (or any other filename with .bat ending). If you cannot see and change the file ending, [enable file extension display](#showFileExt).
3. Open this .bat file in a text editor and type ```DemoHighlightFinder.exe gigaowned.dm_15 10000 ctfreturns``` in the first line.
4. Replace ``10000`` with the amount of milliseconds you wish to have as buffer or "meat" before and after the kills. 10000 milliseconds means 10 seconds.
5. If you want to only find CTF returns from the main perspective of the demo (either you or someone you followed), replace ``ctfreturns`` with ``myctfreturns``
6. Type ```pause``` in the second line. *(This is optional and allows you to see the output instead of closing after it's done)*
7. Save and double click your .bat file.  This will run the analysis of your demo.
8. You should now have 6 new files in your folder:
	- ``highlightExtractionScript.bat``: Contains commands to cut out CTF flag returns (flag carrier kills) that were found.
	- ``highlightExtractionScriptCaptures.bat``: Contains commands to cut out CTF flag captures that were found.
	- ``highlightExtractionScriptDefrag.bat``: Contains commands to cut out Defrag runs that were found.
	- ``highlightExtractionScriptKillSprees.bat``: Contains commands to cut out killing sprees that were found.
	- ``highlightExtractionScriptLaughs.bat``: Contains commands to cut out moments with a lot of laughter that were found.
	- ``killDatabase.db``: SQLite database containing more detailed information about all found highlights and also containing information about all kills that were found in general.
	- ``debugStatsDb.db``: SQLite database containing some statistics that are more for fun than of any real use typically.
9. The new .bat files are likewise just text files. You can open them with a text editor and see that there is a *DemoCutter.exe* command in there for each highlight that was found. You can edit the .bat files to remove uninteresting highlights to only keep the ones you are interested in, if you wish.
10. Execute the .bat file containing the highlights you are looking for. This will create cut demos in your folder containing the highlights.
11. You are done. 

## Understanding cut demo filenames
### CTF Returns
A few examples of how CTF Return demo filenames look like:
- ``ctf_yavin___RET_DBS_345u___^0SuperNerd___^7SadMan___387_269ups___thirdperson_7_3_fea``
- ``ctf_yavin___RET_DOOM_~_DBS_576u___^8MegaWaifu___^8RarePepe___619_1245ups_12_11_334``
- ``ctf_yavin___RET_RED_G99___^7^7CringeBoomer^1___^5TrashLord___283_667ups___thirdperson_16_11_352``

This is what the filename contains:

1. Map name.
2. Kill method. It's RET_ followed by the saber move/weapon/means of death. 
	- **RET_RED** means the kill was a red slash. 
	- **RET_DOOM_~_DBS** means it was a doom kill, and at the time of the actual death, the killer was in a DBS saber move. Since some time passes between boost and actual death to fall damage, consider this a best guess.
	- **RET_DBS_345u**: For special attacks like DBS, there is a suffix indicating the speed in UPS (units per second) that the killer had at the time of triggering the dbs (not at the time of the kill).
	- Some of the possible saber moves are: 
		- **RED** (red slash)
		- **YEL** (yellow slash)
		- **BLU** (blue slash)
		- **DBS** (double-handed backstab)
		- **BS** (backslash)
		- **DFA** (death from above)
		- **YDFA** (yellow DFA)
		- **BLUBS** (blue backstab)
	- If there is a suffix like **_G99**, this means that the saber move was a guess along with a probability in percent. This is due to players that aren't followed missing saber move information, so it has to be guessed from the animation.
3. Name of the killer.
4. Name of the victim.
5. The **___283_667ups** part denotes the maximum speed of the killer in the past 1 second as well as the maximum speed of the victim in the past 1 second *(excluding speed after death to not count boost that was a result of the kill)*
6. If the killer was not followed/spectated, **___thirdperson** will be appended. This means that the kill is not from the killer's perspective. But you can try to use ``DemoReframer.exe`` to change the angle (with some limitations)
7. At the end you see something like **_16_11_3a2**. The first number is the client number of the killer. The second is the client number of whoever recorded the demo. The third is a 3 letter hash code. With this, you can tell whether two kils are distinct kills or just the same kill from two angles. If it is the same kill from two angles, the hash will be identical.


## FAQ
### <a name="showFileExt"></a>I cannot change the textfile extension to .bat because I don't see the .txt extension

If you use Windows 10, in Explorer, go to the View tab and look for the "File name extensions" checkbox in the "Show/Hide" subcategory and tick it.

On older Windows versions, go into the folder and press the Alt button on your keyboard. This will bring up some menu options on top. Go to Tools->Folder Options. Navigate to the "View" tab and search for the "Hide extensions for known file types" checkbox. Untick it. Press the "Apply" button at the bottom and then "OK".


