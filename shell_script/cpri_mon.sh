#!/bin/sh
CPRI_STS=0x0
LED_FLAG=0x0
LEG_VALUE=0x1
while true; do
	CPRI_STS=`rrutool read 36 0` | awk '{print $3}'`
	if [ $CPRI_STS == "0xf" ]; then
		LED_FLAG=1
	else
		LED_FLAG=0
	fi

	LEG_VALUE=$((!$LEG_VALUE && $LED_FLAG))

	sleep 1
done &