#!/bin/bash

FILE=`ls /media/sheldon/UDISK/video`
for file in $FILE
do
	basename=${file%.*}
	cp /media/sheldon/UDISK/video/$file /media/sheldon/UDISK/sos/${basename}_SOS.mp4
done
