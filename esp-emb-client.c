/*
 ============================================================================
 Name        : esp-emb-client.c
 Author      : Grilo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//Got from https://www.tutorialspoint.com/unix_sockets/socket_client_example.htm

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main(int argc, char *argv[])
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];

	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[2]);

	/* Create a socket point */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	server = gethostbyname(argv[1]);

	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		exit(1);
	}

	/* Now ask for a message from the user, this message
	* will be read by server
	*/

	printf("Connection stablished!\n");
	bzero(buffer,256);
	//fgets(buffer,255,stdin);
	strcpy(buffer, "Beaglebone Black Wifi listening!");

	/* Send message to the server */
	n = write(sockfd, buffer, strlen(buffer));

	if (n < 0) {
		perror("ERROR writing to socket");
		exit(1);
	}

	int lets_exit = 0;

	FILE* led = fopen("/sys/class/leds/beaglebone\:green\:usr0/brightness", "w");

	if(led == NULL)
	{
		perror("ERROR opening led file descriptor");
		exit(1);
	}

	char high[] = "1";
	char low[]  = "0";

	do
	{
		/* Now read server response */
		bzero(buffer,256);
		n = read(sockfd, buffer, 255);

		if (n < 0) {
			perror("ERROR reading from socket");
			exit(1);
		}

		if(strcmp(buffer, "0\n") == 0)
		{
			fwrite(low, 1, sizeof(low), led);
			printf("\n\t\tLED apagado\n");
		}
		else if (strcmp(buffer, "1\n") == 0)
		{
			fwrite(high, 1, sizeof(high), led);
			printf("\n\t\tLED aceso\n");
		}
		else if (strcmp(buffer, "q\n") == 0)
		{
			lets_exit = 1;
		}

		fflush(led);

	} while(!lets_exit);

	fclose(led);
	close(sockfd);


	printf("\n--- Client ended! ---\n");

	return 0;
}
