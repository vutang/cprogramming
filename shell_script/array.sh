#!/bin/sh
rxPwr=(7 7 7)
for i in 0 1 2 3
do
	rxPwr[$i]=$i
	echo ${rxPwr[$i]}
done
echo ${rxPwr[0]}

