CLKBASE=0x43c00004
ADDR=$CLKBASE
echo CLK REG > regdump.txt
#while [ $(($ADDR)) -lt $((0x43c00010)) ]; do
	#VALUE=`peek $ADDR`
	#VALUE=`printf "0x%08x" $VALUE`
	#poke $ADDR $VALUE
	#printf "0x%08x\t0x%08x\n" $ADDR $VALUE >> regdump.txt
	#ADDR=$(($ADDR + 4))
	#ADDR=`printf "0x%08x" $ADDR`
#done

#ADDR=0x43c00010
#while [ $(($ADDR)) -le $((0x43c0035c)) ]; do
	#VALUE=`peek $ADDR`
	#poke $(($ADDR)) $(($VALUE))
        #ADDR=$(($ADDR + 4))
#done
poke 0x43c00208 $1
poke 0x43c0025c 0x7
#peek 0x43c0025c
poke 0x43c0025c 0x3
#peek 0x43c0025c


CLKBASE=0x43c00000
ADDR=$CLKBASE
echo CLK REG > regdump.txt
while [ $(($ADDR)) -lt $((0x43c00010)) ]; do
	VALUE=`peek $ADDR`
	printf "0x%08x\t0x%08x\n" $ADDR $VALUE >> regdump.txt
	ADDR=$(($ADDR + 4))
done

ADDR=0x43c00200
while [ $(($ADDR)) -le $((0x43c0025c)) ]; do
	VALUE=`peek $ADDR`
        printf "0x%08x\t0x%08x\n" $ADDR $VALUE >> regdump.txt 
        ADDR=$(($ADDR + 4))
done

