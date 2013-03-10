echo "usage: zht port(ss, se), seend(y/n), file server(f), adminserver(a)"
ss=$1
se=$2
s=$3
f=$4
a=$5

for((i=o;i<=($m-1);i++))
do

rm -f 5000$i.log

done



for((i=$ss;i<=$se;i++))
do

if [[ $s == "y" ]]; then
./zhtnode -p 5000$i -d node.cfg -n neighbor -z zht.cfg -s > 5000$i.log &
echo "zhtserver[5000$i] started..."
elif [[ $s == "n" ]]; then
./zhtnode -p 5000$i -d node.cfg -n neighbor -z zht.cfg > 5000$i.log &
echo "zhtserver[5000$i] started..."
else
echo "no seeds..."
fi

sleep 1

done


if [[ $f == "f" ]]; then
./fileserver &
echo "fileserver started..."
fi

if [[ $a == "a" ]]; then
./adminserver -d node.cfg -n neighbor -z zht.cfg &
echo "adminserver started..."
fi

