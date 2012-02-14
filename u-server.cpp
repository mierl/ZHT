/* server.c - code for example server program that uses UDP */

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "zht_util.h"
int main(int argc, char *argv[]) {
	struct sockaddr_in sad; /* structure to hold server's address  */
	int port = 50000; /* protocol port number                */

	struct sockaddr_in cad; /* structure to hold client's address  */
	int alen; /* length of address                   */

	int serverSocket; /* socket descriptors  */

	char clientSentence[1000];
	char capitalizedSentence[1000];
	char buff[1000];
	int i, n;



	serverSocket = socket(PF_INET, SOCK_DGRAM, 0); /* CREATE SOCKET */
	if (serverSocket < 0) {
		fprintf(stderr, "socket creation failed\n");
		exit(1);
	}


	memset((char *) &sad, 0, sizeof(sad)); /* clear sockaddr structure   */
	sad.sin_family = AF_INET; /* set family to Internet     */
	sad.sin_addr.s_addr = INADDR_ANY; /* set the local IP address   */
	sad.sin_port = htons((u_short) port);/* set the port number        */

	if (bind(serverSocket, (struct sockaddr *) &sad, sizeof(sad)) < 0) {
		fprintf(stderr, "bind failed\n");
		exit(1);
	}

	while (1) {

		clientSentence[0] = '\0';

		alen = sizeof(struct sockaddr);
		socklen_t len = (socklen_t) alen;
		n = recvfrom(serverSocket, buff, sizeof(buff), 0,
				(struct sockaddr *) &cad, &len);
		strncat(clientSentence, buff, n);
		clientSentence[n] = '\0';

		printf("Server received :%s \n", clientSentence);
		cout <<endl;
		cout<< "Server received "<< n<<endl;
/*
		for (i = 0; i <= strlen(clientSentence); i++) {
			if ((clientSentence[i] >= 'a') && (clientSentence[i] <= 'z'))
				capitalizedSentence[i] = clientSentence[i] + ('A' - 'a');
			else
				capitalizedSentence[i] = clientSentence[i];
		}

		n = sendto(serverSocket, capitalizedSentence,
				strlen(capitalizedSentence) + 1, 0, (struct sockaddr *) &cad,
				alen);

		printf("Server sent %d bytes to client\n", n);
		*/
	}
	return 0;
}

