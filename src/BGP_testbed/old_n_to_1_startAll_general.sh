project=$1
numNode=$2
runTime=$3
userName=$4
protoc=$5

rm /intrepid-fs0/users/$userName/persistent/IOtest
rm /intrepid-fs0/users/$userName/persistent/Register_$numNode
rm /intrepid-fs0/users/$userName/persistent/neighbor_$numNode

touch /intrepid-fs0/users/$userName/persistent/Register_$numNode
touch /intrepid-fs0/users/$userName/persistent/neighbor_$numNode
touch /intrepid-fs0/users/$userName/persistent/IOtest
cqsub -p $project -k zepto-vn-eval -n $numNode -t $runTime ./old_n_to_1_node_start_general.sh $numNode $userName $protoc

