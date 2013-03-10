
echo "usage: o(local to datasys), r(datasys to hec)"

if [[ $p == "o" ]]; then
scp -r ./ZHT_Debug/src zhouxb@datasys.cs.iit.edu:/home/zhouxb/ZHT_Debug
fi


if [[ $p == "r" ]]; then
scp -r /home/zhouxb/ZHT_Debug xzhou@hec.cs.iit.edu:/mnt/common/xzhou/
fi




scp -r /home/xiaobing/IIT-study/research/workspace/iit.datasys/iit.datasys.zht/ZHT_Debug/src zhouxb@datasys.cs.iit.edu:/home/zhouxb/ZHT_Debug/


scp -r /home/zhouxb/ZHT_Debug/src tony@hec.cs.iit.edu:/mnt/common/tony/ZHT_Debug/


./zhtclienttest 1 neighbor zht.cfg TCP


./man.sh 6 9

./kill.sh 16 45

./wrapper.sh 16 45

./cat.sh 16 45



5
15
54
64

16-48

#gen neighbor and node
./hec-cf.sh 6 7

#kill and remove, 
./kill.sh

#echo "start service(ns, ne), seed(y/n)"
./ss.sh

#cat, show log(ns, ne)
./cat.sh


#echo run henchmark(ns, ne)
./rr.sh


#cat, show log(ns, ne)
./cat.sh

#cat, show log(ns, ne, op)
./bc.sh
 

for i in `seq 6 7`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-ss.sh hec-$i y f a"; done

for i in `seq 6 7`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-run.sh hec-$i 10000"; done

for i in `seq 6 7`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-kill.sh"; done

for i in `seq 6 7`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-cat.sh $hec-$i"; done > hec.log



for i in `seq 6 9`; do ssh hec-$i "/mnt/common/tony/ZHT_Debug/src/test/dm/hec-run.sh hec-$i 10000"; done


/mnt/common/tony/ZHT_Debug/src/test/dm

./zhtclienttest 10 neighbor2 zht2.cfg TCP


ps -A|grep zhtnode
ps -A|grep adminserver
ps -A|grep fileserver



ssh hec-1 cat /home/tony/fss.log
ssh hec-1 cat /home/tony/mss.log
ssh hec-1 cat /home/tony/zbt.log
ssh hec-1 cat /home/tony/zss.log

