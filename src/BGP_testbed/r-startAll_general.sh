numNode=$1
runTime=$2
protocol=$3
#cqsub -p FusionFS -q prod-devel -k zepto-vn-eval -n $numNode -t 7 ./startTest.sh $numNode 

#rm /intrepid-fs0/users/tonglin/persistent/*
#rm /intrepid-fs0/users/tonglin/persistent/Register_$numNode
#rm /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode

rm /intrepid-fs0/users/tonglin/persistent/Register_$numNode
rm /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode

touch /intrepid-fs0/users/tonglin/persistent/Register_$numNode
touch /intrepid-fs0/users/tonglin/persistent/neighbor_$numNode


cqsub -p FusionFS -k zepto-vn-eval -n $numNode -t $runTime ./r-node_start_general.sh $numNode $protocol

#ls ./hosts >> neighbor_$numNode
#./fixNeighbor.sh neighbor_$numNode

#now hash-phm and client can use neibor file.



