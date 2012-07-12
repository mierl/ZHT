process=$1
pid=`ps -A|grep $process |awk {'print $1'} `
echo "PID= $pid, memory usage as folows:"
top -p $pid


