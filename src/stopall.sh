echo "kill zhtserver"
fuser -k 50001/tcp
fuser -k 50002/tcp
fuser -k 50003/tcp
fuser -k 50004/tcp
fuser -k 50005/tcp
fuser -k 50006/tcp
fuser -k 50007/tcp
fuser -k 50008/tcp

killall -9 peerserver

