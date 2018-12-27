IN_IMG=bin/xboot.bin
SIGN_BIN=./tools/sign_ed25519
OUT_SIG=bin/xboot.sig

# Put your real key pair in keys/ :
PRIV_K=keys/key_priv_0.hex
PUB_K=keys/key_pub_0.hex

if [ -f "$PRIV_K" -a -f "$PUB_K" ];then
	echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^"
	echo "^^^ Sign with REAL keys ^^^"
	echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^"
else
	# A test key pair is in test-keys/ :
	PRIV_K=test-keys/key_priv_0.hex
	PUB_K=test-keys/key_pub_0.hex

	echo "**************************************"
	echo "*** Warning: Sign with TEST key !! ***"
	echo "*** Please put real keys in keys/. ***"
	echo "**************************************"
fi

echo "Private key: $PRIV_K"
echo "Public key : $PUB_K"

if [ ! -x $SIGN_BIN ];then
	echo "Can't execute $SIGN_BIN"
	exit 1
fi

if [ "$1" != "" ];then
	IN_IMG=$1
fi

if [ "$2" != "" ];then
	OUT_SIG=$2
fi

rm -f $OUT_SIG

#sign
echo "Sign: $IN_IMG"
$SIGN_BIN -p "$PRIV_K" -b "$PUB_K" -s $IN_IMG -o $OUT_SIG
if [ $? -ne 0 ];then
	echo "sign program failed"
	exit 1
fi

echo "Output signature: $OUT_SIG"
