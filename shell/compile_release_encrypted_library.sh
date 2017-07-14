#!/bin/bash
libhd_net=libhd_net

function pr_warning()
{
	echo -e "\033[46;31;1m [$1]\033[0m"
	exit 1
}

function pr_info()
{
	echo -e "\033[46;32;1m [$1]\033[0m"
}

function check_sdk_valid() 
{
	pr_info "checking sdk..."
	if [ -z ${CAMLINUX_BUILD_TOP} ]; then
		pr_warning "camdroid hasn't lunched"
		exit 0
	fi
	source $CAMLINUX_BUILD_TOP/build/envsetup.sh 
}

function remove_old_file() 
{
	cd $OUT
	find . -name "libol*" | xargs rm -rf
	find . -name "libCedarX*" | xargs rm -rf
	find . -name "libaw_net*" | xargs rm -rf
	find . -name "libcamrecorder*" | xargs rm -rf
	find . -name "libdatabase*" | xargs rm -rf
	find . -name "init" | xargs rm -rf
}

function complie_code() 
{
	pr_info "compiling code..."
	#make sure libol is compiled
	cd $CAMLINUX_BUILD_TOP/vendor
	mm -j8 -B

	#compile net code
	pr_info "compiling libhd_net... "
	cd $CAMLINUX_BUILD_TOP/frameworks/av/media
	if [ -d $HOME/workspace/$libhd_net ];then
		ln -s  $HOME/workspace/$libhd_net
		cd libhd_net
		mm -j8 -B
		cd ..
		rm -rf libhd_net
	else
		pr_warning "libhd_net $HOME/workspace/$libhd_net doesn't exist"
	fi

	pr_info "compile libhd_net finished"
	sleep 1

	#compile camrecorder
	pr_info "compiling libcamrecorder... "
	cd $CAMLINUX_BUILD_TOP/frameworks/av/media
	rm -rf libcamrecorder
	ln -s ~/workspace/library/media/modules/libcamrecorder/
	cd libcamrecorder
	#mm -j8 -B
	./release.sh 

	#copy release code
	cd $CAMLINUX_BUILD_TOP/frameworks/av/media
	rm -rf libcamrecorder
	libcamrecorder=`ls $HOME/tmp/libcamrecorder*.tar.bz2`
	if [ ! -z $libcamrecorder ];then
		tar -xf $libcamrecorder
		cd libcamrecorder
		mm -j8 -B
	else
		pr_warning "archived file of libcamrecorder doesn'ts exist in $HOME/tmp"
		exit 0
	fi
	pr_info "compile libcamrecorder finished"
	sleep 1
}

function check_ref_valid()
{
	#check if we have enough referece of check function
	cd $OUT/system
	grep -rHsn "atestb_check" .
	ref_num=`grep -rHsn "atestb_check" . | wc -l`
	pr_info "referece nums $ref_num"
	if [ $ref_num -lt 2 ]; then
		pr_warning "check ref num is invalid"
	fi
}

function copy_release_libraries()
{
	OUTPUT=$HOME/tmp/encrypted_libs
	rm -rf $OUTPUT
	mkdir $OUTPUT --parents
	FILES=`grep -rHsn "atestb_check" $OUT/system/lib`
	for file in $FILES
	do
		if [ -f $file ];then
			pr_info "coping file $file"	
			cp $file $OUTPUT
		fi
	done

	cp $OUT/system/bin/network_check $OUTPUT
}
remove_old_file
check_sdk_valid
complie_code
check_ref_valid
copy_release_libraries

