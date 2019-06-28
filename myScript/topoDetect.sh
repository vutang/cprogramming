#!/bin/sh

# For log file
LOGFILE=/home/root/topoDetect.log
#LOGFILE=stdout
echo "Log is saved in $LOGFILE" 
rm $LOGFILE

detect_master() {
	MASTER_SUBNET=166
	MASTER_IP=172.$MASTER_SUBNET.14.1
	LOCAL_IP=172.$MASTER_SUBNET.14.2
	PINGTIMEOUT=3	

	COUNTER=0
	DATE=`date +%Y%m%d_%H%M%S`
	echo $DATE > $LOGFILE

	POSITION=0

	# Detect what master's IP is
	while [ $COUNTER -lt 3 ]; do
		COUNTER=$(($COUNTER + 1))
		ifconfig eth1 $LOCAL_IP

		echo "Start probing $MASTER_IP" >> $LOGFILE
		ping -I eth1 $MASTER_IP -w $PINGTIMEOUT > /dev/null

		if [ $? -eq 0 ]; then
			echo "---> Detect master $MASTER_IP" >> $LOGFILE
			POSITION=$((MASTER_SUBNET-165))
			# break
		fi
	#	else
			if [ $MASTER_SUBNET -eq 168 ]; then
				MASTER_SUBNET=166
			else
				MASTER_SUBNET=$(($MASTER_SUBNET + 1))
			fi

			MASTER_IP=172.$MASTER_SUBNET.14.1
			LOCAL_IP=172.$MASTER_SUBNET.14.2 
	#	fi
	done
	return $POSITION
}

detect_master
POSITION=$?

if [ $POSITION -eq 0 ]; then
	echo "Detect master fail! Restore default configuration" >> $LOGFILE
	ifconfig eth1 172.166.14.2
	exit 1
else
	echo "Detect POSITION $POSITION" >> $LOGFILE
	ETH1=$(($POSITION + 165))
	ETH2=$(($ETH1 + 1))
	ifconfig eth1 172.$ETH1.14.2
	ifconfig eth2 172.$ETH2.14.1

	echo ETH1 172.$ETH1.14.2 >> $LOGFILE
	echo ETH2 172.$ETH2.14.1 >> $LOGFILE
fi

# Found the next RRU
ping 172.$ETH2.14.2 -w 3 > /dev/null
if [ $? -eq 0 ]; then
	echo "Found the next hop: 172.$ETH2.14.2" >> $LOGFILE	
fi

CPRI0_STATUS=`rrutool read 36 0 | awk '{print $3}'`
CPRI0_STATUS_PRE=$CPRI0_STATUS

while true; do
	CPRI0_STATUS_PRE=$CPRI0_STATUS
	CPRI0_STATUS=`rrutool read 36 0 | awk '{print $3}'`
	if [ "$CPRI0_STATUS" != "$CPRI0_STATUS_PRE" ] && [ "$CPRI0_STATUS" == "0xf" ]; then
		echo "CPRI0_STATUS changed" >> $LOGFILE
		sleep 1
		detect_master
		POSITION=$?
		if [ $POSITION -eq 0 ]; then
			echo "Detect master fail! Restore default configuration" >> $LOGFILE
			ifconfig eth1 172.166.14.2
			exit 1
		else
			echo "Detect POSITION $POSITION" >> $LOGFILE
			ETH1=$(($POSITION + 165))
			ETH2=$(($ETH1 + 1))
			ifconfig eth1 172.$ETH1.14.2
			ifconfig eth2 172.$ETH2.14.1

			echo ETH1 172.$ETH1.14.2 >> $LOGFILE
			echo ETH2 172.$ETH2.14.1 >> $LOGFILE
		fi
	fi
	sleep 1
done
