echo "usage: start file server(f), adminserver(a)"
f=$1
a=$2

if [[ $f == "f" ]]; then
./fileserver &
echo "fileserver started..."
fi

if [[ $a == "a" ]]; then
./adminserver -d node.cfg -n neighbor -z zht.cfg &
echo "adminserver started..."
fi

