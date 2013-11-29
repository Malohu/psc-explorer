#ifndef DISPLAY_CLIENT_H
#define DISPLAY_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "../commons/pointcloud.h"
#include "../commons/geometry.h"
#include "../commons/instruction.h"
#include "../commons/error.h"

#define NETWORK 1
#define LOCAL 2

#define MAX_SERVERS 10

class DisplayClient
{
	public :
	
	bool connected;
	int* display_lists;
	int max_display_lists, index_display_lists;
	long nb_new_points;
	long nb_loops;
	int mode;
	
	struct sockaddr_in serv_addr;
	socklen_t clientlen;
	struct hostent* server;
	int sock;
	unsigned server_port;
	long id_client;
	
	char server_path[200];
	char server_ip[MAX_SERVERS][20];
	char files_path[200];
	
		//client();  // cree un nouveau modele
		DisplayClient(char* config_file, int Mode, char* Files_path, long ID_client);  // charge un ancien modele
		int loadConfig(char* config_file);
		int connect();  // se connecte
		int mainLoop();  // boucle principale des actions
};

int createDisplayList(Point* points, int nb_points, int list, unsigned short r_color, unsigned short g_color, unsigned short b_color);

#endif
