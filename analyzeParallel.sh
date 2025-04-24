#/bin/bash
# This is the function that calls the analyzer. 
analyzer () {
	# You can change the analysis parameters here
	# Note: Remove the ./ before DemoHighlightFinder.exe if the .exe file isn't in the same folder and if it is in PATH instead
	while [ -f pause ]; do # You can create a file named "pause" in the same directory to pause analysis.
	  echo "'pause' file found. waiting 10 seconds to retry."
	  sleep 10s
	done
	./DemoHighlightFinder.exe "$1" 10000 ctfreturns
	if [ $? -eq 139 ]; then
		>&2 echo "Demo $1 led to segfault!"
	fi
}

# Replace the path with the path you want to analyze. If you need more than 1 demo path, duplicate the line that says `"/path/to/demo/folder" \` as many times as necessary and replace the paths with the new paths
demoPaths=( \
"/path/to/demo/folder" \
)

for demoPath in "${demoPaths[@]}"; do

	# This following line is for serial (non parallel) analysis of a folder. Leave it commented out unless you don't want parallel processing.
	#find "$demoPath" -name '*' -type f -exec bash -c "$(declare -f analyzer); analyzer \"{}\"" \; 2>>errorsXargs.log

	# This following line is for parallel analysis of a folder with 5 threads (5 files being analyzed at same time). Comment it out if you don't want parallel processing.
	find "$demoPath" -name '*' -type f -print0 | xargs -0 -P 5 -I %  bash -c "$(declare -f analyzer); analyzer \"%\"" \; 2>>errors.log

done

read -n1 -r