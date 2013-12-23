#include <iostream>
#include <map>
#include <string>
using namespace std;

int main(){
	map<string, string> hmap;
	pair<map<string,string>::iterator,bool> ret;
		ret = hmap.insert(pair<string, string> ("key", "value"));


		if (ret.second == false) {
			cout <<"fail to insert."<<endl;
			return -3;
		}
		else
	return 0;
}


