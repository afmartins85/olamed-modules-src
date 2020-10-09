#!/bin/sh

if [ $# -eq 0 ] 
then
	echo "No arguments supplied. Try: './make.sh [target_name_version]'"
else
	dh_make --single --createorig -p $1
fi

