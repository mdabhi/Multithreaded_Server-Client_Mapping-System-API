/* The project was prepared by Minita Dabhi, Anson Jacob and Niyaz Murshed as part of coursework ECE 650 -'Tools & Mothods for Software Engineering'.
 clients.c contains the code for client side in project 'Multithreaded server - client mapping system.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFERSIZE	(5*1024)
#define IPADD		"127.0.0.1"
#define PORTNO		(8888)

int option1 = 0;
int option2 = 0;

int main(int argc, char* argv[])
{
	int sock;
	int size = 0, i;
	struct sockaddr_in server;
	char message[1000], server_reply[BUFFERSIZE];
	char *ip = IPADD;
	int portno = PORTNO;
	char *p0 = NULL;
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;

	if(argc == 3) {
		if(strcmp(argv[1], "localhost") == 0)
			ip = IPADD;
		else
			ip = argv[1];

		portno = atoi(argv[2]);
	}

	printf("Connecting to %s:%d : Toronto TTC \n", ip, portno);

	// Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Could not create socket\n");
		return -1;
	}

	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);

	// Connect to remote server
	if(connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Connect failed. Error\n");
		return -1;
	}

	if(recv(sock, server_reply, BUFFERSIZE, 0) < 0) {
		printf("recv failed\n");
	}

	// printf("Server reply : %s\n", server_reply);

	memset(server_reply, 0, sizeof(server_reply));

	// keep communicating with server
	while(1) {
		printf("Please choose user type : \n");
		printf("1. Admin\n");
		printf("2. User\n");
		scanf("%d", &option1);
		int level =0, sublevel = 0, temp = 0;
		char tempchar[20], tempchar1[20];

		if(option1 == 1) {
			level = 0;
			sublevel = 1;
		} else {
			sublevel = 2;
		}

		switch (option1) {
			case 1:
				while(level==0) {
					printf("Enter Admin Password : ");
					strcat(message, "0|1|");
					scanf("%s", &message[4]);
					if(send(sock, message, strlen(message), 0) < 0) {
						printf("\nSend failed\n");
						return 1;
					}

					size = recv(sock, server_reply, BUFFERSIZE, 0);
					if (strcmp(server_reply, "1") != 0) {
						printf("\nPassword Incorrect\n");

					} else {
						level = 1;
					}
				}

				while(sublevel == 1) {
					memset(message, 0, sizeof(message));
					printf("\nAvailable Options : \n");
					printf("1. Add Vertex\n");
					printf("2. Add Edge\n");
					printf("3. Trip\n");
					printf("4. Store\n");
					printf("5. Retrieve\n");
					printf("6. Edge Event\n");
					printf("7. Add Road Label\n");
					printf("Option : ");

					scanf("%d", &option2);
					switch(option2) {
						case 1:
							printf("\nChoose Vertex type\n 1: POI\n 2: Intersection\n");
							printf("Type : ");
							strcat(message,"2|");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter Vertex Name : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							if(send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}

							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1") != 0) {
									printf("Vertex Not Added\n");
								} else {
									printf("Vertex Added Successfully\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						case 2:
							printf("Enter Vertex1 : ");
							strcat(message, "3|");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter Vertex2 : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter direction of edge (Bidirection - 1, Unidirection - 0) : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter speed : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter Distance : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							if(send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}
							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1") != 0) {
									printf("Edge Not Added, add vertexes fisrt\n");
								} else {
									printf("Edge Added Successfully\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}

							memset(message, 0, sizeof(message));
							continue;

						case 3:
							strcat(message, "7|");
							printf("Enter Starting Point : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter Destination Point : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							if( send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}

							//Receive a reply from the server
							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);

							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								p0 = strtok(server_reply, "|");
								temp = atoi(p0);

								if(temp == 0) {
									printf("Please enter different vertices : ");
								} else {
									for(i = 1; i <= atoi(p0); i++) {
										p1 = strtok(NULL, "|");
										p2 = strtok(NULL, "|");
										p3 = strtok(NULL, "|");
										printf("\nTake the road %s ", p1);
										printf("at Max speed %s ", p2);
										printf("in %skm", p3);
									}
									printf("\nDestination reached..\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						case 4:
							printf("Enter Filename to store info : ");
							strcat(message, "5|");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							if(send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}

							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1")!=0){
									printf("File note saved\n");
								} else{
									printf("File saved Successfully\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						case 5:
							printf("Enter File to be retrieved : ");
							strcat(message, "6|");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							if( send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}
							size = recv(sock, server_reply, BUFFERSIZE, 0);
							//printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1") != 0) {
									printf("File not retrieved\n");
								} else{
									printf("File retrieved Successfully\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						case 6:
							printf("Enter the edge name : ");
							strcat(message, "10|");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter the event for the edge 1: Normal 2: Road Block 3: Accident : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							if( send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}
							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1") != 0) {
									printf("Edge Event not added \n");
								} else {
									printf("Edge Event added succesfully\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						case 7:
							printf("Enter the # of edges(Max-5) : ");
							strcat(message, "4|");
							scanf("%s", tempchar1);
							strcat(message, tempchar1);
							strcat(message, "|");
							for(i = 0; i < atoi(tempchar1); i++) {
								printf("Enter the edge : ");
								scanf("%s", tempchar);
								strcat(message, tempchar);
								strcat(message, "|");
							}
							printf("Enter the road name : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							if( send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}
							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1") != 0) {
									printf("Edges not found\n");
								} else{
									printf("Edges are labelled with road name succesfully!!!\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						default:
							printf("\n Admin Error..\n");
							break;
					}
				}
				continue;

			case 2:

				strcat(message, "0|0|");
				if(send(sock,  message, strlen(message), 0) < 0) {
					printf("Send failed\n");
					return 1;
				}
				size = recv(sock, server_reply, BUFFERSIZE, 0);
				if (strcmp(server_reply, "1") != 0) {
					sublevel=0;
				}

				while (sublevel == 2) {
					memset(message, 0, sizeof(message));
					printf("Please Enter your choices : \n");
					printf("1. Search Route\n");
					printf("2. Search Vertex\n");

					scanf("%d", &option2);

					switch (option2) {
						case 1:
							strcat(message, "7|");
							printf("Enter Starting Point : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							strcat(message, "|");
							printf("Enter Destination Point : ");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							if(send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}

							//Receive a reply from the server
							size = recv(sock, server_reply, BUFFERSIZE, 0);
							// printf(" SIZE %d\n", size);
							if(size < 0) {
								printf("recv failed\n");
								break;
							} else {
								p0 = strtok(server_reply, "|");
								temp = atoi(p0);

								if(temp == 0) {
									printf("Please enter different vertices : ");
								} else {
									for(i = 1;i <= atoi(p0); i++) {
										p1 = strtok(NULL, "|");
										p2 = strtok(NULL, "|");
										p3 = strtok(NULL, "|");
										printf("\nTake the road %s ",p1);
										printf("at Max speed %s ", p2);
										printf("in %skm", p3);
									}
									printf("\nDestination reached..\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						case 2:
							memset(message, 0, sizeof(message));
							printf("Enter Point of Interest or Intersection : ");
							strcat(message, "8|");
							scanf("%s", tempchar);
							strcat(message, tempchar);
							if( send(sock, message, strlen(message), 0) < 0) {
								printf("Send failed\n");
								return 1;
							}

							// Receive a reply from the server
							size = recv(sock, server_reply, BUFFERSIZE, 0);

							if(size < 0){
								printf("recv failed\n");
								break;
							} else {
								if(strcmp(server_reply, "1")!=0){
									printf("Vertex Not Found\n");
								}
								else{
									printf("Vertex Found\n");
								}
								memset(server_reply, 0, sizeof(server_reply));
							}
							memset(message, 0, sizeof(message));
							continue;

						default:
							break;
					}
				}
				continue;

			default:
				printf("\n User Error..\n");
				break;
		}
	}
	close(sock);
	return 0;
}

