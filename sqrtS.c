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

int main()
{
	int server_sockfd, client_sockfd;
	socklen_t server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET - IPv4

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); //bindujemy WSZYSTKIE interfejsy, nie tylko np localhost
	server_address.sin_port = htons(9734);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *) &server_address, server_len); //powiazanie socketa z deskryptorem, domyslnie gniazdo jest anonimowe

	/*  Create a connection queue, ignore child exit details and wait for clients.  */

	listen(server_sockfd, 5); //5 - dlugosc kolejki w oczekiwaniu na sockfd

	signal(SIGCHLD, SIG_IGN); //pozwala zamknac sie procesom zombie 

	while (1)
	{
		
		printf("Server waiting for new clients\n");
		
		/*  Accept connection.  */

		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
		printf("Client connected\n");
		fflush(stdout);

		/*  Fork to create a process for this client */

		if(fork() == 0)
		{
			double liczba;
			int rqId;
			char choose;
			time_t mytime;
			char * time_str;
			
			do
			{
				mytime = time(NULL);
				time_str = ctime(&mytime);
				//time_str[strlen(time_str)-1] = '\0';
				int length = strlen(time_str);

				read(client_sockfd, &rqId, 4);
				read(client_sockfd, &liczba, 8);

				printf("SQRT of %lf is %lf. RequestId: %d\n", liczba, sqrt(liczba), rqId);
				fflush(stdout);

				liczba = sqrt(liczba);
				write(client_sockfd, &rqId, 4);
				write(client_sockfd, &liczba, 8);
				write(client_sockfd, &length, 4);
				write(client_sockfd, time_str, length-1);

				read(client_sockfd, &choose, 1);
			}while(choose != 'N');
			close(client_sockfd);
			printf("Client disconnected\n");
			fflush(stdout);
			exit(0);
		}

		/*  Otherwise, we must be the parent and our work for this client is finished.  */

		else
		{
			close(client_sockfd);
		}
	}
}