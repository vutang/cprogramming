fpga_infofile="fpga_info.txt"
fw_releasedir="release.txt"

logger() {
	echo "[FPGABIT] $1"
}

load_default_bit() {
	logger "Default bit is loaded"
}

test -f $fpga_infofile && cat $fpga_infofile | grep "RRU Hardware" >/dev/null && cat $fpga_infofile | grep "Topo" >/dev/null && cat $fpga_infofile | grep "Mode" >/dev/null && cat $fpga_infofile | grep "BW1" >/dev/null && cat $fpga_infofile | grep "BW2" >/dev/null
if [ $? -ne 0 ]; then
	logger "$fpga_infofile is not exist or in wrong format"
	logger "Using default bitstream"
	load_default_bit
	rm -rf /tmp/bitstream
	exit 0
fi

hw_version=`cat $fpga_infofile | grep "RRU Hardware" | awk {'print $4'} | sed 's/["",]//g'`
topo=`cat $fpga_infofile | grep "Topo" | awk {'print $3'} | sed 's/["",]//g'`
mode=`cat $fpga_infofile | grep "Mode" | awk {'print $3'} | sed 's/["",]//g'`
bw1=`cat $fpga_infofile | grep "BW1" | awk {'print $3'} | sed 's/["",]//g'`
bw2=`cat $fpga_infofile | grep "BW2" | awk {'print $3'} | sed 's/["",]//g'`

logger "fpga_info.txt: $hw_version, $topo, $mode, $bw1, $bw2"

# Check release file
test -f $fw_releasedir && cat $fw_releasedir | grep "RRU Hardware" >/dev/null
if [ $? -ne 0 ]; then
	logger "$fpga_infofile is not exist or in wrong format"
	logger "Using default bitstream"
	rm -rf /tmp/bitstream
	exit 0
fi

release_hw_version=`cat $fw_releasedir | grep "RRU Hardware" | awk {'print $4'} | sed 's/["",]//g'`
logger "fw-release: $release_hw_version"

if [ $release_hw_version != $hw_version ]; then
	logger "$fpga_infofile is not exist or in wrong format"
	logger "Using default bitstream"
	rm -rf /tmp/bitstream
	exit 0
fi

logger "Load bitstream"

