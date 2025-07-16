#/bin/bash
# This is the function that calls the analyzer. 
analyzer () {
	mkdir -p "_demoopt_meta"
	# You can change the analysis parameters here
	# Note: Remove the ./ before DemoHighlightFinder.exe if the .exe file isn't in the same folder and if it is in PATH instead
	filename=$(basename -- "$1")
	#echo "$filename"
	extension="${filename##*.}"
	#echo "$extension"
	#noext="${filename%.*}"
	failure="_FAILURE"
	success="_SUCCESS"
	failurename="_demoopt_meta/$filename$failure"
	successname="_demoopt_meta/$filename$success"
	#successname="_demoopt_meta/$filename"
	#echo "$failurename"
	
	if test -f "$failurename"; then
	  echo "File already failed before."
	  return
	fi
	if test -f "$successname"; then
	  echo "File already succeeded before."
	  return
	fi

	
	random=$(openssl rand -hex 28)
	#echo "random: $random"
	outputname="$random.$extension"
	
	DemoOptimizer "$1" "$random"
	
	if [ $? -eq 0 ]; then
		echo "Demo $1 optimizing successful"
		mv "$filename" "_demoopt_meta"
		mv "$outputname" "$filename"
		touch "$successname"
	else
		>&2 echo "Demo $1 optimizing failed for unknown reason"
		rm "$outputname"
		touch "$failurename"
	fi
	
	#while [ -f pause ]; do
	#  echo "'pause' file found. waiting 10 seconds to retry."
	#  sleep 10s
	#done
	#DemoHighlightFinder.exe "$1" 30000 ctfreturns -C "script" --skip-kills
	#if [ $? -eq 139 ]; then
	#	>&2 echo "Demo $1 led to segfault!"
	#fi
}

# Replace the path with the path you want to analyze. If you need more than 1 demo path, duplicate the line that says `"/path/to/demo/folder" \` as many times as necessary and replace the paths with the new paths
demoPaths=( \
"." \
)

for demoPath in "${demoPaths[@]}"; do

	# This following line is for parallel analysis of a folder with 5 threads (5 files being analyzed at same time). Comment it out if you don't want parallel processing.
	find "$demoPath" -maxdepth 1 -name '*.dm?[0-9][0-9]' -type f -print0 | xargs -0 -P 1 -I %  bash -c "$(declare -f analyzer); analyzer \"%\"" \; 2>>errors.log

done

read -n1 -r