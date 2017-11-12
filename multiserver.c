/* The project was prepared by Minita Dabhi, Anson Jacob and Niyaz Murshed as part of coursework ECE 650 -'Tools & Mothods for Software Engineering' at University of Waterloo.
 multiserver.c contains the code for multithreaded server side in project 'Multithreaded server - client mapping system. Mutex is used to protect the 
 Graphinfo file from being accessed by multiple threads*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "roadmap.h"

//#define DEBUG

#define SERVER_PORT         (8888)
#define MAX_PACKET_SIZE     (5*1024)
#define MAX_HANDSHAKE       (5)
#define WELCOME_MSG         "Connected to University of Waterloo Mapper "
#define SERVER_PASSWORD     "1"
#define MASTER_NODE         (1)
#define CLIENT_NODE         (0)

enum commands {
	HANDSHAKE,
	ADD_VERTEX = 2,
	ADD_EDGE,
	ADD_ROAD,
	STORE,
	RETREIVE,
	GET_TRIP,
	GET_VERTEX,
	EDGE_EVENT = 10,
	MAX_COMMAND
};

int quit = 0;
pthread_mutex_t mutex_mapper;

void *connection_handler(void *socket_desc)
{
	int err = 0;
	int i = 0;
	int pos = 0;
	int once = 1;
	int count = 0;
	int sock = *(int*)socket_desc;
	int type = 0;
	int command = 0;
	int size = 0;
	char reply = 0;
	char filled = 0;
	char *client_message = NULL;
	char *holder = NULL;
	char *p0 = NULL;
	char *p1 = NULL;
	char *p2 = NULL;
	char *p3 = NULL;
	char *p4 = NULL;
	char *p5 = NULL;
	char *temp = NULL;

	client_message = calloc(MAX_PACKET_SIZE, 1);
	if(client_message == NULL) {
		printf("ERROR: malloc failed\n");
		return NULL;
	}

	while(!quit) {
		int done = 0;

		if(once) {
			strcpy(client_message, WELCOME_MSG);
			err = write(sock, client_message, strlen(client_message));
		} else {
			err = 0;
		}

		if(err == -1) {
			printf("ERROR: Client handshake failed 1\n");
		} else {
			memset(client_message, 0, MAX_PACKET_SIZE);
			err = recv(sock, client_message, MAX_PACKET_SIZE, 0);
			if(err < 1) {
				printf("ERROR: Client handshake failed 2\n");
			} else {

#ifdef DEBUG
				{
					int j = 0;
					printf("Recv Client:");
					for(j = 0; j < err; j++) {
						printf("%c", client_message[j]);
					}
					printf("\n");
				}
#endif
				p0 = strtok_r(client_message, "|", &holder);
				p1 = strtok_r(NULL, "|", &holder);
				p2 = strtok_r(NULL, "|", &holder);

				command = MAX_COMMAND;
				if(p0 != NULL) {
					command = atoi(p0);
				}

				// check for password if master node
				if(command == HANDSHAKE) {
					int node = 0;

					if(p1 != NULL) {
						node = atoi(p1);
						//printf("Type: %d Type %d\n", node, command);
					}

					switch(node) {
						case MASTER_NODE:
							if(p2 != NULL) {
								if(strncmp(p2, SERVER_PASSWORD, sizeof(SERVER_PASSWORD)) != 0) {
									printf("ERROR: Password Wrong\n");
								} else {
									type = 1;
									reply = 1;
									done = 1;
								}
							} else {
								printf("ERROR: password place holder error\n");
							}
							break;

						case CLIENT_NODE:
							reply = 1;
							done = 1;
							break;

						default:
							printf("ERROR: Unknown node\n");
							break;
					}
				} else {
					printf("Handshake not completed, Command %d rejected\n", command);
				}

				memset(client_message, 0, MAX_PACKET_SIZE);
				client_message[0] = (reply == 1) ? '1' : '0';
				size = strlen(client_message);
#ifdef DEBUG
				{
					int j = 0;
					printf("Sending to Client:");
					for(j = 0; j < size; j++) {
						printf("%c", client_message[j]);
					}
					printf("\n");
				}
#endif
				err = write(sock, client_message, size);
				if(err == -1) {
					printf("ERROR: Client handshake reply failed\n");
					done = 0;
				}

				if(done)
					goto start;
			}
		}

		if(count > MAX_HANDSHAKE) {
			printf("ERROR: Client failed handshake 3\n");
			goto free_mem;
		}
		count++;
	}

start:
	once = 0;
	count = 0;

	while(!quit) {

		if(count > 5) {
			printf("ERROR: Client communication is not stable\n");
			goto free_mem;
		}

		memset(client_message, 0, MAX_PACKET_SIZE);
		err = recv(sock, client_message, MAX_PACKET_SIZE, 0);
		if(err < 1) {
			count++;
			continue;
		}

#ifdef DEBUG
		{
			int j = 0;
			printf("Recv Client:");
			for(j = 0; j < err; j++) {
				printf("%c", client_message[j]);
			}
			printf("\n");
		}
#endif
		count = 0;
		reply = 0;
		filled = 0;
		p0 = strtok_r(client_message, "|", &holder);
		p1 = strtok_r(NULL, "|", &holder);
		p2 = strtok_r(NULL, "|", &holder);

		command = MAX_COMMAND;
		if(p0 != NULL) {
			command = atoi(p0);
			//printf("Type: %d Command %d\n", type, command);
		}

		if(type == 1) {
			switch(command) {
				case ADD_VERTEX:
					pthread_mutex_lock(&mutex_mapper);
					err = addVertex(atoi(p1), p2);
					pthread_mutex_unlock(&mutex_mapper);
					if(err != -1)
						reply = 1;
					break;

				case ADD_EDGE:
					p3 = strtok_r(NULL, "|", &holder);
					p4 = strtok_r(NULL, "|", &holder);
					p5 = strtok_r(NULL, "|", &holder);

					if(p3 != NULL && p4 != NULL && p5 != NULL) {
						int dir = atoi(p3);
						double speed = atof(p4);
						double length = atof(p5);
						pthread_mutex_lock(&mutex_mapper);
						err = addEdge(p1, p2, dir, speed, length);
						pthread_mutex_unlock(&mutex_mapper);
						if(err == 0)
							reply = 1;
					}
					break;

				case EDGE_EVENT:
					pthread_mutex_lock(&mutex_mapper);
					err = edgeEvent(p1, atoi(p2));
					pthread_mutex_unlock(&mutex_mapper);
					if(err == 0)
						reply = 1;
					break;

			   case ADD_ROAD:
					{
						char edges[5][EDGE_NAME_SIZE];
						printf("here\n");
						count = atoi(p1);
						pos = 0;
						for(i = 0; i < count; i++) {
							strncpy(edges[i], p2, EDGE_NAME_SIZE);
							p2 = strtok_r(NULL, "|", &holder);

						}

						memset(client_message, 0, MAX_PACKET_SIZE);
						if(i != count || p2 == NULL) {
							strcpy(client_message, "Wrong Input");
							size = strlen(client_message);
							filled = 1;
							break;
						}

						reply = 1;
						pthread_mutex_lock(&mutex_mapper);
						for(i = 0; i < count; i++) {
							err = edgeAddRoad(edges[i], p2);
							if(err != 0) {
								reply = 0;
								filled = 1;
							}
						}
						pthread_mutex_unlock(&mutex_mapper);
					}

					break;

				case GET_TRIP:
					{
						char add1[VERTEX_NAME_SIZE], add2[VERTEX_NAME_SIZE];
						pthread_mutex_lock(&mutex_mapper);
						strcpy(add1, vertex(p1));
						strcpy(add2, vertex(p2));

						printf("Here %s:%s\n", add1, add2);
						size = MAX_PACKET_SIZE;
						memset(client_message, 0, MAX_PACKET_SIZE);
						err = trip(add1, add2, client_message, &size);
						pthread_mutex_unlock(&mutex_mapper);
						if(err >= 0 && size > 0) {
							printf("Size: %d\n", size);
							printf("%s\n", client_message);
							reply = 1;
							filled = 1;
						}
					}
					break;

				case GET_VERTEX:
					pthread_mutex_lock(&mutex_mapper);
					temp = vertex(p1);
					pthread_mutex_unlock(&mutex_mapper);
					if(temp != NULL) {
						memset(client_message, 0, MAX_PACKET_SIZE);
						strncpy(client_message, temp, VERTEX_NAME_SIZE);
						size = strlen(client_message);
						reply = 1;
						filled = 1;
					}
					break;

				case STORE:
					pthread_mutex_lock(&mutex_mapper);
					if(store(p1) == 0)
						reply = 1;
					pthread_mutex_unlock(&mutex_mapper);
					break;

				case RETREIVE:
					pthread_mutex_lock(&mutex_mapper);
					if(retrieve(p1) == 0)
						reply = 1;
					pthread_mutex_unlock(&mutex_mapper);
					break;

				default:
					printf("Unknown request\n");
					break;
			}
		} else {
			switch(command) {
				case GET_TRIP:
					{
						char add1[30], add2[30];
						pthread_mutex_lock(&mutex_mapper);
						strcpy(add1, vertex(p1));
						strcpy(add2, vertex(p2));

						printf("Here %s:%s\n", add1, add2);
						size = MAX_PACKET_SIZE;
						memset(client_message, 0, MAX_PACKET_SIZE);
						err = trip(add1, add2, client_message, &size);
						pthread_mutex_unlock(&mutex_mapper);
						if(err >= 0 && size > 0) {
							printf("Size: %d\n", size);
							printf("%s\n", client_message);
							reply = 1;
							filled = 1;
						}
					}
					break;

				case GET_VERTEX:
                {
                    char tempv[30];
					pthread_mutex_lock(&mutex_mapper);
					temp = vertex(p1);
                    strcpy(tempv, vertex(p1));
					pthread_mutex_unlock(&mutex_mapper);
					if(temp != NULL) {
                        memset(client_message, 0, MAX_PACKET_SIZE);
						strncpy(client_message, temp, VERTEX_NAME_SIZE);
						size = strlen(client_message);
						reply = 1;
						filled = 0;
					}
                }
					break;
				default:
					printf("Unknown request\n");
					break;
			}
		}

		if(!filled) {
			memset(client_message, 0, MAX_PACKET_SIZE);
			client_message[0] = (reply == 1) ? '1' : '0';
			size = strlen(client_message);
		}
//#ifdef DEBUG
		{
			int j = 0;
			printf("Sending to Client:");
			for(j = 0; j < size; j++) {
				printf("%c", client_message[j]);
			}
			printf("\n");
		}
//#endif
		err = write(sock, client_message, size);
		if(err == -1) {
			printf("ERROR: Client reply failed\n");
			count++;
		}
	}

free_mem:
	if(client_message != NULL)
		free(client_message);

	if(socket_desc != NULL)
		free(socket_desc);

	printf("Closing Client\n");

	return 0;
}

int main(int argc , char *argv[])
{
	int err = 0;
	int portno = SERVER_PORT;
	int socket_desc = 0;
	int client_sock = 0;
	int c = 0;
	int *new_sock = NULL;
	struct sockaddr_in server;
	struct sockaddr_in client;

	if(argc == 2) {
		portno = atoi(argv[1]);
	}

	printf("Mapper Running at Port No: %d\n Waiting for Client \n", portno);

	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portno);

	// retrive the map from the file
	retrieve(FILENAME);

	// Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1)
	{
		printf("Could not create socket\n");
		return -1;
	}

	// Bind
	if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("ERROR: Binding on port %d failed", portno);
		return -1;
	}

	// Listen
	err = listen(socket_desc, 5);
	if(err != 0) {
		printf("ERROR: Listen failed\n");
		return -1;
	}

	err = pthread_mutex_init(&mutex_mapper, NULL);
	if(err == -1) {
		printf("Mutex creation failed\n");
	}

	c = sizeof(struct sockaddr_in);

	while(!quit) {
		// Accept incoming connection
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if(client_sock < 0) {
			printf("ERROR: Connection failed\n");
			continue;
		}

		//printf("Connection accepted from %d:%d\n",
		//		client.sin_addr.s_addr, htons(client.sin_port));

		pthread_t sniffer_thread = 0;
		new_sock = calloc(sizeof(new_sock), 1);
		*new_sock = client_sock;

		err = pthread_create(&sniffer_thread, NULL, connection_handler, (void*)new_sock);
		if(err != 0) {
			printf("ERROR: Thread creation failed\n");
			close(client_sock);
			continue;
		}
	}
	return 0;
}
