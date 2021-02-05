#!/bin/bash

echo "$0"

source sb_info.sh_inc

# This script:
# 1. generate signature by signing { xboot, sb_info }
# 2. put the signature into sb_info


SB_INFO=out/sb_info.bin
BODY_BIN=../bin/xboot.bin
BODY_EN=out/body_encrypted.bin
XBOOT_SB_BIN=out/xboot_sb.bin

OUT_SIG=out/xboot_sb.sig


if [ ! -f $SB_INFO ];then
	echo "$0 : Not found sb_info : $SB_INFO"
fi

# Is "signature" enabled in sb_info of this build ?
val=0x`xxd -l 1 -s 4 $SB_INFO |cut -d' ' -f2`
need_sign=$((val & 1))
need_encrypt=$((val & 2))

echo "sb_flags=$val (need_sign=$need_sign, need_encrypt=$need_encrypt) "

if [ "$need_sign" = "0" ];then
	echo "skip sign : $SB_INFO doesn't enable signature"
	exit 0
fi

# public key is only for self-verification

echo "Private key: $SB_KPRIV"
echo "Public key : $SB_KPUB"

if [ ! -f $SB_KPRIV ];then
	echo "$0 : Not found Sb_Kpriv : $SB_KPRIV"
	exit 1
fi

if [ ! -f $SB_KPUB ];then
	echo "$0 : Not found Sb_Kpub : $SB_KPUB"
	exit 1
fi

if [ ! -x $ED_SIGN_TOOL ];then
	echo "Can't execute $ED_SIGN_TOOL"
	exit 1
fi

rm -f $OUT_SIG

# clear field before sign
# 64-byte signature @ offset 16
dd if=/dev/zero of=$SB_INFO conv=notrunc bs=1 count=64 seek=$SB_SIG_OFF 2>/dev/null

# if not encrypted : use xboot.bin
if [ $need_encrypt -eq 0 ];then
	echo "Sign plaintext xboot and sb_info"
	body_file=$BODY_BIN
else
	echo "Sign encrypted xboot and sb_info"
	body_file=$BODY_EN
fi

[ ! -f $body_file ] && echo "missed xboot file : $body_file" && exit 1

body_size=`stat -c%s $body_file`
[ "$body_size" = "0" ] && echo "empty xboot file : $body_file" && exit 1

#echo "xboot size=$body_size"

cat $body_file $SB_INFO >$XBOOT_SB_BIN

# Sign it -> gen { hash.bin , OUT_SIG }
$ED_SIGN_TOOL -p "$SB_KPRIV" -b "$SB_KPUB" -s $XBOOT_SB_BIN -o $OUT_SIG
if [ $? -ne 0 ];then
	echo "sign program failed"
	exit 1
fi

rm -f hash.bin

if [ ! -f $OUT_SIG ];then
	echo "$0 : error, signaure ($OUT_SIG) isn't generated!"
fi

echo "Output signature: $OUT_SIG"


# Replace the 64-byte signaure @ offset (xboot size)+16
dd if=$OUT_SIG of=$XBOOT_SB_BIN conv=notrunc bs=1 count=64 seek=$((body_size + SB_SIG_OFF)) 2>/dev/null

hexdump -C -s $body_size $XBOOT_SB_BIN

echo "Output xboot + sb_info : $XBOOT_SB_BIN"

