rm svr1/*
rm svr2/*
rm svr3/*
cp ./hash svr1/.
cp ./neighbor svr1/.
cp ./hash svr2/.
cp ./neighbor svr2/.
cp ./hash svr3/.
cp ./neighbor svr3/.

#svr1/hash 50000 neighbor &
svr2/hash 50001 neighbor &
svr3/hash 50002 neighbor 
