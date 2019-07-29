#!/bin/sh
cat input.txt | while read line
do
	echo $line
done

exit 0
while read -r line
do
	name="$line"
	echo "name $name"
done < `cat input.txt`
