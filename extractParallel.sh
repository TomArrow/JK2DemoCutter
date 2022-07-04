#!/bin/bash

#use this to run the highlightExtractionScript in parallel.
#Make sure to strip/replace ! exclamation marks (from playernames) from the .bat file and switch to Linux file endings.

function_name () {
	eval "./$1"
}
xargs  -d '\n' -I '{}' -P 5  bash -c "$(declare -f function_name); function_name '{}' ; "   <"highlightExtractionScript-linuxlines-noex-short.bat"
read -n1 -r