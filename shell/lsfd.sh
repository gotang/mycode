#!/bin/bash
while ((1 == 1))
do
	pid=`adb shell ps | grep mediaserver | awk '{print $2}'`
	echo "mediaserver pid $pid"
	adb shell ls -l /proc/$pid/fd
	sleep 1
done

