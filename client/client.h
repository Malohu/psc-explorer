#ifndef CLIENT_H
#define CLIENT_H

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

#include "telemeter.h"
#include "motor.h"

#define MAX_SERVERS 10

class client
{
	public :
	
	bool working, connected;
	
	struct sockaddr_in serv_addr;
	socklen_t clientlen;
	struct hostent* server;
	int sock;
	unsigned server_port;
	long id_client;
	
	PointCloud main_cloud;
	char server_path[200];
	char server_ip[MAX_SERVERS][20];
	char telemeter_device[200];
	long last_file;
	Position position, next_position;
	Telemeter tel;
	Motor motor;
	
	double default_max_world_size;
	double default_epsilon;
	long sleep_server_busy;
	double delay_step_acquisition, nb_points_acquisition, step_acquisition;
	double offset_phi;
	
	
		//client();  // cree un nouveau modele
		client(char* config_file);  // charge un ancien modele
		int connect();  // se connecte
		int mainLoop();  // boucle principale des actions
		//int disconnect();
};

#endif
