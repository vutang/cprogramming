#!/bin/sh
RRU_IP=172.168.14.2
GENPASSDIR=/tmp/genpass.sh
RRU_COMPRESS_FILE=/tmp/compressLog


if [[ $1 =~ ^-?[0-9]+$ ]]; then
	HOUR=$1
	echo "Log is copied every $HOUR hours"
else
	HOUR=8
	echo "$1 is not valid argument number" 
	echo "Use default: $HOUR (hours)"
fi
TIMEOUT=$(($HOUR * 3600))
# for test
TIMEOUT=10

# Creat genpass file
echo "#!/bin/sh" > $GENPASSDIR
echo "echo root" >> $GENPASSDIR
chmod +x $GENPASSDIR

# Create compress log file for rru
echo "#!/bin/sh" > $RRU_COMPRESS_FILE
echo "DATE=\`date +%Y%m%d_%H%M%S\`" >> $RRU_COMPRESS_FILE
echo "LOGFILE=\"rrulog_\"$DATE" >> $RRU_COMPRESS_FILE
echo "tar -czf /tmp/\"$LOGFILE\".tar.gz /lte_sw/running/rru/log" >> $RRU_COMPRESS_FILE

# Prepare for SSH 
export SSH_ASKPASS="$GENPASSDIR" && export DISPLAY=

SSH='ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null'
SCP='scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null'
SSH_PASS='timeout 20 setsid taskset 0x80 ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null'
SCP_PASS='setsid taskset 0x80 scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null'

# Compress and copy log procedure
process_copy_log() {
    $SSH_PASS root@${RRU_IP} "sh /home/root/compressLog" 2>/dev/null
    sleep 5
    $SCP_PASS root@${RRU_IP}:/tmp/rrulog_* . 2>/dev/null
    sleep 5
    $SSH_PASS root@${RRU_IP} "rm /tmp/rrulog_*" 2>/dev/null
}

# Init RRU
$SCP_PASS $RRU_COMPRESS_FILE root@$RRU_IP:~/ 2>/dev/null

# Timer
COUNT=0
while [ $COUNT -le $TIMEOUT ]; do
    if [ $COUNT -ge $TIMEOUT ]; then
        process_copy_log   
        COUNT=0         
    else           
        COUNT=$(($COUNT + 1))
    fi                           
    sleep 1
done

# RRU
# DATE=`date +%Y%m%d_%H%M%S`
# LOGFILE="rrulog_"$DATE
# tar -czf /tmp/"$LOGFILE".tar.gz /lte_sw/running/rru/log
