
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
4. Replace ``gigaowned.dm_15`` with the actual name of your own demo.
5. Replace ``10000`` with the amount of milliseconds you wish to have as buffer or "meat" before and after the kills. 10000 milliseconds means 10 seconds.
6. If you want to only find CTF returns from the main perspective of the demo (either you or someone you followed), replace ``ctfreturns`` with ``myctfreturns``
7. Type ```pause``` in the second line. *(This is optional and allows you to see the output instead of closing after it's done)*
8. Save and double click your .bat file.  This will run the analysis of your demo.
9. You should now have 6 new files in your folder:
	- ``highlightExtractionScript.bat``: Contains commands to cut out CTF flag returns (flag carrier kills) that were found.
	- ``highlightExtractionScriptCaptures.bat``: Contains commands to cut out CTF flag captures that were found.
	- ``highlightExtractionScriptDefrag.bat``: Contains commands to cut out Defrag runs that were found.
	- ``highlightExtractionScriptKillSprees.bat``: Contains commands to cut out killing sprees that were found.
	- ``highlightExtractionScriptLaughs.bat``: Contains commands to cut out moments with a lot of laughter that were found.
	- ``killDatabase.db``: SQLite database containing more detailed information about all found highlights and also containing information about all kills that were found in general.
	- ``debugStatsDb.db``: SQLite database containing some statistics that are more for fun than of any real use typically.
10. The new .bat files are likewise just text files. You can open them with a text editor and see that there is a *DemoCutter.exe* command in there for each highlight that was found. You can edit the .bat files to remove uninteresting highlights to only keep the ones you are interested in, if you wish.
11. Execute the .bat file containing the highlights you are looking for. This will create cut demos in your folder containing the highlights.
12. You are done. 

### Example #2: Finding CTF Flag Captures

Exactly the same as Example #1, but you will be instead interested in the generated file ``highlightExtractionScriptCaptures.bat`` instead of ``highlightExtractionScript.bat``

### Example #3: Finding defrag runs

Exactly the same as Example #1, but you leave away the ``myctfreturns`` or ``ctfreturns`` part of the analysis command. And you will be instead interested in the generated file ``highlightExtractionScriptDefrag.bat`` instead of ``highlightExtractionScript.bat``

*Note*: By default, defrag runs that weren't detected to be top 10 runs are not searched for. This is the case if the time distance to the best time on the map is not zero, and the server does not output a "Top 10 time" message. This can be the case for runs that aren't the runner's personal best time (even if their time might fall within top 10, since this can't be detected), or for runs that weren't logged and weren't the number 1 time either.

If you want to search for absolutely all runs, put ``alldefrag`` in the command at the same place that ``myctfreturns`` or ``ctfreturns`` would have been put (see example #1).

### Example #4: Finding killing sprees

Exactly the same as Example #1 and it doesn't matter if you specify ``myctfreturns`` or ``ctfreturns`` or not. Killing sprees are always searched for and found for all visible players and saved in ``highlightExtractionScriptKillSprees.bat``.

### Example #5: Finding laugh sequences

Exactly the same as Example #1 and it doesn't matter if you specify ``myctfreturns`` or ``ctfreturns`` or not. Laugh sequences are always searched for and found and saved in ``highlightExtractionScriptLaughs.bat``.

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
	- **RET_RED_BF** means the kill was a red slash preceded by a backflip. Any **_BF** suffix on saber kills means it was preceded by a backflip. 
      - Specifically, the backflip must have happened less than 1000ms (1 second) ago and must not have been interrupted by touching the ground.
	- **RET_DOOM_~_DBS** means it was a doom kill, and at the time of the actual death, the killer was in a DBS saber move. Since some time passes between boost and actual death to fall damage, consider this a best guess.
	- **RET_WEIRD** means the cause of death can't be determined. For example if one player's sentry explodes and another player dies from it. It can come with a guess as well, like DOOM.
	- **RET_TELE** is one of those "player crushed player 2 into another dimension" kind of things. If someone spawns right where you are for example.
	- **RET_DBS_345u**: For special attacks like DBS, there is a suffix indicating the speed in UPS (units per second) that the killer had at the time of triggering the dbs (not at the time of the kill).
	- **RET_YEL_BST1A**: A **_BST** suffix indicates a boost that preceded the kill:
      - Boosts can be either boosts of the attacker or of the victim. For example
        - **_BST1A** would indicate 1 boost of the attacker.
        - **_BST2V** would indicate 2 boosts of the victim.
      - The detection of boosts is experimental and it might detect things that you don't consider to be a boost. Someone might be thrown against a wall from a slash and immediately lose his speed, but it will be detected as a boost anyway.
      - Boosts are counted if they happened less than 5000ms (5 seconds) before a kill.
      - If walking (moving on the ground below highest possible walk speed) is detected, it will invalidate boosts before the walk ***if*** the detected walking happened 1000ms (1 second) or longer before the kill. This is because you could be boosted towards someone but walk for half a second then before doing your kill. IT's still a valid detection.
	- **RET_MINE** or **RET_MINE_AIR**: Mine kill. If the **_AIR** suffix is present, the mine was in the air when it hit the target/exploded, as opposed to sticking to a floor/wall at the time of explosion.
	- Some of the possible saber moves are: 
		- **RED** (red slash)
		- **YEL** (yellow slash)
		- **BLU** (blue slash)
		- **DBS** (double-handed backstab)
		- **BS** (backslash)
		- **DFA** (death from above)
		- **YDFA** (yellow DFA)
		- **BLUBS** (blue backstab)
		- **IDLE** (death from touch or "burn" without actual swing)
	- If there is a suffix like **_G99**, this means that the saber move was a guess along with a probability in percent. This is due to players that aren't followed missing saber move information, so it has to be guessed from the animation.
3. Name of the killer.
4. Name of the victim.
5. The **___283_667ups** part denotes the maximum speed of the killer in the past 1 second as well as the maximum speed of the victim in the past 1 second *(excluding speed after death to not count boost that was a result of the kill)*
6. If the killer was not followed/spectated, **___thirdperson** will be appended. This means that the kill is not from the killer's perspective. But you can try to use ``DemoReframer.exe`` to change the angle (with some limitations)
7. At the end you see something like **_16_11_3a2**. The first number is the client number of the killer. The second is the client number of whoever recorded the demo. The third is a 3 letter hash code. With this, you can tell whether two kils are distinct kills or just the same kill from two angles. If it is the same kill from two angles, the hash will be identical.

### CTF Captures
A few examples of how CTF capture demo filenames look like:
- ``ctf_yavin___CAPTURE1K1R___000-28-350___^8Benzos___P0T1___0DANGER14___1133_478ups_9_17``
- ``ctf_yavin___CAPTURE___000-26-614___^8GenericName___P0T1___4DANGER37___932_450ups_9_17``
- ``ctf_batcherzone___CAPTURE2K___001-19-247___^5^5^4Whatever___P2T1___15DANGER65___938_381ups___thirdperson_3_17``
- ``ctf_yavin___CAPTURE___001-14-781___^2hello___P0T2___0DANGER0___403_244ups___thirdperson_0_1``

This is what the filename contains:
1. Mapname
2. **CAPTURE** keyword. If the capper got any rets/kills himself, this will be appended, for example:
    - **CAPTURE2K** means the capper got 2 kills while holding the flag.
    - **CAPTURE1K1R** means the capper got 1 kill while holding the flag, out of which 1 was a ret (kill of enemy flag carrier).
3. Duration of the flag hold.
4. Name of the flag holder.
5. A string like **P0T1** denoting the flag pickup origin and the flag team:
    - **P0** means the flag was picked up from the enemy pad.
    - **P2** means the flag was picked up while it was dropped (a "swoop" perhaps, or an intentional drop on a teammate through fall damage).
    - **T0** means the flag belonged to the red team.
    - **T1** means the flag belonged to the blue team.
6. A string like **15DANGER65** approximating in how much danger the capper was throughout the run.
	- The first number is the percentage of time of the flag hold that more than 1 enemy was very close to the capper.
    - The second number is the average amount of players that were very close to the capper throughout the flag hold, multiplied by 100.
    - *Note*: "very close" means closer than 300 units.
    - So for the example of *15DANGER65*, this means that 15% of the time, there was more than 1 enemy that was very close. And on average, 0.65 enemies were very close.
7. A string like **1133_478ups** denoting the maximum and average speed of the capper throughout the flag hold time in units per second.
8. Optional: **___thirdperson** is appended if the flag carrier was not followed throughout the entire run.
9. A string like **_9_17** indicating the client number of the capper, as well as the client number of the demo recorder.

### Killing sprees
A few examples of how killing spree demo filenames look like:
- ``ffa_bespin___KILLSPREE7000_3___^1tom___14_8_5___662_267ups___thirdperson3___3_6_4ac``
- ``ctf_yavin___KILLSPREE3000_4___^1^1[ I ] ^8^8^1ANDREW___2_3_11_6___502_649ups___thirdperson4___15_17_70f``
- ``ctf_yavin___KILLSPREE5000_4R2E1___^3^3^3[aCd] Interlude___2_11_5_1___616_670ups___thirdperson4___16_3_e2e``
- ``ffa_bespin___KILLSPREE5000_3___The ^0^0^7Legged ^1^1^0One___3_9_7___894_511ups___10_6_2dc``

This is what the filename contains:
1. Mapname
2. **KILLSPREE** keyword followed by the maximum time in milliseconds between each kill (maximum 7000 milliseconds = 7 seconds).
	- *For clarification, an example*: If there are 3 kills with 2.5 second distances and a fourth kill with a 4.5 second distance to the last kill, then you will get two separate killing spree demos. One **KILLSPREE3000** demo with the first 3 kills and one **KILLSPREE5000** demo with all 4 kills.
    - *Good to know*: 3000 milliseconds (3 seconds) is the limit for displaying a kill in the centerprint on newer JK2 clients that show multiple centered stacked kill messages. Hence, all demos with **KILLSPREE3000** in the name should show all the kills of the killspree stacked on the screen.
3. String like **_4R2E1** denoting count of kills and kills of different types
	- The first number is the total number of kills.
    - If special kill types are among those kills, they are appended as follows:
		- **R2** would indicate that 2 of the kills were CTF flag carrier kills (rets).
        - **D1** would indicate that 1 of the kills was a doom kill. 
        - **E1** would indicate that 1 of the kills was through explosion damage, like mines.
4. The name of the killer.
5. A string like **___3_9_7** denoting the client numbers of the players that were killed.
6. A string like **___894_511ups** denoting the maximum speed of the killer throughout the killspree as well as the highest speed of any of the victims in the 1 second leading up to their deaths.
7. Optionally: A string like **___thirdperson4** denoting how many of the kills the killer was not followed for (read: was visible from another player's perspective instead). **___thirdperson4** would indicate that 4 of the kills were not recorded from the killer's perspective.
8. A string like **___16_3_e2e** denoting the client number of the killer, the client number of the demo recorder and a 3 letter hash, same as normal kills/rets. If two killsprees have the same hash and same details, they are most likely the same killspree recorded from multiple angles (or you had duplicate demos).

### Defrag runs
A few examples of how defrag demo filenames look like:
- ``climbit2___015-31-680___hella___top10___thirdperson_1_11``
- ``dfmp2_maze___000-38-050___mommy_3_4``
- ``acrobat-dash2___000-25-860___^7ferditheant___unlogged___thirdperson_3_1``
- ``oc1_Chronos_cpmrun1___000-15-570___^1superRunner___top10_4_1"``

This is what the filename contains:
1. Mapname.
2. Time of the run.
3. Playername (*not* the username on the defrag server, this can't be detected)
4. Optional: **___top10** if the time was not a #1 (world record) run, but was still a top 10 time.
5. Optional: **___unlogged** if the time was not a recorded top 10 time or **___unloggedWR** if the time was not recorded, but was detected to be faster or equally fast to the WR.
	- *Note*: Older versions do not support the **___unloggedWR** functionality, so for those it is not possible to distinguish from the filename whether an unlogged run was a number 1 run or not.
6. Optional: **___thirdperson** if the run wasn't recorded fully from the runner's perspective.
7. A string like **_4_1** denoting the client number of the runner and the client number of the demo recorder.

### Laughs
A few examples of how laugh sequence demo filenames look like:
- ``ffa_bespin___LAUGHS2_4300_rofl,hahaha_6``
- ``ctf_yavin___LAUGHS2_980_XD,rofl_8``
- ``ffa_bespin___LAUGHS3_4700_LOL,XD,OMG_15``
- ``ctf_yavin___LAUGHS3_360_LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOL,LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOL,LOOOOO--_10``

This is what the filename contains:
1. Mapname.
2. **LAUGHS** keyword specifying the amount of chats detected as laughter. **LAUGHS2** for example means 2 chat messages that express laughter.
3. Duration of the laughter spree. Specifically, the time distance between the first and last detected laugh.
4. The laughs themselves, separated by comma. If the laughs would cause filenames to become too long, this is cut off with a **--** at the end. 
5. The demo recorder client number.

## FAQ
### <a name="showFileExt"></a>I cannot change the textfile extension to .bat because I don't see the .txt extension

If you use Windows 10, in Explorer, go to the View tab and look for the "File name extensions" checkbox in the "Show/Hide" subcategory and tick it.

On older Windows versions, go into the folder and press the Alt button on your keyboard. This will bring up some menu options on top. Go to Tools->Folder Options. Navigate to the "View" tab and search for the "Hide extensions for known file types" checkbox. Untick it. Press the "Apply" button at the bottom and then "OK".


