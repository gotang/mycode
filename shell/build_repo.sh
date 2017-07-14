#!/bin/bash

SRC_DIR=$HOME/workspace/t3
DEST_DIR=$HOME/workspace/T3

dup_git()
{
	#copy repo
	rm ~/repo_tmp.tar.bz2 -rf

	if [ ! -d $SRC_DIR/$1/$2 ];then
		echo "source director $SRC_DIR/$1/$2 doesn't exist"
		exit 0
	fi
	if [ ! -d $DEST_DIR/$1 ];then
		echo "target director $DEST_DIR/$1 doesn't exist"
		exit 0
	fi

	echo "moving $SRC_DIR/$1/$2 to $DEST_DIR/$1/$2"

	#compress repository
	cd $SRC_DIR/$1
	tar -cjf ~/repo_tmp.tar.bz2 $2 
	cd -

	#extract repository
	rm -rf $DEST_DIR/$1/$2
	tar -xf ~/repo_tmp.tar.bz2  -C $DEST_DIR/$1
}

function dup_repository()
{
	if [ ! -f $1 ]; then
		echo "git list file $1 doesn't exist"
		return 0;
	fi
	#copy repo
	dup_git lichee .repo
	dup_git lichee/buildroot .git
	dup_git lichee/brandy .git
	dup_git lichee/linux-3.10 .git
	dup_git lichee/tools .git

	dup_git android .repo
	echo "finishing android"

	while read line
	do
		dup_git android/$line .git
	done < $1
}

function build_repository() 
{
	if [ ! -f $1 ]; then
		echo "git list file $1 doesn't exist"
		return 0;
	fi
	while read line
	do
		line=huidi/$line
		if [ ! -d $line.git ]; then
			git init --bare $line.git
			#chown -R git $line.git
			#chgrp -R git $line.git
		else
			echo "repository $line.git already exists"
		fi
	done < $1
}

function commit_repository()
{
	local gitignore_exist=0
	git init .
	if [ -f .gitignore ];then
		mv .gitignore $HOME/tmp
		gitignore_exist=1
	fi

	git add *
	git add ".*"
	if [ $gitignore_exist -eq 1 ]; then
		mv $HOME/tmp/.gitignore . dddd
		git add .gitignore
	fi

	repo=${line##*/}		
	git commit -m "init repository $repo.git" --allow-empty
	#git commit --amend
	echo $line | grep device
	if [ $? -eq 0 ]; then
		git remote add android git@192.168.1.99:/home/git/huidi/$line.git
		#git remote add android $HOME/repo/huidi/$line.git
	else
		git remote add android git@192.168.1.99:/home/git/huidi/platform/$line.git
		#git remote add android $HOME/repo/huidi/platform/$line.git
	fi
	git checkout -b t3-dev
	git push android t3-dev:t3-dev --force
}

function init_repository() 
{
	if [ ! -f $1 ]; then
		echo "git path file $1 doesn't exist"
		return 0;
	fi

	while read line
	do
		if [ -d $line ]; then
			cd $line
			echo "init repository $line.git"
			commit_repository
			cd -
		else
			echo "repository $line doesn't exist"
		fi
	done < $1
}


function check_repository() 
{
	if [ ! -f $1 ]; then
		echo "git path file $1 doesn't exist"
		return 0;
	fi
	
	while read line
	do
		if [ -d $line ]; then
			echo "checking repository $line"
			cd $line
			git status
			if [ -f .gitignore ];then
				echo "There's .gitignore in repository $line.git"
				#rm .git -rf
				#commit_repository
			fi
			cd -

		else
			echo "repository $line doesn't exist"
		fi
	done < $1
}

function add_files() 
{
	if [ ! -f $1 ]; then
		echo "git path file $1 doesn't exist"
		return 0;
	fi
	
	while read line
	do
		if [ -d $line ]; then
			echo "checking repository $line"
			cd $line
			git status >$HOME/files.log

		else
			echo "repository $line doesn't exist"
		fi
	done < $1
}

function clone_repository() 
{
	cd $HOME/workspace/cdr/$1 
	REPOSITORY=$(find . -name ".repo" -prune -o -name ".git")
	cd -

	mkdir $1
	cd $1
	for repository in $REPOSITORY
	do
		echo "repository is $repository"
		echo "$repository" | grep "\.git"
		if [ $? -eq 0 ]; then
			repository=${repository#/*}
			repository=${repository/\/.git/.git}
			echo "cloning $repository"
			git clone git@192.168.1.99:/home/git/huidi/$1/$repository
		fi
	done
	cd -

	REPOSITORY=$(find . -name ".repo" -prune -o -name ".git")
	for repository in $REPOSITORY
	do
		echo "repository is $repository"
		cd $repository
		TAGS=`git tag`
		for tag in $TAGS; 
		do 
			echo $tag | grep "sdv";  
			if [ $? -eq 0 ];then 
				git tag -d $tag;
				git push origin :$tag;
			fi;
		done
		cd -
	done
}

printf_usage() 
{
	echo "Usage:`basename $0` [-i path of git to init] [-b git list]"
}


if [ $# -lt 1 ]; then
	printf_usage
	exit 0
fi

while getopts i:b:c:d: OPTION
do
	case $OPTION in
		i)
			echo "init repository"
			init_repository $OPTARG
			;;
		b)
			echo "build repository"
			build_repository $OPTARG
			;;
		c)
			echo "build repository"
			check_repository $OPTARG
			;;
		d)
			echo "duplicate repository"
			dup_repository $OPTARG
			;;
		\?)
			printf_usage
			exit 0
			;;
	esac
done

