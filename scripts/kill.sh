process=$1
ps -A|grep $process |awk {'print $1'} > tempDistFile

while read line
do
        kill -9 $line
        echo "Process $line is killed.";
done < "tempDistFile"
rm tempDistFile
