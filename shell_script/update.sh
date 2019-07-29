
tar -xzvf evt3a_4g9_patch_180615.tar.gz -C /tmp/
cp /tmp/librru.so.1.0.1 /lte_sw/running/rru/lib/librru.so.1.0.1
killall rrumon && cp /tmp/rrumon /lte_sw/running/rru/bin/rrumon && rrumon
