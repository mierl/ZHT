#!/bin/sh

#Prepare runing code
#echo "Start..."
cp server_general /dev/shm/.
cp benchmark_client /dev/shm/.
cp torusIP /dev/shm/.
cp zht.cfg /dev/shm/.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/tonglin/Installed/built/lib
#cat /proc/sys/net/core/somaxconn
ulimit -n 65536 #The maximum number of open file descriptors.
ulimit -s unlimited #The maximum stack size.
ulimit -l unlimited #The maximum size that may be locked into memory.
ulimit -c unlimited #The maximum size of core files created.
ulimit -m unlimited #The maximum resident set size. 
#echo "Original settings:"
#cat /proc/sys/net/core/somaxconn
#cat /proc/sys/net/core/netdev_max_backlog
#cat /proc/sys/net/ipv4/tcp_fin_timeout
#cat /proc/sys/net/ipv4/ip_local_port_range
echo 8000 > /proc/sys/kernel/threads-max
echo 8000 > /proc/sys/net/core/somaxconn #limit of listen queue, default is 128
echo 8000 > /proc/sys/net/core/netdev_max_backlog
echo 1 > /proc/sys/net/ipv4/tcp_fin_timeout
echo "10000   65000" > /proc/sys/net/ipv4/ip_local_port_range

echo 40960 > /proc/sys/net/ipv4/neigh/default/gc_thresh2
echo 40960 > /proc/sys/net/ipv4/neigh/default/gc_thresh3

echo 1 > /proc/sys/net/ipv4/tcp_timestamps 	#sysctl -w net.ipv4.tcp_timestamps=1
echo 1 > /proc/sys/net/ipv4/tcp_tw_recycle 	#sysctl -w net.ipv4.tcp_tw_recycle=1
echo 1 > /proc/sys/net/ipv4/tcp_tw_reuse	#sysctl -w net.ipv4.tcp_tw_reuse=1

#echo "24576   32768   49152" > /proc/sys/net/ipv4/tcp_mem
echo "16384     18000   20000" > /proc/sys/net/ipv4/tcp_mem # 16384 = 1GB
#echo "Memory page size = `getconf PAGE_SIZE`" page size = 65536.
#echo "/proc/sys/fs/file-max = `cat /proc/sys/fs/file-max`"
echo 201510 > /proc/sys/fs/file-max
#echo "/proc/sys/net/ipv4/tcp_mem = `cat /proc/sys/net/ipv4/tcp_mem`"
#echo "/proc/sys/net/ipv4/tcp_rmem = `cat /proc/sys/net/ipv4/tcp_rmem`"
#echo "/proc/sys/net/ipv4/tcp_wmem = `cat /proc/sys/net/ipv4/tcp_rmem`"

#echo "New settings:====================================="
#cat /proc/sys/net/core/somaxconn
#cat /proc/sys/net/core/netdev_max_backlog
#cat /proc/sys/net/ipv4/tcp_fin_timeout
#cat /proc/sys/net/ipv4/ip_local_port_range
#echo "initial mem usage: `free `"
numNode=$1
TCP=$2
#ip=`./ip.sh` # Within 1 partition (64 nodes)
ip=`./torusIP`
#port=`expr 50000 + $numNode`
port=50000
#echo $ip
#touch /intrepid-fs0/users/tonglin/persistent/$numNode/$ip
#echo "$ip"
export IP=$ip
export NNODE=$numNode
export PORT_FOR_REPLICA=50009
#echo "Time: Start: `date`"
echo "$ip $port" >> /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode
sleep 10 # 3 for small, 10 for large
#echo "check neighbor list"
numLine=`wc -l /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode | awk {'print $1'} `
# check if neighbor list is ready.
#echo "--------------1"
while [[ "$numLine" != "$numNode" ]]
do
	sleep 5 # 2 for small, 5 for large
#	echo "Neighbotlist: --------------should be $numNode, numLine = $numLine"
	numLine=`wc -l /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode | awk {'print $1'} `
done
#echo "--------------3"
cp /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode /dev/shm/.
#echo "Time: neighbor list ready: `date`"
/dev/shm/server_general $port /dev/shm/neighbor_$numNode /dev/shm/zht.cfg  $TCP tonglin  &
sleep 10
#echo "server started on $ip"
nLine=0
#Check if all servers are started


#while [[ "$nLine" != "$numNode" ]]
#do
#        sleep 5
#	echo "Register: --------------should be $numNode, nLine = $nLine"
#        nLine=`wc -l /intrepid-fs0/users/tonglin/persistent/Register_$numNode | awk {'print $1'} `
#done
#echo "Time: Server start complete: `date`"
sleep 15
#sleep $numNode
#echo "About to start client on $ip"
#/dev/shm/client 10000 /dev/shm/neighbor_$numNode /dev/shm/zht.cfg &
#/dev/shm/client 10000 /dev/shm/neighbor_$numNode /dev/shm/zht.cfg &
#/dev/shm/client 10000 /dev/shm/neighbor_$numNode /dev/shm/zht.cfg &
/dev/shm/benchmark_client 10000 /dev/shm/neighbor_$numNode /dev/shm/zht.cfg $TCP

#echo "client started on $ip"
