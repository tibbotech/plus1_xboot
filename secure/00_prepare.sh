echo "$0"

rm -rf out
mkdir -p out

XBOOT_BIN=../bin/xboot.bin

# let xboot.bin size be 8-byte aligned
sz=`du -sb $XBOOT_BIN | cut -f1` ;
printf "$XBOOT_BIN size = %d (hex %x)\n"  $sz $sz ;
if [ $((sz & 7)) != 0 ];then
	sz_new=$(((sz + 7) & ~7))
	printf "pad xboot bin size to %d (hex %x)\n" $sz_new $sz_new
	truncate -s $sz_new $XBOOT_BIN
	ls -l $XBOOT_BIN
fi

#TODO: add more sanity check here
