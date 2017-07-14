#!/bin/bash

push()
{
	suffix=${1##*.}
	mode=""
	case $suffix in
		so)
			mode=644
			;;
		apk)
			mode=644
			;;
		jar)
			mode=644
			;;
		*)
			mode=755
			;;
	esac
	if [ ! -n $TARGET ];then
		target=$TARGET
	fi

	if [ ! -f $OUT_DIR/$1 ]; then
		echo "file $OUT_DIR/$1 doesn't exist"
		return
	fi
	adb push $OUT_DIR/$1 /$1
	adb shell chmod $mode /$1 
	echo "pushing $OUT_DIR/$1 has finished"
}

PROJECT=t3
REBOOT=false
while getopts rp: OPTION
do
	case $OPTION in
		r)
			REBOOT=true
			;;
		p)
			PROJECT=$OPTARG
			;;
		\?)

			;;
	esac
done

SOLUTION=
PRODUCT=
SDK=
TARGET=

case $PROJECT in
	t3)
		SOLUTION=t3
		PRODUCT=t3-p1
		SDK=android
		;;
	owl)
		SOLUTION=cdr
		PRODUCT=crane-owl
		SDK=camdroid
		TARGET=/mnt/extsd
		;;
	\?)
		;;
esac

OUT_DIR=$HOME/workspace/server/workspace/$SOLUTION/$SDK/out/target/product/$PRODUCT
if [ ! -d $OUT_DIR ]; then
	echo "invalid route:$OUT_DIR"
	exit 0
fi

echo "output dir:$OUT_DIR"
adb remount

if [ "$PRORJECT" == "owl" ]; then
	ITEMS=(
	newcdr
	obd_test
	screen_test
	camera_check
	)
else
	ITEMS=(
	system/bin/surfaceflinger
	system/lib/libgui.so
	system/lib/libui.so
	system/lib/libsurfaceflinger.so
	system/framework/services.jar
	system/lib/libbluetooth_jni.so
	system/app/Bluetooth.apk
	system/lib/hw/bluetooth.default.so
	system/priv-app/SystemUI.apk
	system/lib/hw/camera.t3.so
	)
fi

for item in ${ITEMS[*]}
do
	push $item
done


if [ "true" = "$REBOOT" ];then
	adb shell sync
	adb reboot
else
	adb logcat -c
	#adb logcat -v time
fi
