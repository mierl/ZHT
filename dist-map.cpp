#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stdlib.h>
#include <map>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sstream>



using namespace std;

struct timeval tp;

bool DEBUG = false;//change from false

string CONFIG_FILE = "nodes.txt";
int MAX_NUM_REPLICAS = 3;

struct HostEntity {
	struct sockaddr_in si;
	int sock;
	string host;
	int port;
	bool valid;
};

class Element {
public:
	Element() {
		valid = true;
	}

	void setMsg(string m) {
		//strcpy(msg, m);
		msg = m;
	}

	string getMsg() {
		//strcpy(msg, m);
		return msg;
	}

	void setValid() {
		valid = true;
	}
	bool isValid() {
		return valid;
	}

	void setInvalid() {
		valid = false;
	}
private:
	bool valid;
	string msg;

};

const int BUFLEN = 1024;
//const int PORT=50000;
int PORT = 30000;

void *UDPServer(void*);
void diep(string s) {
	perror(s.c_str());
	//exit(1);
}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

class MAP {
public:
	MAP() {

	}

	bool insert(string k, Element v) {
		if (DEBUG)
			cout << "insert(" << k << "," << v.getMsg() << ")" << endl;
		pthread_mutex_lock(&lock);

		map<string, Element>::iterator it;
		it = m.find(k);
		if (it != m.end()) {
			//return (*it).second;
			it->second = v;
		} else {
			m.insert((pair<string, Element> (k, v)));
		}
		//should check if insert or update was succesful
		pthread_mutex_unlock(&lock);
		return true;
	}

	void clear() {
		pthread_mutex_lock(&lock);
		m.clear();
		pthread_mutex_unlock(&lock);
	}

	bool remove(string k) {
		if (DEBUG)
			cout << "remove(" << k << ")" << endl;
		pthread_mutex_lock(&lock);
		map<string, Element>::iterator it;
		it = m.find(k);
		bool ret;
		if (it != m.end()) //end: not found?
		{
			m.erase(it);
			ret = true;
		} else {
			if (DEBUG)
				cout << "remove failed... key not found..." << endl;
			ret = false;
		}

		pthread_mutex_unlock(&lock);
		return ret;
	}

	Element find(string k) {
		if (DEBUG)
			cout << "find(" << k << ")" << endl;
		map<string, Element>::iterator it;
		it = m.find(k);
		if (it != m.end()) {
			//return (*it).second;
			return it->second;
		} else {
			Element e;
			e.setInvalid();
			return e;

		}
	}

	int size() {
		return m.size();
	}

	void print() {
		map<string, Element>::iterator it;

		cout << "MAP contains " << size() << " elements: \nkey\tvalue\n";
		for (it = m.begin(); it != m.end(); it++)
			cout << it->first << " => " << (it->second).getMsg() << endl;

	}

private:
	map<string, Element> m;
};

int min(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

class CommunicationLayer {
public:

	CommunicationLayer() {
	}

	bool init(string confFile) {

		string fileName = confFile;
		idSet = false;
		machIPs = getMachineIP();
		if (machIPs.size() > 0) {
			cout << "Getting machine name/ip was successful" << endl;
		} else {
			cout << "Getting machine name/ip failed" << endl;

		}
		bool ret = startServer();
		if (ret) {
			cout << "Start server was successful" << endl;
		} else {
			cout << "Start server failed" << endl;
		}

		ret = getMembershipInfo(fileName);
		if (ret) {
			cout << "Get membership succesful" << endl;
		} else {
			cout << "Start server failed" << endl;
		}

		NUM_REPLICAS = min(MAX_NUM_REPLICAS, endpoint_table.size());
		cout << "Number of replicas set to " << NUM_REPLICAS << endl;

		cout << "Node " << getId()
				<< ": finished initializing the CommunicationLayer!" << endl;
		return true;

	}

	bool deInit() {
		bool ret = stopServer();
		if (ret) {
			cout << "Stop server was successful" << endl;
		} else {
			cout << "Stop server failed" << endl;
		}
		cout << "Node " << getId()
				<< ": finished de-initializing the CommunicationLayer!" << endl;
		return true;
	}

	~CommunicationLayer() {

	}

	void clear() {
		//not implemented
	}

	void print() {
		//not implemented
	}

	int size() {
		//not implemented
		return 0;
	}

	vector<string> getMachineIP() {
		vector<string> machineIPs;
		struct hostent *he;
		int i;
		char buf[80];

		if (gethostname(buf, sizeof(buf)) == -1) {
			if (DEBUG)
				perror("gethostname");
			//exit(1);
		}

		if ((he = gethostbyname(buf)) == 0) {
			if (DEBUG)
				herror("gethostbyname");
			//exit(1);
		}
		if (DEBUG)
			printf("Your Host Name : %s\n", he->h_name);
		string name(he->h_name);
		machineIPs.push_back(name);
		for (i = 0; he->h_addr_list[i] != '\0'; i++) {
			if (DEBUG)
				printf("Your %d. IP Address : %s\n", i + 1, inet_ntoa(
						*((struct in_addr *) he->h_addr_list[i])));
			string ip(inet_ntoa(*((struct in_addr *) he->h_addr_list[i])));
			machineIPs.push_back(ip);
		}

		return machineIPs;
	}

	bool isCurrentNode(string host) {//just compare IP, not with port
		for (int i = 0; i < machIPs.size(); i++) {
			if (host.compare(machIPs[i]) == 0) {
				return true;
			}
		}
		return false;
	}

	bool getMembershipInfo(string fileName) {
		ifstream in(fileName.c_str(), ios::in);//make a file input stream
		string host;
		int port;

		if (!in.is_open()) {
			return false;
		}

		if (!in.eof()) {
			in >> host >> port;
		}

		while (!in.eof()) {
			HostEntity ee;

			struct sockaddr_in si_other;


			int s, i, slen = sizeof(si_other);
			//char buf[BUFLEN];
			if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
				diep("socket");
			memset((char *) &si_other, 0, sizeof(si_other));
			si_other.sin_family = AF_INET;
			si_other.sin_port = htons(port);
			if (inet_aton(host.c_str(), &si_other.sin_addr) == 0) {
				fprintf(stderr, "inet_aton() failed\n"); //address to number
				//exit(1);
			}

			ee.si = si_other;
			ee.sock = s;
			ee.host = host;
			ee.port = port;
			ee.valid = true;
			addEndpointTable(ee);

			if (isCurrentNode(host)) {
				setId(getNumberOfNodes() - 1);
			}

			in >> host >> port;

		}
		in.close();

		cout << "finished reading membership info, for a total of "
				<< getNumberOfNodes() << " nodes" << endl;
		return true;

	}

	bool startServer() {
		serverThreadRunning = true;

		int rc;
		long t = 0;
		cout << "Creating server thread..." << endl;
		//CommunicationLayer *cl = this;
		rc = pthread_create(&serverThread, NULL, UDPServer, (void *) t);
		if (rc) {
			cout << "ERROR; return code from pthread_create() is " << rc
					<< endl;
			return false;
		}
		return true;

	}

	bool stopServer() {
		serverThreadRunning = false;
		void *status;
		int rc;

		rc = pthread_join(serverThread, &status);

		if (rc) {
			printf("ERROR; return code from pthread_join()	is %d\n", rc);
			return false;
		}
		printf("Main: completed join having a status of %ld\n", (long) status);
		return true;
	}

	int getId() {
		return id;

	}

	void setId(int i) {
		id = i;
	}

	/*
	 bool join()
	 {
	 cout<<"starting join..."<<endl;
	 sleep(1);
	 struct sockaddr_in si_other;
	 int s, i, slen=sizeof(si_other);
	 char buf[BUFLEN];
	 if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	 diep("socket");
	 memset((char *) &si_other, 0, sizeof(si_other));
	 si_other.sin_family = AF_INET;
	 si_other.sin_port = htons(PORT);
	 if (inet_aton(bootStrapPoint.c_str(), &si_other.sin_addr)==0)
	 {
	 fprintf(stderr, "inet_aton() failed\n");
	 exit(1);
	 }
	 printf("Sending packet...\n");
	 sprintf(buf, "join");
	 if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, slen)==-1)
	 diep("sendto()");

	 close(s);
	 return true;

	 }
	 */

	bool leave() {
		return true;
	}

	Element find(string key) {
		if (DEBUG)
			cout << "starting remote find..." << endl;//modify: insert->find
		//sleep(1);


		int keyLocation = getLocation(key);
		int numKeyInc = 0;
		int numReplicasStored = 0;

		for (int iReplica = 0; iReplica < NUM_REPLICAS && numKeyInc
				< endpoint_table.size(); iReplica++) {

			//the location is the current node
			if (keyLocation == getId()) {
				//insert
				Element e = localHM.find(key);
				if (e.isValid()) {
					//found element
					return e;
				} else {
					numReplicasStored++;
					if (DEBUG)
						cout << "failed to find in localHM" << endl;
				}
			}
			//make sure remote node is valid
			else {
				while (!endpoint_table[keyLocation].valid) {
					keyLocation++;
					numKeyInc++;
					if (keyLocation >= endpoint_table.size()) {
						keyLocation = 0;
					}
				}

				struct sockaddr_in si_other = endpoint_table[keyLocation].si;
				int s, i, slen = sizeof(si_other);
				char buf[BUFLEN];
				//if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
				//	diep("socket");
				s = endpoint_table[keyLocation].sock;
				//memset((char *) &si_other, 0, sizeof(si_other));
				//si_other.sin_family = AF_INET;
				//si_other.sin_port = htons(PORT);
				//if (inet_aton(bootStrapPoint.c_str(), &si_other.sin_addr)==0)
				//{
				//	fprintf(stderr, "inet_aton() failed\n");
				//	exit(1);
				//}
				if (DEBUG)
					printf("Sending packet...\n");
				//sprintf(buf, "%s %s", key.c_str(), value.getMsg().c_str());

				/*
				 string msg = serializeMsg("insert", key, value);
				 cout<<"serialized: "<<msg<<endl;
				 string com, k;
				 Element v;
				 bool ret = deserializeMsg(msg, &com, &k, &v);
				 cout<<"deserialized: "<<com<<" "<<k<<" "<<v.getMsg()<<endl;;
				 */

				Element e;
				e.setInvalid();
				if (sendto(s, serializeMsg("find", key, e).c_str(), BUFLEN, 0,
						(struct sockaddr *) &si_other, slen) == -1) {
					if (DEBUG)
						diep("sendto()");
					//return false;
				}

				const int ackBUFLEN = 5;
				char bufACK[ackBUFLEN];
				struct timeval tv;
				tv.tv_sec = 1; /* 30 Secs Timeout */
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
						sizeof(struct timeval));
				int numBytes = recvfrom(s, bufACK, ackBUFLEN, 0,
						(struct sockaddr *) &si_other, (socklen_t*) &slen);
				if (numBytes > 0) {
					if (DEBUG)
						cout << "received ACK packet: " << bufACK << endl;
					string sBufACK(bufACK);
					if (sBufACK.compare("ACKK") == 0) {
						char buf[BUFLEN];
						struct timeval tv;
						tv.tv_sec = 1; /* 30 Secs Timeout */
						setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,
								(struct timeval *) &tv, sizeof(struct timeval));
						int numBytes = recvfrom(s, buf, BUFLEN, 0,
								(struct sockaddr *) &si_other,
								(socklen_t*) &slen);
						if (numBytes > 0) {
							if (DEBUG)
								cout << "received Element: " << buf << endl;
							//string msg(buf);

							string msg(buf);
							string com, k;
							Element v;
							bool ret = deserializeMsg(msg, &com, &k, &v);

							if (ret) {
								if (DEBUG)
									cout << "deserialized: " << com << " " << k
											<< " " << v.getMsg() << endl;
								return v;
							}

						} else {
							if (DEBUG)
								cout << "didn't receive anything...." << endl;

						}
					}
					//not found, go to next replica
					else {
						if (DEBUG)
							cout << "not found, go to next replica...." << endl;

					}

				} else {
					if (DEBUG)
						cout << "failed to receive ACK..." << endl;
				}
			}
			keyLocation++;
			numKeyInc++;
			numReplicasStored++;
		}
		//close(s);
		if (DEBUG)
			cout << "finished remote insert with " << numReplicasStored
					<< " replicas stored!" << endl;
		Element e;
		e.setInvalid();

		return e;
	}

	int getLocation(string key) {
		int SUM = 0;
		for (int i = 0; i < key.length(); i++) {
			SUM += (int) key[i];
		}
		return SUM % getNumberOfNodes();
	}

	string serializeMsg(string command, string key, Element value) {
		string msg(command);
		msg.append(";");
		msg.append(key);
		if (value.isValid()) {
			msg.append(";");
			msg.append(value.getMsg());
		}

		if (msg.length() >= BUFLEN) {
			cout
					<< "warning: not enough buffer space, increase BUFLEN to at least "
					<< msg.length() << " from " << BUFLEN << endl;
		}
		return msg;

	}

	vector<string> tokenize(string msg, char delim) {
		stringstream ss(msg, stringstream::in);
		string s;
		vector<string> tokens;
		while (getline(ss, s, delim)) {
			tokens.push_back(s);
		}
		return tokens;

	}

	bool deserializeMsg(string msg, string * command, string * key,
			Element * value) {
		Element v;
		v.setValid();

		vector<string> tokens = tokenize(msg, ';');
		if (DEBUG)
			cout << "number of tokens found: " << tokens.size() << " in "
					<< msg << endl;

		if ((tokens[0].compare("insert") == 0 && tokens.size() == 3)
				|| (tokens[0].compare("result") == 0 && tokens.size() == 3)) {
			*command = tokens[0];
			*key = tokens[1];

			v.setMsg(tokens[2]);
			*value = v;
		} else if ((tokens[0].compare("remove") == 0 && tokens.size() == 2)
				|| (tokens[0].compare("find") == 0 && tokens.size() == 2)) {
			*command = tokens[0];
			*key = tokens[1];

		} else {
			cout
					<< "operation not supported, or not enough tokens to desialize..."
					<< endl;

			//operation not supported
			return false;
		}

		return true;
	}

	//might have to synchronize
	bool insertLocal(string k, Element v) {
		return localHM.insert(k, v);
	}

	//might have to synchronize
	bool removeLocal(string k) {
		return localHM.remove(k);
	}

	//might have to synchronize
	Element findLocal(string k) {
		return localHM.find(k);
	}

	bool insert(string key, Element value) {
		if (DEBUG)
			cout << "starting remote insert..." << endl;
		//sleep(1);


		int keyLocation = getLocation(key);
		int numKeyInc = 0;
		int numReplicasStored = 0;

		for (int iReplica = 0; iReplica < NUM_REPLICAS && numKeyInc
				< endpoint_table.size(); iReplica++) {

			//the location is the current node
			if (keyLocation == getId()) {
				//insert
				if (localHM.insert(key, value))
					numReplicasStored++;
				else {
					cout << "failed to insert into localHM" << endl;
				}
			}
			//make sure remote node is valid
			else {
				while (!endpoint_table[keyLocation].valid) {
					keyLocation++;
					numKeyInc++;
					if (keyLocation >= endpoint_table.size()) {
						keyLocation = 0;
					}
				}

				struct sockaddr_in si_other = endpoint_table[keyLocation].si;
				int s, i, slen = sizeof(si_other);
				char buf[BUFLEN];
				//if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
				//	diep("socket");
				s = endpoint_table[keyLocation].sock;
				//memset((char *) &si_other, 0, sizeof(si_other));
				//si_other.sin_family = AF_INET;
				//si_other.sin_port = htons(PORT);
				//if (inet_aton(bootStrapPoint.c_str(), &si_other.sin_addr)==0)
				//{
				//	fprintf(stderr, "inet_aton() failed\n");
				//	exit(1);
				//}
				if (DEBUG)
					printf("Sending packet...\n");
				//sprintf(buf, "%s %s", key.c_str(), value.getMsg().c_str());

				/*
				 string msg = serializeMsg("insert", key, value);
				 cout<<"serialized: "<<msg<<endl;
				 string com, k;
				 Element v;
				 bool ret = deserializeMsg(msg, &com, &k, &v);
				 cout<<"deserialized: "<<com<<" "<<k<<" "<<v.getMsg()<<endl;;
				 */

				if (sendto(s, serializeMsg("insert", key, value).c_str(),
						BUFLEN, 0, (struct sockaddr *) &si_other, slen) == -1) {
					if (DEBUG)
						diep("sendto()");
					return false;
				}

				const int ackBUFLEN = 5;
				char bufACK[ackBUFLEN];
				struct timeval tv;
				tv.tv_sec = 1; /* 30 Secs Timeout */
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
						sizeof(struct timeval));
				int numBytes = recvfrom(s, bufACK, ackBUFLEN, 0,
						(struct sockaddr *) &si_other, (socklen_t*) &slen);
				if (numBytes > 0) {
					if (DEBUG)
						cout << "received ACK packet: " << bufACK << endl;
				} else {
					if (DEBUG)
						cout << "failed to receive ACK..." << endl;
				}
			}
			keyLocation++;
			numKeyInc++;
			numReplicasStored++;
		}
		//close(s);
		if (DEBUG)
			cout << "finished remote insert with " << numReplicasStored
					<< " replicas stored!" << endl;

		return true;
	}

	bool remove(string key) {
		if (DEBUG)
			cout << "starting remote insert..." << endl;
		//sleep(1);


		int keyLocation = getLocation(key);
		int numKeyInc = 0;
		int numReplicasRemoved = 0;

		for (int iReplica = 0; iReplica < NUM_REPLICAS && numKeyInc
				< endpoint_table.size(); iReplica++) {

			//the location is the current node
			if (keyLocation == getId()) {
				//insert
				if (localHM.remove(key))
					numReplicasRemoved++;
				else {
					cout << "failed to remove from localHM" << endl;
				}
			}
			//make sure remote node is valid
			else {
				while (!endpoint_table[keyLocation].valid) {
					keyLocation++;
					numKeyInc++;
					if (keyLocation >= endpoint_table.size()) {
						keyLocation = 0;
					}
				}

				struct sockaddr_in si_other = endpoint_table[keyLocation].si;
				int s, i, slen = sizeof(si_other);
				char buf[BUFLEN];
				//if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
				//	diep("socket");
				s = endpoint_table[keyLocation].sock;
				//memset((char *) &si_other, 0, sizeof(si_other));
				//si_other.sin_family = AF_INET;
				//si_other.sin_port = htons(PORT);
				//if (inet_aton(bootStrapPoint.c_str(), &si_other.sin_addr)==0)
				//{
				//	fprintf(stderr, "inet_aton() failed\n");
				//	exit(1);
				//}
				if (DEBUG)
					printf("Sending packet...\n");
				//sprintf(buf, "%s %s", key.c_str(), value.getMsg().c_str());

				/*
				 string msg = serializeMsg("insert", key, value);
				 cout<<"serialized: "<<msg<<endl;
				 string com, k;
				 Element v;
				 bool ret = deserializeMsg(msg, &com, &k, &v);
				 cout<<"deserialized: "<<com<<" "<<k<<" "<<v.getMsg()<<endl;;
				 */

				Element e;
				e.setInvalid();
				if (sendto(s, serializeMsg("remove", key, e).c_str(), BUFLEN,
						0, (struct sockaddr *) &si_other, slen) == -1) {
					if (DEBUG)
						diep("sendto()");
					return false;
				}

				const int ackBUFLEN = 5;
				char bufACK[ackBUFLEN];
				struct timeval tv;
				tv.tv_sec = 1; /* 30 Secs Timeout */
				setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
						sizeof(struct timeval));
				int numBytes = recvfrom(s, bufACK, ackBUFLEN, 0,
						(struct sockaddr *) &si_other, (socklen_t*) &slen);
				if (numBytes > 0) {
					if (DEBUG)
						cout << "received ACK packet: " << bufACK << endl;
					string sBufACK(bufACK);
					if (sBufACK.compare("ACKK") == 0) {
						numReplicasRemoved++;
					} else {

					}

				} else {
					if (DEBUG)
						cout << "failed to receive ACK..." << endl;
				}
			}
			keyLocation++;
			numKeyInc++;
			//numReplicasStored++;
		}
		//close(s);
		if (DEBUG)
			cout << "finished remote remove with " << numReplicasRemoved
					<< " replicas removed!" << endl;

		if (numReplicasRemoved > 0) {
			return true;
		} else {
			return false;
		}

	}

	/*
	 string getBootStrapPoint()
	 {
	 return bootStrapPoint;

	 }*/
	/*
	 void setBootStrapPoint(string s)
	 {
	 bootStrapPoint = s;

	 }*/

	bool isServerThreadRunning() {
		return serverThreadRunning;
	}

	void setUpdateEndpointTable(char * h, int p) {

	}

	void addEndpointTable(HostEntity ee) {

		endpoint_table.push_back(ee);
	}

	int getNumberOfNodes() {
		return endpoint_table.size();

	}

private:

	//string bootStrapPoint;
	int id;
	bool idSet;
	vector<HostEntity> endpoint_table;
	vector<string> machIPs;

	pthread_t serverThread;
	bool serverThreadRunning;

	MAP localHM;
	int NUM_REPLICAS;
};

CommunicationLayer cl;

void *UDPServer(void *td) {
	cout << "starting server..." << endl;

	struct sockaddr_in si_me, si_other;
	int s, i, slen = sizeof(si_other);
	char buf[BUFLEN];
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		diep("socket");

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);//??
	if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) < 0)
		diep("bind");

	int tid;
	tid = cl.getId();

	struct timeval tv;
	tv.tv_sec = 1; /* 30 Secs Timeout */

	cout << "storing bootstraping info..." << endl;

	//char ACK[];
	const int ACKBUF_LENGTH = 5;
	char ACKe[ACKBUF_LENGTH] = "ACKe";
	ACKe[ACKBUF_LENGTH - 1] = '\0';
	char ACK[ACKBUF_LENGTH] = "ACKK";
	ACK[ACKBUF_LENGTH - 1] = '\0';
	char NACK[ACKBUF_LENGTH] = "NACK";
	NACK[ACKBUF_LENGTH - 1] = '\0';

	cout << "Server thread started with ID " << tid
			<< " and listening on port " << PORT << endl;
	while (cl.isServerThreadRunning()) {

		if (DEBUG)
			cout << "waiting..." << endl;

		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *) &tv,
				sizeof(struct timeval));
		if (DEBUG)
			printf("\n Before numBytes is calculated: Received message from %s:%d\nData: \n", inet_ntoa(
					si_other.sin_addr), ntohs(si_other.sin_port)); //added by hjin

		int numBytes = recvfrom(s, buf, BUFLEN, 0,
				(struct sockaddr *) &si_other, (socklen_t*) &slen);
		//recvfrom()is suppose to receive msg from a socket, say, si_other
		//but here the socket is not assigned and from which we of course receive nothing---tony
		if (numBytes > 0) {//here the problem sits---tony
			if (DEBUG)
				printf("numBytes =%d, Received message from %s:%d\nData: %s\n\n", numBytes, inet_ntoa(
						si_other.sin_addr), ntohs(si_other.sin_port), buf);

			string msg(buf);
			string com, k;
			Element v;
			bool ret = cl.deserializeMsg(msg, &com, &k, &v);

			if (ret) {
				if (DEBUG)
					cout << "deserialized: " << com << " " << k << " "
							<< v.getMsg() << endl;

				if (com.compare("insert") == 0) {

					if (cl.insertLocal(k, v)) {
						//send ACK
						if (DEBUG)
							cout << "ACK..." << endl;
						if (sendto(s, ACK, ACKBUF_LENGTH, 0,
								(struct sockaddr *) &si_other, slen) == -1)
							if (DEBUG)
								diep("sendto()");

					} else {
						//send NACK
						if (DEBUG)
							cout << "NACK..." << endl;
						if (sendto(s, NACK, ACKBUF_LENGTH, 0,
								(struct sockaddr *) &si_other, slen) == -1)
							if (DEBUG)
								diep("sendto()");
					}
				} else if (com.compare("remove") == 0) {
					if (cl.removeLocal(k)) {
						//send ACK
						if (DEBUG)
							cout << "ACK..." << endl;
						if (sendto(s, ACK, ACKBUF_LENGTH, 0,
								(struct sockaddr *) &si_other, slen) == -1)
							if (DEBUG)
								diep("sendto()");
					} else {
						//send NACK
						if (DEBUG)
							cout << "NACK..." << endl;
						if (sendto(s, NACK, ACKBUF_LENGTH, 0,
								(struct sockaddr *) &si_other, slen) == -1)
							if (DEBUG)
								diep("sendto()");
					}
				} else if (com.compare("find") == 0) {
					Element e = cl.findLocal(k);
					if (e.isValid()) {
						//send e
						if (DEBUG)
							cout << "send e..." << endl;
						if (DEBUG)
							cout << "ACK..." << endl;
						if (sendto(s, ACK, ACKBUF_LENGTH, 0,
								(struct sockaddr *) &si_other, slen) == -1)
							if (DEBUG)
								diep("sendto()");
						if (sendto(s, cl.serializeMsg("result", k, e).c_str(),
								BUFLEN, 0, (struct sockaddr *) &si_other, slen)
								== -1)
							if (DEBUG)
								diep("sendto()");
					} else {
						//send NACK
						if (DEBUG)
							cout << "NACK..." << endl;
						if (sendto(s, NACK, ACKBUF_LENGTH, 0,
								(struct sockaddr *) &si_other, slen) == -1)
							if (DEBUG)
								diep("sendto()");

					}
				} else {
					//opearation not supported
					//send NACK
					if (DEBUG)
						cout << "NACK..." << endl;
					if (sendto(s, NACK, ACKBUF_LENGTH, 0,
							(struct sockaddr *) &si_other, slen) == -1)
						if (DEBUG)
							diep("sendto()");
				}

			} else {
				if (DEBUG)
					cout << "deserialization failed..." << endl;
				if (DEBUG)
					cout << "NACK..." << endl;
				if (sendto(s, NACK, ACKBUF_LENGTH, 0,
						(struct sockaddr *) &si_other, slen) == -1)
					if (DEBUG)
						diep("sendto()");
				//send NACK

			}

		} else {
			if (DEBUG)
				cout << "received nothing, trying again..." << endl;

		}
	}
	cout << "Shuting down server..." << endl;
	close(s);
	pthread_exit(NULL);

}

double getTime_usec() {
	gettimeofday(&tp, NULL);
	return static_cast<double> (tp.tv_sec) * 1E6
			+ static_cast<double> (tp.tv_usec);

}

double getTime_msec() {
	gettimeofday(&tp, NULL);
	return static_cast<double> (tp.tv_sec) * 1E3
			+ static_cast<double> (tp.tv_usec) / 1E3;

}

double getTime_sec() {
	gettimeofday(&tp, NULL);
	return static_cast<double> (tp.tv_sec) + static_cast<double> (tp.tv_usec)
			/ 1E6;

}

string randomString(int len) {
	string s(len, ' ');
	static const char alphanum[] = "0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	//s[len] = 0;
	return s;
}

void mapTest(int numOper) {
	double start, end;
	MAP myHM;



	int TEST_SIZE = numOper * 1000;
	string keys[TEST_SIZE];
	for (int i = 0; i < TEST_SIZE; i++) {
		keys[i] = randomString(10);
	}

	sleep(5);//added by tony

	start = getTime_msec();
	for (int i = 0; i < TEST_SIZE; i++) {
		Element e;
		e.setMsg(".");
		myHM.insert(keys[i], e);
	}
	end = getTime_msec();
	cout << "inserted " << TEST_SIZE << " elements (" << end - start << " ms)"
			<< endl;
	if (DEBUG)
		myHM.print();

	int found = 0;
	start = getTime_msec();

	for (int i = 0; i < TEST_SIZE; i++) {
		Element e = myHM.find(keys[i]);
		if (e.isValid()) {
			found++;
		}
	}
	end = getTime_msec();
	cout << "found " << found << " elements out of " << TEST_SIZE << " ("
			<< end - start << " ms)" << endl;

	found = 0;
	start = getTime_msec();
	for (int i = 0; i < TEST_SIZE; i++) {
		if (myHM.remove(keys[i])) {
			found++;
		}
	}
	end = getTime_msec();
	cout << "removed " << found << " elements out of " << TEST_SIZE
			<< " elements (" << end - start << " ms)" << endl;

	if (DEBUG)
		myHM.print();

}

void mapTestInt() {
	MAP myHM;
	double start, end;

	bool RUNNING = true;
	while (RUNNING) {
		string input;
		cout << endl << "enter next command" << endl;
		cin >> input;
		if (input.compare("help") == 0) {
			cout << "list of possible commands:" << endl;
			cout << "insert <key> <value>" << endl;
			cout << "find <key>" << endl;
			cout << "remove <key>" << endl;
			cout << "clear" << endl;
			cout << "print" << endl;
			cout << "quit" << endl;
			cout << "help" << endl;
		} else if (input.compare("insert") == 0) {
			string key, value;
			cin >> key >> value;
			Element e;
			e.setMsg(value);
			start = getTime_msec();
			bool ret = myHM.insert(key, e);
			end = getTime_msec();

			if (ret) {
				cout << "Insert succesful: " << key << " => " << value << "( "
						<< (end - start) << " ms)" << endl;
			} else {
				cout << "Insert failed: " << key << " => " << value << "( "
						<< (end - start) << " ms)" << endl;

			}
		} else if (input.compare("find") == 0) {
			string key, value;
			cin >> key;

			start = getTime_msec();
			Element e = myHM.find(key);
			end = getTime_msec();

			if (e.isValid()) {
				cout << "Find succesful: " << key << " => " << e.getMsg()
						<< "( " << (end - start) << " ms)" << endl;
			} else {
				cout << "Find failed: " << key << " not found" << "( " << (end
						- start) << " ms)" << endl;

			}

		} else if (input.compare("remove") == 0) {
			string key, value;
			cin >> key;

			start = getTime_msec();
			bool ret = myHM.remove(key);
			end = getTime_msec();

			if (ret) {
				cout << "Remove succesful: " << key << "( " << (end - start)
						<< " ms)" << endl;
			} else {
				cout << "Remove failed: " << key << " not found" << "( "
						<< (end - start) << " ms)" << endl;

			}

		} else if (input.compare("clear") == 0) {
			start = getTime_msec();
			myHM.clear();
			end = getTime_msec();
			cout << "Cleared all key,value pairs ( " << (end - start) << " ms)"
					<< endl;
		} else if (input.compare("print") == 0) {
			start = getTime_msec();
			myHM.print();
			end = getTime_msec();
			cout << "Printed " << myHM.size() << " key,value pairs ( " << (end
					- start) << " ms)" << endl;
		} else if (input.compare("quit") == 0) {
			cout << "quiting..." << endl;
			RUNNING = false;
		} else {
			cout
					<< "operation not supported, type 'help' to get a list of supported commands"
					<< endl;
		}

	}
}

void comLayerTest(int numOper) {
	double start, end;
	//MAP myHM;
	CommunicationLayer myHM = cl;

	int TEST_SIZE = numOper * 1000;
	string keys[TEST_SIZE];
	for (int i = 0; i < TEST_SIZE; i++) {
		keys[i] = randomString(10);
	}
	sleep(5);
	start = getTime_msec();
	for (int i = 0; i < TEST_SIZE; i++) {
		Element e;
		e.setMsg(".");
		myHM.insert(keys[i], e);
		//cout<<"+";
		//if (i%100 == 0)
		//{
		//	cout<<endl;
		//}
	}
	end = getTime_msec();
	cout << "inserted " << TEST_SIZE << " elements (" << end - start << " ms)"
			<< endl;
	if (DEBUG)
		myHM.print();

	int found = 0;
	start = getTime_msec();

	for (int i = 0; i < TEST_SIZE; i++) {
		Element e = myHM.find(keys[i]);
		if (e.isValid()) {
			found++;
		}
	}
	end = getTime_msec();
	cout << "found " << found << " elements out of " << TEST_SIZE << " ("
			<< end - start << " ms)" << endl;

	found = 0;
	start = getTime_msec();
	for (int i = 0; i < TEST_SIZE; i++) {
		if (myHM.remove(keys[i])) {
			found++;
		}
	}
	end = getTime_msec();
	cout << "removed " << found << " elements out of " << TEST_SIZE
			<< " elements (" << end - start << " ms)" << endl;

	if (DEBUG)
		myHM.print();

}

void comLayerTestInt() {
	//MAP myHM;
	CommunicationLayer myHM = cl;
	double start, end;

	bool RUNNING = true;
	while (RUNNING) {
		string input;
		cout << endl << "enter next command" << endl;
		cin >> input;
		if (input.compare("help") == 0) {
			cout << "list of possible commands:" << endl;
			cout << "insert <key> <value>" << endl;
			cout << "find <key>" << endl;
			cout << "remove <key>" << endl;
			cout << "clear" << endl;
			cout << "print" << endl;
			cout << "quit" << endl;
			cout << "help" << endl;
		} else if (input.compare("insert") == 0) {
			string key, value;
			cin >> key >> value;
			Element e;
			e.setMsg(value);
			start = getTime_msec();
			bool ret = myHM.insert(key, e);
			end = getTime_msec();

			if (ret) {
				cout << "Insert succesful: " << key << " => " << value << "( "
						<< (end - start) << " ms)" << endl;
			} else {
				cout << "Insert failed: " << key << " => " << value << "( "
						<< (end - start) << " ms)" << endl;

			}
		} else if (input.compare("find") == 0) {
			string key, value;
			cin >> key;

			start = getTime_msec();
			Element e = myHM.find(key);
			end = getTime_msec();

			if (e.isValid()) {
				cout << "Find succesful: " << key << " => " << e.getMsg()
						<< "( " << (end - start) << " ms)" << endl;
			} else {
				cout << "Find failed: " << key << " not found" << "( " << (end
						- start) << " ms)" << endl;

			}

		} else if (input.compare("remove") == 0) {
			string key, value;
			cin >> key;

			start = getTime_msec();
			bool ret = myHM.remove(key);
			end = getTime_msec();

			if (ret) {
				cout << "Remove succesful: " << key << "( " << (end - start)
						<< " ms)" << endl;
			} else {
				cout << "Remove failed: " << key << " not found" << "( "
						<< (end - start) << " ms)" << endl;

			}

		} else if (input.compare("clear") == 0) {
			start = getTime_msec();
			myHM.clear();
			end = getTime_msec();
			cout << "Cleared all key,value pairs ( " << (end - start) << " ms)"
					<< endl;
		} else if (input.compare("print") == 0) {
			start = getTime_msec();
			myHM.print();
			end = getTime_msec();
			cout << "Printed " << myHM.size() << " key,value pairs ( " << (end
					- start) << " ms)" << endl;
		} else if (input.compare("quit") == 0) {
			cout << "quiting..." << endl;
			RUNNING = false;
		} else {
			cout
					<< "operation not supported, type 'help' to get a list of supported commands"
					<< endl;
		}

	}
}

void doBarrier(string fileName) {

	bool lock = true;

	while (lock) {
		cout << "waiting for " << fileName << " to appear" << endl;
		ifstream in(fileName.c_str(), ios::in);

		if (in.is_open()) {
			lock = false;
			sleep(1);
			return;
		}
		in.close();
		sleep(1);

	}
}

void printHelp() {
	cout << "Help Screen" << endl;
	cout << "Supported options:" << endl;
	cout << "-debug : enables DEBUG output" << endl;
	cout << "-h : This help screen" << endl;
	cout << "-map : enable simple local map mode" << endl;
	cout << "-dist_map : enable distributed map mode" << endl;
	cout << "-int : enable interactive mode" << endl;
	cout
			<< "-perf <numOperations> : enable performance mode that runs <numOperations> inserts, finds, and removes"
			<< endl;
	cout
			<< "-barrier <fileLock> : enable barrier waiting for <fileLock> to appear before doing the performance test specified by -perf"
			<< endl;
	cout << "-membership <fileName> : enable membership config file" << endl;
}

int main(int argc, char* argv[]) {

	cout << "DistributedMap v0.0" << endl;

	bool localMap = true;
	bool distributedMap = false;
	bool interactive = true;
	bool barrier = false;
	string barrierLock;
	bool perf = false;
	int numOperations = 0;
	bool membership = false;
	string membershipFile;
	bool debug;

	for (int i = 1; i < argc; i++) {

		string curArg(argv[i]);
		if (curArg.compare("-debug") == 0) {
			debug = true;
			DEBUG = debug;
		} else if (curArg.compare("-h") == 0) {
			printHelp();
			return 0;
		} else if (curArg.compare("-map") == 0) {
			localMap = true;
		} else if (curArg.compare("-dist_map") == 0) {

		//	distributedMap = true;
		//	localMap = false;
			if (i + 1 != argc) {
				distributedMap = true;
				localMap = false;
				i++;
				PORT = atoi(argv[i]);

			} else {
				cout << "unsupported option for -dist_map: " << argv[i] << endl;
				cout
						<< "Acceptable option follows -dist_map should be a user specified port."
						<< endl;
				printHelp();
				return 0;

			}

		} else if (curArg.compare("-int") == 0) {
			interactive = true;
			perf = false;
			barrier = false;
		} else if (curArg.compare("-perf") == 0) {
			if (i + 1 != argc) {
				perf = true;
				i++;
				numOperations = atoi(argv[i]);
				interactive = false;

			} else {
				cout << "unsported option for -perf: " << argv[i] << endl;
				printHelp();
				return 0;

			}
		} else if (curArg.compare("-barrier") == 0) {
			if (i + 1 != argc) {
				barrier = true;
				i++;
				string s(argv[i]);
				barrierLock = s;
				interactive = false;
			} else {
				cout << "unsported option for -barrier: " << argv[i] << endl;
				printHelp();
				return 0;

			}
		} else if (curArg.compare("-membership") == 0) {
			if (i + 1 != argc) {
				membership = true;
				i++;
				string s(argv[i]);
				membershipFile = s;
			} else {
				cout << "unsported option for -membership_file: " << argv[i]
						<< endl;
				printHelp();
				return 0;

			}
		} else {
			cout << "unsported option: " << argv[i] << endl;
			printHelp();
			return 0;
		}
	}

	if (distributedMap && interactive == false && perf == false && barrier
			== false) {
		//do nothing, simply start the DistributedMap...
		cout << "Node up and running..." << endl;
		while (1) {
			sleep(1);
		}
		cl.deInit();
	} else if (localMap && interactive) {
		mapTestInt();
	} else if (localMap && perf) {
		mapTest(numOperations);
	} else if (distributedMap && interactive) {
		if (membership) {
			cl.init(membershipFile);
		} else {
			cl.init(CONFIG_FILE);

		}
		comLayerTestInt();
		cl.deInit();
	} else if (distributedMap && perf) {
		if (membership) {
			cl.init(membershipFile);
		} else {
			cl.init(CONFIG_FILE);

		}

		if (barrier) {
			doBarrier(barrierLock);
		}

		comLayerTest(numOperations);
		cl.deInit();
	} else {
		cout << "unsupported combination of options..." << endl;
		cout << "localMap = " << localMap << endl;
		cout << "distributedMap = " << distributedMap << endl;
		cout << "interactive = " << interactive << endl;
		cout << "barrier = " << barrier << endl;
		cout << "barrierLock = " << barrierLock << endl;
		cout << "membership = " << membership << endl;
		cout << "membershipFile  = " << membershipFile << endl;
		cout << "perf = " << perf << endl;
		cout << "numOperations = " << numOperations << endl;
	}

	return 0;
}

