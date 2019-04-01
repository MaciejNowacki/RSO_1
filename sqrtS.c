#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <time.h>

int
main ()
{
	int server_sockfd, client_sockfd;
	socklen_t server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_sockfd = socket (AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl (INADDR_ANY);
	server_address.sin_port = htons (9734);
	server_len = sizeof (server_address);
	bind (server_sockfd, (struct sockaddr *) &server_address, server_len);

	/*  Create a connection queue, ignore child exit details and wait for clients.  */

	listen (server_sockfd, 5);

	signal (SIGCHLD, SIG_IGN);

	while (1)
	{
		
		printf ("Server waiting\n");
		
		/*  Accept connection.  */

		client_len = sizeof (client_address);
		client_sockfd = accept (server_sockfd,
				(struct sockaddr *) &client_address,
				&client_len);

		/*  Fork to create a process for this client */

		if (fork () == 0)
		{
			double number;
			time_t mytime = time(NULL);
			char * time_str = ctime(&mytime);
			time_str[strlen(time_str)-1] = '\0';
			int length = strlen(time_str);

			read (client_sockfd, &number, 8);
			number = sqrt(number);
			write (client_sockfd, &number, 8);
			
			write(client_sockfd, &length, 4);
			write(client_sockfd, time_str, length);
			close (client_sockfd);
			exit (0);
		}

		/*  Otherwise, we must be the parent and our work for this client is finished.  */

		else
		{
			close (client_sockfd);
		}
	}
}
