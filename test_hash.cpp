/*
 * test_hash.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: tony
 */



#include <string>
#include "novoht.h"
#include <stdlib.h>
#include <sys/time.h>

#include "zht_util.h"
using namespace std;


int main(int argc, char * argv[]){

	//int num = atoi(argv[1]);
	string fileName = "hashmap.data";
	NoVoHT map = NoVoHT(fileName, 100000, 1000, 0.7);

	int lenString=10;
	Package package, package_ret;
			package.set_virtualpath(randomString(lenString)); //as key
			package.set_isdir(true);
			package.set_replicano(5); //orginal--Note: never let it be nagative!!!
			package.set_operation(3); // 3 for insert, 1 for look up, 2 for remove
			package.set_realfullpath(
					"Some-Real-longer-longer-and-longer-Paths--------");
			package.add_listitem("item-----1");
			package.add_listitem("item-----2");
			package.add_listitem("item-----3");
			package.add_listitem("item-----4");
			package.add_listitem("item-----5");
			string str = package.SerializeAsString();

		//int ret = db.set(package.virtualpath(), package_str); //virtualpath as key
		cout<<"Insert to pmap..."<<endl;
		string key = package.virtualpath();
		cout<<"key:"<<key<<endl;
		string value = package.SerializeAsString();
		cout<<"value:"<<value<<endl;
		cout<<"Insert: k-v ready. put..."<<endl;
		int ret = map.put(key,value);


		cout <<"insert:"<<ret<<endl;
		string*	 result = map.get(key);
		cout<<"Result = "<<*result<<endl;

		if(result==NULL){
			cout<<"lookup find nothing."<<endl;
		}
		int r =map.remove(key);
		cout<<"remove:"<<r<<endl;


		cout<<endl<<endl<<"Separate test finished."<<endl;


/*
		HostEntity destination;
		destination.host = "localhost";
		destination.port = 50000;
		int current_sock = -1;

		int r = simpleSend(str, destination, current_sock);
*/


	return 0;
}
