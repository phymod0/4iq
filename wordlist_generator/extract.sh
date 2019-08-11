#!/bin/bash

if [ $1 ]; then
	find "$1/data/" -type f \
		| xargs cat
else
	echo "Specify the breach input directory"
	exit -1
fi
