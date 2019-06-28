# @Author: vutt6
# @Date:   2018-08-24 16:44:22
# @Last Modified by:   vutt6
# @Last Modified time: 2018-08-25 09:00:07

cd /home/root/

TXCPL_THRES=(1.9 1.9 1.9 1.9)
RXPWR_THRES=(-45 -45 -45 -45)

rxPwr=(0 0 0 0)
txCpl=(0 0 0 0)
gainAnalog=(0 0 0 0)
gainDigital=(0 0 0 0)

rrutool xadcinfo > xadcinfo.txt
logdir=/home/root/testReboot.log
countdir=/home/root/count
WAITTIME=5 #in minutes

is_smaller() {
	ret=`awk -v n1="$1" -v n2="$2" 'BEGIN { print (n1 < n2) ? "1" : "0" }'`
	return $ret
}

# Init Counter 
# $count1 $count2 $count3
# $count1: boot time
# $count2: boot time but fail
# $count3: boot time fail but self-fix
test -f "$countdir"
if [ $? -ne 0 ]; then
	echo "1 0 0" > $countdir
else
	count1=`cat /home/root/count | awk {'print $1'}`
	count2=`cat /home/root/count | awk {'print $2'}`
	count3=`cat /home/root/count | awk {'print $3'}`
	count1=$(($count1+1))
	echo "$count1 $count2 $count3" > /home/root/count
fi

get_board_info() {
	dateTime=`date +%Y-%m-%d\ %H:%M:%S` 
	printf "$dateTime, " >> $logdir
	# Read FWD
	for i in 0 1 2 3
	do
		rxPwr[$i]=`dpdctl check $i | grep "rx" | awk {'print $4'}`
		printf "%0.2f, " ${rxPwr[$i]} >> $logdir
	done

	# Read TXCPL
	for i in 0 1 2 3
	do
		txCpl[$i]=`cat xadcinfo.txt | grep "TX$1_CPL_OUT" | awk {'print $4'}`
		printf "%0.2f, " ${txCpl[$i]} >> $logdir
	done

	for i in 0 1 2 3
	do
		gainAnalog[$i]=`rrutool read $((26+$i)) 0 | grep "Read data" | awk {'print $3'}`
		printf "${gainAnalog[$i]}, " >> $logdir
	done	

	gainDigital[0]=`peek 0x46000000`
	printf "${gainDigital[0]}, " >> $logdir
	gainDigital[1]=`peek 0x46000004`
	printf "${gainDigital[1]}, " >> $logdir
	gainDigital[2]=`peek 0x46000008`
	printf "${gainDigital[2]}, " >> $logdir
	gainDigital[3]=`peek 0x4600000c`
	printf "${gainDigital[3]}, " >> $logdir
	printf "\n" >> $logdir
}

echo "-----------------------------------------!" >> $logdir
get_board_info

error=0
for i in 0 1 2 3
do
	# is_smaller ${txCpl[$i]} ${TXCPL_THRES[$i]}
	# ret1=$?

	is_smaller ${rxPwr[$i]} ${RXPWR_THRES[$i]}
	ret2=$?

	if [ $ret2 -eq 1 ]; then
		dateTime=`date +%Y-%m-%d\ %H:%M:%S` 
		echo "$dateTime, Error occurs in $i: ${rxPwr[$i]}!" >> $logdir
		error=1
	fi
done

# Update count1 if error
if [ $error -eq 1 ]; then
	count1=`cat /home/root/count| awk {'print $1'}`
	count2=`cat /home/root/count| awk {'print $2'}`
	count3=`cat /home/root/count| awk {'print $3'}`
	count2=$(($count2+1))
	echo "$count1 $count2 $count3" > /home/root/count
else
	echo "Success. Reboot...."
fi

# Wait in $WAITTIME minutes and re-check
timeWait=$((WAITTIME * 60))
sleep $timeWait

dateTime=`date +%Y-%m-%d\ %H:%M:%S` 
echo "$dateTime, re-check!" >> $logdir
get_board_info

error=0
for i in 0 1 2 3
do
	# is_smaller ${txCpl[$i]} ${TXCPL_THRES[$i]}
	# ret1=$?

	is_smaller ${rxPwr[$i]} ${RXPWR_THRES[$i]}
	ret2=$?

	if [ $ret2 -eq 1 ]; then
		dateTime=`date +%Y-%m-%d\ %H:%M:%S` 
		echo "$dateTime, Error occurs in $i: ${rxPwr[$i]}!" >> $logdir
		error=1
	fi
done

# Update count2 if error
if [ $error -eq 0 ]; then
	dateTime=`date +%Y-%m-%d\ %H:%M:%S` 
	echo "$dateTime, re-check, no error exists!" >> $logdir
	count1=`cat /home/root/count| awk {'print $1'}`
	count2=`cat /home/root/count| awk {'print $2'}`
	count3=`cat /home/root/count| awk {'print $3'}`
	count3=$(($count3+1))
	echo "$count1 $count2 $count3" > /home/root/count
fi

# reboot
