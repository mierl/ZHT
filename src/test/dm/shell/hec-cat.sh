
nfs="/mnt/common/tony/ZHT_Debug/src/test/dm"
local="/home/tony"

hec=$1
echo ""
echo "hec-$hec work node"
echo "showing bechmark log..."
cat $local/zbt.log
echo ""

echo ""
echo "showing zht server log..."
cat $local/zss.log
echo ""

echo ""
echo "showing zht manager file server log..."
cat $local/mss.log
echo ""

echo ""
echo "showing file server log..."
cat $local/fss.log
echo ""

echo ""
echo "showing mig log..."
cat $local/mig.log
echo ""
