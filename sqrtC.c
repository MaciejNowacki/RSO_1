/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int
main ()
{
	int sockfd;
	socklen_t len;
	struct sockaddr_in address;
	double number;
	int length;
	scanf("%lf", &number);

	/*  Create a socket for the client.  */

	sockfd = socket (AF_INET, SOCK_STREAM, 0);

	/*  Name the socket, as agreed with the server.  */

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr ("127.0.0.1");
	address.sin_port = htons (9734);
	len = sizeof (address);

	/*  Now connect our socket to the server's socket.  */

	result = connect (sockfd, (struct sockaddr *) &address, len);

	if (result == -1)
	{
		perror ("oops: netclient");
		exit (1);
	}

	/*  We can now read/write via sockfd.  */

	write (sockfd, &number, 8);
	read (sockfd, &number, 8); 

	read (sockfd, &length, 4); 
	char * date = (char*) malloc (length * sizeof(char));
	read (sockfd, date, length);

 	printf ("The square root of this number is %f\n", number);
	printf("Current date: %s\n", date);

	close (sockfd);
	exit (0);
}
