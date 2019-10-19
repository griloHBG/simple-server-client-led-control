/*
 ============================================================================
 Name        : esp-emb-server.c
 Author      : Grilo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//Got from https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main( int argc, char *argv[] )
{
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;

   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5002;

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   int reuse = 1;

   setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

   listen(sockfd,5);
   clilen = sizeof(cli_addr);

   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }

   /* If connection is established then start communicating */
   bzero(buffer,256);

   n = read( newsockfd,buffer,255 );

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   printf("Here is the message: ##%s##\n",buffer);

   int bad_value = 1;
   int lets_exit = 0;
   do
   {
	   do
	   {
		   printf("Turn LED on (1) or off (0)?\tEnter q to exit.\n");
		   bzero(buffer,256);
		   fgets(buffer,255,stdin);

		   if((strcmp("1\n", buffer) == 0) || strcmp("0\n", buffer) == 0)
		   {
			   bad_value = 0;
		   }
		   else if (strcmp("q\n", buffer) == 0)
		   {
			   bad_value = 0;
			   lets_exit = 1;
		   }
		   else
		   {
			   bad_value = 1;
			   printf("\nOnly 0, 1 or q are acceptable inputs.\n");

		   }
	   }while (bad_value == 1);

	   /* Write a response to the client */
	   n = write(newsockfd, buffer, sizeof(buffer));

	   if (n < 0) {
		  perror("ERROR writing to socket");
		  exit(1);
	   }

   }while (!lets_exit);

   close(sockfd);
   close(newsockfd);

   printf("\n--- Server ended! ---\n");

   return 0;
}
