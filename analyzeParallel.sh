#/bin/bash

# Note: Remove the ./ before DemoHighlightFinder.exe if the .exe file isn't in the same folder and if it is in PATH instead

# This following line is for serial (non parallel) analysis of a folder
#find "demos" -name '*' -type f -exec ./DemoHighlightFinder.exe {} 10000 ctfreturns \;

# This following line is for parallel analysis of a folder with 5 threads (5 files being analyzed at same time)
find "demos" -name '*' -type f -print0 | xargs -0 -P 5 -I %  ./DemoHighlightFinder.exe % 10000 ctfreturns \; 2>>errors.log
read -n1 -r