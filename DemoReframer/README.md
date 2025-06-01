
Standalone demo cutter for dm_15 and dm_16 files based on jamme/jomme source code.

Code is lifted and adapted from https://github.com/entdark/jaMME/ and https://github.com/entdark/jk2mp/

All credit for the basic demo cutting functionality goes to entdark. Lots of functions from the Jedi Knight source code releases included as well. 

# DemoReframer

This tool can help you change the perspective of a demo to that of another player.

## Usage

DemoReframer is a command line (CLI) tool. You can most comfortably run it using a .bat script. A .bat script is a normal text file with a .bat file ending. 

### Example #1: Reframing a demo to the player 'arborian'
Here is a simple working example of how to reframe to a player named 'arborian' (see from his perspective) of an imaginary demo called *dummydum.dm_15* and save that segment as *dummydum_reframed.dm_15*.

1. Put the demo and *DemoReframer.exe* in a fresh folder.
2. Create a new .txt text file and rename it to *randomScriptName.bat* (or any other filename with .bat ending). If you cannot see and change the file ending, [enable file extension display](#showFileExt).
3. Open this .bat file in a text editor and type ```DemoReframer.exe "dummydum.dm_15" "dummydum_reframed.dm_15" "arborian"``` in the first line.
4. Replace ``dummydum.dm_15`` with the actual name of your own demo.
5. Replace ``dummydum_reframed.dm_15`` with the actual name of (not yet existing) reframed demo you want.
6. Replace ``arborian`` with the name of the player you wish to see the perspective of.
7. Type ```pause``` in the second line. *(This is optional and allows you to see the console output instead of closing after it's done)*
8. Save and double click your .bat file.  This will run the reframing of your demo.
9. You should now have *dummydum_reframed.dm_15* (or a demo of your chosen name as per point 5) in your folder. 
10. You are done.

### Choosing the player for the reframe

There are a couple of ways to provide info about the player whose perspective you want to DemoReframer.

#### Playername

As in the example above, you can provide the name of the player in double quotation marks. You can leave away the quotation marks if there are only simple letters/numbers and no empty spaces in the name. This name does NOT need to include the full color code of the player's name. It can be a partial name. 

Limitation: The player must be present at the beginning of the demo. If he connects later, he will not be recognized

#### Client number 

Instead of a player name, you can simply write the client number of the client you wish to see the perspective of. 

E.g.

```DemoReframer.exe "dummydum.dm_15" "dummydum_reframed.dm_15" 3```

This way works even if the client connects later.

#### Demos that include map changes/new gamestates

If you have a demo that includes a map change, this process is likely to fail or produce weird results. Use DemoCutter to cut out a fitting segment first.

### Extra commandline options

You can append one of the following options to the command line in the .bat file to change the reframe behavior:

- ``--no-dimensions``: If you have a demobot demo containing information about dimensions (like democuts from the demobot), this option lets you ignore that data and simply show all dimensions in the reframe. In case the dimension separation bugs out.
- ``--vis-all``: If you are getting hall-of-mirrors effect (invisible parts of the level) in a reframe, try this option.

## FAQ
### <a name="showFileExt"></a>I cannot change the textfile extension to .bat because I don't see the .txt extension

If you use Windows 10, in Explorer, go to the View tab and look for the "File name extensions" checkbox in the "Show/Hide" subcategory and tick it.

On older Windows versions, go into the folder and press the Alt button on your keyboard. This will bring up some menu options on top. Go to Tools->Folder Options. Navigate to the "View" tab and search for the "Hide extensions for known file types" checkbox. Untick it. Press the "Apply" button at the bottom and then "OK".
