#/bin/bash

PWD=$(pwd)

read -p "Do you really want to delete anything in \"$PWD\" (Y/N)? " -n 1 answer
answer=$(echo $answer | tr '[:lower:]' '[:upper:]')
echo ""
if [ $answer != "Y" ];then
	exit 0
fi

find . -maxdepth 1 -type d | while read dir
do
	if [ "$dir" != "." ]; then
		echo -e "\033[40;31m Entering $dir \033[0m"
		cd "$dir"
		rm * -rf 
		echo -e "\033[40;31m Leaving $dir \033[0m"
		cd -
	fi
done

