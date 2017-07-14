#!/bin/bash
cd $OUT
FILES=`ls $HOME/tmp/libs`

for file in $FILES
do
	echo "file is $file"
	cd $OUT
	rm system/lib/$file -rf
	find . -name "*.img" | xargs rm -rf
	cp $HOME/tmp/libs/$file system/lib/$file
done
