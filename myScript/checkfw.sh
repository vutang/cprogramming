#!/bin/sh

[ "$1" = "-h" ] && \
echo "Usage:" && \
echo "	-h for help" && \
echo "	\"all\" for checking all channel" && \
echo "	\"channum\" for specific channel" && \
exit 0

if [ "$1" = "all" ]; then
	for i in 0 1 2 3
	do
		dpd_chan=$i
		pa_chan=$((dpd_chan+1))				
		dpdctl check $dpd_chan		
		rrutool pa $pa_chan temp		
		sleep 1
	done
	exit 0
fi

[ "$1" -lt "0" ] || [ "$1" -gt "3" ] && \
echo "error: channel have to in [0:3]" && exit 1

dpd_chan=$1
pa_chan=$((dpd_chan+1))
#Check Power
dpdctl check $1
#Check PA Temperature
rrutool pa $pa_chan temp

