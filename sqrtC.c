#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int isBigEndian() 
{
	int x = 1;
	//1000 0000 - littleEndian
	//0000 0001 - bigEndian
	if((int) (((char *)&x)[0]) == 1)
		return 0; //littleEndian
	else
		return 1; //bigEndian
}

int main()
{
	srand(time(0)); //inicjalizacja ziarna dla random()
	int sockfd; //deskryptor polaczenia
	socklen_t len; //rozmiar struktury; potrzebne do polaczenia
	struct sockaddr_in address;

	int result; //rezultat polaczenia; 0 - polacznie udane, -1 - polacznie nieudane
	double liczba; //pierw przechowuje liczbe DO pierwiastkowania, pozniej PO pierwiastkowaniu
	int length; //dlugosc stringa daty i czasu; potrzebne do zaalokowania pamieci

	char str[32]; //tymczasowy buffor dla liczby w formacie stringa; potrzebne dla wyznaczenia pozycji korpki
	char *whereIsTheDot;
	
	int rqId;
	int rqIdReturnedByServer;
	int status; //walidacja wprowadzonej liczby
	char *buffer = NULL; //buffor dla wprowadzonego tekstu; potrzebne do sscanf
	size_t n = 0; //wielkosc przechwyconego inputa, to samo co unsigned int
	char choose = 'T';
	printf("You're %s-Endian\n", isBigEndian() ? "Big" : "Little");
	
	/*  Create a socket for the client.  */

	sockfd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM - TCP

	/*  Name the socket, as agreed with the server.  */

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1"); //AF_INET - IPv4
	address.sin_port = htons(9734); //host to network short
	len = sizeof(address);

	/*  Now connect our socket to the server's socket.  */

	result = connect(sockfd, (struct sockaddr *) &address, len);

	if(result == -1)
	{
		perror("oops: netclient");
		exit (1);
	}

	/*  We can now read/write via sockfd.  */
	
	do
	{
		rqId = 1 + rand() % 10000000;
		
		do
		{
			getline(&buffer, &n, stdin);
	   		status = sscanf(buffer, "%lf", &liczba);
		}
		while(liczba < 0 || status != 1);

		snprintf(str, sizeof(str), "%lf", liczba);
		whereIsTheDot = strchr(str, '.');
		printf("Dot position: %ld\n", whereIsTheDot-str);

		write(sockfd, &rqId, 4);
		write(sockfd, &liczba, 8);

		read(sockfd, &rqIdReturnedByServer, 4);
		printf("Sent ID: %d, Returned ID: %d\n", rqId, rqIdReturnedByServer);

		read(sockfd, &liczba, 8); 
		read(sockfd, &length, 4); 

		char * data = (char*) malloc (length * sizeof(char));
		//char data[length+1];

		read (sockfd, data, length);
		data[length] = '\0';

	 	printf("Result: %f\n", liczba);
		printf("Date: %s\n", data);
		printf("Do you want to continue?? [Y/N]: ");
		scanf(" %c", &choose);
		write(sockfd, &choose, 1);
	}while(choose != 'N');

	close (sockfd);
	exit (0);
}
