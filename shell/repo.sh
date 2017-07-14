#!/bin/bash

GITS=(
)


function build_repository() 
{
	echo "start directory $1"

	for repository in GITS
	do
		rm $repository -rf
		echo "repository is $repository"
		git init --bare $repository
		chown -R git $repository
		chgrp -R git $repository
	done
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

#build_repository $HOME/workspace/cdr/camdroid
#build_repository $HOME/workspace/cdr/lichee
clone_repository camdroid
clone_repository lichee
