#!/bin/bash

filename="$1"
while read -r line
do
	HEADER=${line}.h
	SOURCE=${line}.cpp
	NAME=`echo ${line} | tr '[a-z]' '[A-Z]'`

	touch ${HEADER}
	touch ${SOURCE}

echo " 
/* 
 * ${HEADER}
 * 
 *  Created on: May 9, 2017
 *      Author: sheldon
 */

#ifndef LEETCODE_${NAME}_H_
#define LEETCODE_${NAME}_H_

class ${line} {
public:

private:

};


#endif /* LEETCODE_${NAME}_H_ */
" > ${HEADER}

echo "
/*
 * ${SOURCE}
 *
 *  Created on: Jul 19, 2017
 *      Author: sheldon
 */
" > ${SOURCE}

done < "$filename"


