#!/bin/bash
ANDROID_CODE_TOP=/home/sheldon/workspace/openSource/android/frameworks/av

TARGET_DIR=$HOME/avformat
if [ ! -d $TARGET_DIR ]; then
    mkdir $TARGET_DIR -p 
fi

INCLUDE_DIRECTORY=$(find include -type d)
SOURCE_DIRECTORY=$(find src -type d)

function build_directory() {
for dir in $INCLUDE_DIRECTORY
do
    mkdir $TARGET_DIR/$dir -p
done

for dir in $SOURCE_DIRECTORY
do
    mkdir $TARGET_DIR/$dir -p
done
}


INCLUDES=$(find . -type f -name "*.h")
SOURCES=$(find . -type f -name "*.cpp")

function copy_file() {
for include in $1
do
    file=${include##*/}
    echo $file
    cd $ANDROID_CODE_TOP >/dev/null
    android_file=$(find . -type f -name $file)
    if [ ! -z $android_file ]; then
        cp $android_file $TARGET_DIR/$include
    fi
    cd -
done
}

build_directory
copy_file "$INCLUDES"
copy_file "$SOURCES"


