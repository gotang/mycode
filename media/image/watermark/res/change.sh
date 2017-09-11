#!/bin/bash
FILES=`find . -name "*.bmp"`
for file in $FILES
do
	echo "file is $file"
	suffix=${file##*_}
	prefix=${file%%_*}
	echo "dest file is ${prefix}_$suffix"
	mv $file ${prefix}_${suffix}
#	ffmpeg -i $file -s 12x16 wm_320p_$suffix
done
