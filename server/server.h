#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../commons/pointcloud.h"
#include "../commons/geometry.h"
#include "../commons/instruction.h"

#define MAX_CLIENTS 10

class client
{
	public :
	
	bool used;
	
	struct sockaddr_in client_addr;
	socklen_t clientlen;
	int sock;
	long id_client;
	
	PointCloud main_cloud;
	Position position;
	char server_path[200];
	long last_file;
	
	Position next_position;
	int action;
	double next_telemeter_move;
	
	// variables du thread de traitement de nouveaux points
	bool processing;  // true si le serveur possède un thread de traitement relatif à ce client en cours d'exécution
	long nb_new_points;
	Point* new_points;
	Position position_new_points;
	
};

class server
{
	public :
	
	char config_file[200], clients_data[200];
	unsigned listening_port;
	int nb_clients, max_clients;
	int listening_socket, client_socket;
	struct sockaddr_in serv_addr, cli_addr;  // addresse du serveur et du client
	socklen_t clientlen;
	bool is_listening, is_online;
	
	client clients[MAX_CLIENTS];  // les clients
	
	long sample_alignment;
	long nb_closest_points_alignment;
	long max_iterations;
	double step_translation;
	double alignment_accuracy;
	double default_max_world_size;
	double default_epsilon;
	double threshold_mutual_distance_filter;
	long nb_closest_points_mutual_distance_filter;
	double threshold_deleted_points;
	long nb_spheres_alignment;
	long nb_used_spheres_alignment;
	double radius_spheres_alignment;
	
	int mutual_distance_filter_enabled;
	
		server(char* Config_file);
		int loadConfig();  // charge la config
		int startListening();  // lance le thread d'ecoute (et le socket au passage)
		int stopListening();
		int closeAllConnexions();
		int listeningThread();  // fonction du thread d'ecoute
		int clientThread();  // fonction du thread client
		int processingThread();  // thread de traitement des nouvelles données envoyées par le client
		int interface();
		
		
};

#endif
