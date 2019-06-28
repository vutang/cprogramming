# @Author: vutt6
# @Date:   2018-08-13 11:44:18
# @Last Modified by:   vutt6
# @Last Modified time: 2018-08-13 15:44:24
#!/bin/sh

# IP Configuration for Chain Topology
# BBC --------- RRU1 --------- RRU2 -------- RRU3
# x.168.14.1    x.168.14.2
# 				x.169.14.1     x.169.14.2
#				               x.170.14.1    x.170.14.2

configdir=/home/root/viettel/config
echo "Setting networking configuration"

# Each RRU has its own configuration
rru1_cfg() {
	route add -host 172.170.14.1 gw 172.169.14.2 # for RRU3
}

rru2_cfg() {
	route add -host 172.168.14.1 gw 172.169.14.1 # for BBC
}

rru3_cfg() {
	route add -host 172.169.14.1 gw 172.170.14.1 # for RRU1
	route add -host 172.168.14.1 gw 172.170.14.1 # for BBC
}

if [[ $1 -eq 1 ]]; then 
	echo "setting for 1"
	# route add -host 172.168.14.1 gw 172.167.14.1
	echo 1 > $configdir/chain_position
	rru1_cfg
elif [[ $1 -eq 2 ]]; then
	echo "setting for 2"
	echo 2 > $configdir/chain_position
	rru2_cfg
elif [[ $1 -eq 3 ]]; then
	echo "setting for 3"
	echo 3 > $configdir/chain_position
	rru1_cfg
fi
