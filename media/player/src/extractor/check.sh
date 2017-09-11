#!/bin/bash

VIDEO_PATH=/media/sheldon/UDISK/video/
FILES=`ls $VIDEO_PATH`
for file in $FILES
do
	echo "check file $file"
	./out/test_extractor $VIDEO_PATH/$file  >>log.log

done

