pn="zhtnode"
pid=`ps -A|grep $pn |awk {'print $1'}`
kill -9 $pid
echo "stopping zhtnode"

pn="fileserver"
pid=`ps -A|grep $pn |awk {'print $1'}`
kill -9 $pid
echo "stopping fileserver"

pn="adminserver"
pid=`ps -A|grep $pn |awk {'print $1'}`
kill -9 $pid
echo "stopping adminserver"

echo "deleting data"
cd /home/tony
rm -rf `find . -type d -name data`
rm -rf `find . -type f -name mss.log`
rm -rf `find . -type f -name zss.log`
rm -rf `find . -type f -name fss.log`
rm -rf `find . -type f -name zbt.log`
rm -rf `find . -type f -name mig.log`

cd /mnt/common/tony/ZHT_Debug/src/test/dm
rm -rf `find . -type f -name hec.log`
