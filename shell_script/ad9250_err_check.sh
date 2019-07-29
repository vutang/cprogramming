#!/bin/sh
# @Author: vutt6
# @Date:   2018-07-28 08:10:20
# @Last Modified by:   vutt6
# @Last Modified time: 2018-07-28 08:22:09

LOG_MON_FILE=/home/root/err_ad9250_reg_mon.log
LOG_INFO_FILE=/home/root/err_ad9250_info.log

# PA state: 0 ~ off, 1 ~ on 
PA_STATE=1

readonly TEMP_THRES_HIGH=70
readonly TEMP_THRES_LOW=60

echo "Log for monitoring register: $LOG_MON_FILE"
echo "Information log: $LOG_INFO_FILE" 

# Check if log file is created or not
# If not, files are created
if [ -f $LOG_MON_FILE ]; then
	echo 
else
	echo "Register monitor file" > $LOG_MON_FILE
fi
if [ -f $LOG_INFO_FILE ]; then
	echo 
else
	echo "Infor log" > $LOG_INFO_FILE
fi

while true; do
	# Get information
	PATEMP=`rrutool pa 1 temp | awk '{print $3}'`
	REG0=`peek 0x75000038`
	REG1=`peek 0x7500001C`
	REG2=`rrutool read 47 0xa | awk '{print $3}'`	
	REG3=`rrutool read 47 0xf3 | awk '{print $3}'`
	UPTIME1=`uptime | awk '{print $1}'`
	UPTIME2=`uptime | awk '{print $3}'`

	# Log
	printf "$UPTIME1, $UPTIME2, %f, 0x%x, 0x%x, 0x%x, 0x%x\n" \
		$PATEMP $REG0 $REG1 $REG2 $REG3 >> $LOG_MON_FILE

	# Convert from float to int
	PATEMP=${PATEMP%%.*}

	# If PA temp is high and PA is on -> turn off PA
	if [ $PATEMP -gt $TEMP_THRES_HIGH ] && [ $PA_STATE -eq 1 ]; then
		UPTIME=`uptime`
		echo "[$UPTIME] PA is in high temp. Turn all PA off" >> \
			$LOG_INFO_FILE

		rrutool pa 1 off
		rrutool pa 2 off
		rrutool pa 3 off
		rrutool pa 4 off
		# Change PA state to OFF (0)
		PA_STATE=0
	fi 

	# If PA temp is low and PA is off -> turn on PA
	if [ $PATEMP -lt $TEMP_THRES_LOW ] && [ $PA_STATE -eq 0 ]; then
		UPTIME=`uptime`
		echo "[$UPTIME]PA is in low temp. Turn all PA on" >> \
			$LOG_INFO_FILE

		rrutool pa 1 on
		rrutool pa 2 on
		rrutool pa 3 on
		rrutool pa 4 on
		# Change PA state to ON (1)
		PA_STATE=1
	fi
	sleep 1
done & 
