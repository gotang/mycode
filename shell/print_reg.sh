#!/bin/bash
i=1
while ((i<=100))
do
	var=$(printf "0,1,0x%x" $i)
	adb shell "echo $var >/sys/devices/platform/sunxi-pcm-codec/audio_reg_debug/audio_reg"
	echo "$ses"
	let ++i
done

let i=1
while ((i<=100))
do
	var=$(printf "0,2,0x%x" $i)
	adb shell "echo $var >/sys/devices/platform/sunxi-pcm-codec/audio_reg_debug/audio_reg"
	echo "$ses"
	let ++i
done
