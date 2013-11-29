#include "server.h"
#include "../commons/benchmark.h"

server* p_server;
int last_started_client;
int last_processing_client;

// lanceur du thread d'ecoute
void* listening_thread(void* p_data)
{
	p_server -> listeningThread();
	return 0;
}

// lanceur du thread client
void* client_thread(void* p_data)
{
	int c = last_started_client;
	p_server -> clientThread();
	if  (!p_server -> clients[c].processing) p_server -> clients[c].main_cloud.deleteCloud();
	p_server -> nb_clients--;
	p_server -> clients[c].used = false;
	return 0;
}

// lanceur du thread de traitement
void* processing_thread(void* p_data)
{
	int c = last_processing_client;
	p_server -> processingThread();
	if  (!p_server -> clients[c].used) p_server -> clients[c].main_cloud.deleteCloud();
	p_server -> clients[c].processing = false;
	return 0;
}

// crée le serveur
server::server(char* Config_file)
{
	strcpy(config_file, Config_file);
	nb_clients = 0;
	for (int i = 0 ; i < MAX_CLIENTS ; i++)
	{
		clients[i].used = false;
		clients[i].processing = false;
	}
	p_server = this;
	is_listening = false;
	is_online = false;
	
	info("server initialized");
}

// charge la config
int server::loadConfig()
{
	// pour l'instant tout est hardcodé
	strcpy(clients_data, "../../data");
	
	char cmd[500];
	strcpy(cmd, "mkdir ");
	strcat(cmd, clients_data);
	system(cmd);
	
	max_clients = min(10, MAX_CLIENTS);
	listening_port = 18997;
	
	sample_alignment = 500;
	nb_closest_points_alignment = 1;
	max_iterations = 100;
	step_translation = 0.001;
	alignment_accuracy = 0.001;
	default_max_world_size = 100.0;
	default_epsilon = 0.01;
	threshold_mutual_distance_filter = 0.05;
	nb_closest_points_mutual_distance_filter = 5;
	threshold_deleted_points = 1;
	nb_spheres_alignment = 1;
	nb_used_spheres_alignment = 1;
	radius_spheres_alignment = 200;
	
	mutual_distance_filter_enabled = 0;
	
	info("config loaded");
	
	return 0;
}

// demarre le socket puis le thread d'ecoute
int server::startListening()
{
	listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if  (listening_socket < 0)
	{
		error("could not create listening socket");
		return 1;
	}
	
	//bzero((char *) &serv_addr, sizeof(serv_addr));
     	serv_addr.sin_family = AF_INET;
     	serv_addr.sin_addr.s_addr = INADDR_ANY;
     	serv_addr.sin_port = htons(listening_port);
     	if (bind(listening_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
     	{
     		error("could not bind listening socket");
     		return 2;
     	}
     	
     	listen(listening_socket, 5);
     	
     	pthread_t listening_thread_ID;
     	pthread_create(&listening_thread_ID, NULL, listening_thread, NULL);
     	
     	return 0;
}

// arrete l'écoute
int server::stopListening()
{
	close(listening_socket);
	is_listening = false;
	return 0;
}

// ferme toutes les connexions
int server::closeAllConnexions()
{
	is_online = false;
	return 0;
}

// le thread d'écoute
int server::listeningThread()
{
	info("Successfuly started server listening");
	is_listening = true;
	is_online = true;
	Message* w = new Message();
	while (is_listening)
	{
		// accepte une connexion
		clientlen = sizeof(cli_addr);
     		client_socket = accept(listening_socket, (struct sockaddr *) &cli_addr, &clientlen);
     		if (client_socket < 0)
     		{
     			error("error on accept");
     			continue;
     		}
     		
     		// recoit le message d'identification du client
     		Message welcome = *w;
     		int n = welcome.recv(client_socket);
     		if  (n != 0)
     		{
     			error("error while receiving client identification");
     			printf("%d\n", n);
     			close(client_socket);
     			continue;
     		}
     		
     		// traite le message
     		if  (welcome.nb_param != 4 || welcome.return_nb_param != 0)
     		{
     			error("invalid client identification message");
     			close(client_socket);
     			continue;
     		}
     		long id_client = ((unsigned char)welcome.param[0][0]) * 0x1000000 + ((unsigned char)welcome.param[0][1]) * 0x10000 + ((unsigned char)welcome.param[0][2]) * 0x100 + ((unsigned char)welcome.param[0][3]);
     		char server_path[welcome.param_size[1]];
     		strcpy(server_path, welcome.param[1]);
     		long last_file = ((unsigned char)welcome.param[2][0]) * 0x1000000 + ((unsigned char)welcome.param[2][1]) * 0x10000 + ((unsigned char)welcome.param[2][2]) * 0x100 + ((unsigned char)welcome.param[2][3]);
     		Position* last_position = new Position();
     		*last_position = *((Position*)welcome.param[3]);
     		
     		// cherche une structure client libre
     		int i;
     		for (i = 0 ; i < max_clients ; i++)
     		{
     			if  (!clients[i].used && !clients[i].processing) break;
     		}
     		if  (i == max_clients)
     		{
     			error("too many clients connected, cannot accept more connexions");
     			close(client_socket);
     			continue;
     		}
     		
     		// recopie les donnees recues dans la structure client
     		clients[i].used = true;
     		clients[i].processing = false;
     		clients[i].client_addr = cli_addr;
     		clients[i].clientlen = clientlen;
     		clients[i].sock = client_socket;
     		clients[i].id_client = id_client;
     		clients[i].last_file = last_file;
     		strcpy(clients[i].server_path, server_path);
     		PointCloud* pc = new PointCloud(default_epsilon, default_max_world_size);
     		clients[i].main_cloud = *pc;
     		delete pc;
     		clients[i].position = *last_position;
     		clients[i].action = 0;
     		nb_clients++;
     		
     		// lance le thread client
     		last_started_client = i;
     		pthread_t client_thread_ID;
     		pthread_create(&client_thread_ID, NULL, client_thread, NULL);
     		
     		printf("New client connected : %d\n", i);
	}
	
	delete w;
	close(listening_socket);
	info("Stopped server listening\n");
	
	return 0;
}

// le thread client
int server::clientThread()
{
	int c = last_started_client;  // numéro du client géré ici
	Message* me = new Message();
	
	/* code pour charger d'eventuels fichiers de donnees et pour reconstruire l'arbre existant */
	char tmp_buf[200];
	strcpy(tmp_buf, clients_data);
	strcat(tmp_buf, "/");
	strcat(tmp_buf, clients[c].server_path);
	strcpy(clients[c].server_path, tmp_buf);
	
	// cree le dossier client sur le serveur
	char cmd[500];
	strcpy(cmd, "mkdir ");
	strcat(cmd, clients[c].server_path);
	system(cmd);
	
	printf("Info : Loading %ld data files\n", clients[c].last_file);
	
	for (long i = 0 ; i < clients[c].last_file ; i++)
	{
		char char_file_name[20], file_path[200];
		longToChar(i, char_file_name);
		strcpy(file_path, clients[c].server_path);
		strcat(file_path, "/");
		strcat(file_path, char_file_name);
		strcat(file_path, ".txt");
		PointCloud* old_cloud = new PointCloud(file_path, default_epsilon, default_max_world_size);
		clients[c].main_cloud.mergeCloud(old_cloud);
		delete old_cloud;
	}
	
	// boucle principale de gestion du client
	while (is_online)
	{
		Message m = *me;
		if  (m.recv(clients[c].sock) != 0)
		{
			error("error while receiving instruction from client (1)");
			close(clients[c].sock);
			return 1;
		}
		
		if  (m.nb_param < 0 || m.nb_param > MAX_PARAM || m.return_nb_param != 0)
		{
			error("incorrect number of parameters from client (2)");
			close(clients[c].sock);
			return 2;
		}
		
		switch (m.instruction)
		{
			case 2 :  // requete de position
			{
				if  (m.nb_param != 0 || m.return_nb_param != 0)
				{
					error("incorrect number of parameters from client (4)");
					close(clients[c].sock);
					return 4;
				}
				m.return_value = 0;
				m.return_nb_param = 1;
				m.return_param = new char*[1];
				m.return_param[0] = new char[sizeof(Position)+1];
				*((Position*)(m.return_param[0])) = clients[c].position;
				m.return_param_size[0] = sizeof(Position) + 1;
				
				if  (m.send(clients[c].sock) != 0)
				{
					error("error while sending back reply to client (5)");
					close(clients[c].sock);
					return 5;
				}
				
				m.deleteMessage();
				
				break;
			}
			
			case 3 :  // requete d'action
			{
				if  (m.nb_param != 0 || m.return_nb_param != 0)
				{
					error("incorrect number of parameters from client (6)");
					close(clients[c].sock);
					return 6;
				}
				m.return_value = clients[c].action;
				clients[c].action = 0;
				
				if  (m.return_value == 2)  // si deplacement
				{
					m.return_nb_param = 1;
					m.return_param = new char*[1];
					m.return_param[0] = new char[sizeof(Position)+1];
					*((Position*)(m.return_param[0])) = clients[c].next_position;
					m.return_param_size[0] = sizeof(Position) + 1;
					clients[c].position = clients[c].next_position;
				}
				
				if  (m.return_value == 4)  // si deplacement du telemetre
				{
					m.return_nb_param = 1;
					m.return_param = new char*[1];
					m.return_param[0] = new char[sizeof(double)+1];
					*((double*)(m.return_param[0])) = clients[c].next_telemeter_move;
					m.return_param_size[0] = sizeof(double)+1;
				}
				
				if  (m.send(clients[c].sock) != 0)
				{
					error("error while sending back reply to client (7)");
					close(clients[c].sock);
					return 7;
				}
				
				m.deleteMessage();
				
				break;
			}
			
			case 4 :  // transfert de points
			{
				if  (m.nb_param != 3 || m.return_nb_param != 0)
				{
					error("incorrect number of parameters from client (8)");
					close(clients[c].sock);
					return 8;
				}
				if  (clients[c].processing)
				{
					m.deleteMessage();
					m.instruction = 4;
					m.nb_param = 0;
					m.return_nb_param = 0;
					m.return_value = 1;
					
					if  (m.send(clients[c].sock) != 0)
					{
						error("error while sending back reply to client (11)");
						close(clients[c].sock);
						return 11;
					}
					
					error("server already processing data for this client (9)");
				}
				else
				{
					clients[c].processing = true;
					clients[c].nb_new_points = *((long*)(m.param[0]));
					clients[c].position_new_points = *((Position*)(m.param[1]));
					clients[c].new_points = new Point[clients[c].nb_new_points];
					for (long i = 0 ; i < clients[c].nb_new_points ; i++) clients[c].new_points[i] = *((Point*)(m.param[2]+i*sizeof(Point)));
					
					m.deleteMessage();
					m.instruction = 4;
					m.return_value = 0;
					m.nb_param = 0;
					m.return_nb_param = 0;
					
					if  (m.send(clients[c].sock) != 0)
					{
						error("error while sending back reply to client (10)");
						close(clients[c].sock);
						clients[c].processing = false;
						return 10;
					}
					
					// lance le thread de traitement
					last_processing_client = c;
					pthread_t processing_thread_ID;
     					pthread_create(&processing_thread_ID, NULL, processing_thread, NULL);
				}
				
				m.deleteMessage();
				
				break;
			}
			
			default :
			{
				error("unknown instruction");
				return 3;
			}
		}
	}
	
	close(clients[c].sock);
	delete me;
	return 0;
}

// thread de traitement des nouvelles données envoyées par le client
int server::processingThread()
{
	int c = last_processing_client;
	info("processing thread started");
	
	// cree l'isometrie de passage du referentiel telemetre -> global
	double no = sqrt(clients[c].position_new_points.orientation.x * clients[c].position_new_points.orientation.x + clients[c].position_new_points.orientation.y * clients[c].position_new_points.orientation.y);
	double x = clients[c].position_new_points.orientation.x / no, y = clients[c].position_new_points.orientation.y / no;
	double theta;
	if  (clients[c].position_new_points.orientation.x / no > 1) theta = 0;
	else if  (clients[c].position_new_points.orientation.x / no < -1) theta = -Pi;
	else theta = acos(x);
	if  (y < 0) theta = -theta;
	Isometry ref_to_global(0, 0, theta, clients[c].position_new_points.position);
	ref_to_global.makeMatrix();
	
	// cree le nouveau nuage a partir du tableau de points
	PointCloud new_cloud(default_epsilon, default_max_world_size);
	for (long i = 0 ; i < clients[c].nb_new_points ; i++) new_cloud.addPoint(ref_to_global.apply(clients[c].new_points[i]));
	delete clients[c].new_points;
	
	/* ici tout le code utile relatif a la reconstruction du nuage de points */
	
	// filtre gaussien sur le nouveau nuage
	long nb_deleted_points = 0;
	//...
	
	if  (clients[c].last_file != 0)
	{
		// suppression des points du nouveau nuage trop loins de l'ancien
		PointCloud filtered_cloud(default_epsilon, default_max_world_size);
		if  (mutual_distance_filter_enabled == 1)
		{
			info("deleting points too far from old cloud...");
			filtered_cloud = new_cloud.referenceFilter(&clients[c].main_cloud, threshold_mutual_distance_filter, nb_closest_points_mutual_distance_filter);
			nb_deleted_points = new_cloud.nb_points - filtered_cloud.nb_points;
			printf("nb_deleted_points = %ld\n", nb_deleted_points);
		}
		else filtered_cloud = new_cloud.copy();
		
		// recalage initial grossier
		info("starting alignment...\n");
		PointCloud sample = filtered_cloud.sampleRandom(sample_alignment);
		Isometry rough_iso = gradientAlgo(clients[c].main_cloud, sample, max_iterations, step_translation, alignment_accuracy, FAST);
		sample.deleteCloud();
		rough_iso.makeMatrix();
		new_cloud.applyTransformation(rough_iso);
		filtered_cloud.applyTransformation(rough_iso);
		
		/*Point* points_tab = filtered_cloud.toArray();
		double* features = new double[filtered_cloud.nb_points];
		printf("...\n");
		double rmin = 0.0001;
		double rmax = 0.001;
		long nb_steps = 5;
		long max_points = 10;
		
		for (long i = 0 ; i < filtered_cloud.nb_points ; i++) {features[i] = filtered_cloud.integralFeature(points_tab[i], rmin, rmax, nb_steps, max_points); printf("%ld - %lf\n", i, features[i]);}
		for (long i = 0 ; i < filtered_cloud.nb_points ; i++) printf("feature %ld = %lf\n", i, features[i]);
		
		// calcule la distribution des points par geometric feature
		long hist_width = 3.5*0.4*sqrt(filtered_cloud.nb_points);
		long* eff = new long[hist_width+1];
		for (long i = 0 ; i < hist_width ; i++) eff[i] = 0;
		for (long i = 0 ; i < filtered_cloud.nb_points ; i++)
		{
			if  (features[i] > 0 && features[i] < 1) eff[(long)(features[i]*((double)hist_width))]++;
		}
		for (long i = 0 ; i < hist_width ; i++) printf("%lf <= feature < %lf : Effectif : %ld\n", ((double)i)/((double)hist_width), ((double)(i+1))/((double)hist_width), eff[i]);
		
		delete eff;
		delete features;
		delete points_tab;*/
		
		/*if  (((double)nb_deleted_points)/((double)clients[c].nb_new_points) < threshold_deleted_points)
		{
			Isometry alignment_transformations[nb_spheres_alignment];
			for (int i = 0 ; i < nb_spheres_alignment ; i++)
			{
				// échantillonnage du nouveau nuage
				info("sampling cloud...");
				PointCloud sphere_sample(default_epsilon, default_max_world_size);
				do {
					sphere_sample.deleteCloud();
					Vect3d center = {Random(-5,5), Random(-0.5,0.5), Random(-5,5)};
					sphere_sample = filtered_cloud.sampleSphere(center, radius_spheres_alignment);
				   } while (sphere_sample.nb_points < 0/*sample_alignment*///);
				/*PointCloud sample = sphere_sample.sampleRandom(sample_alignment);
				sphere_sample.deleteCloud();
			
				// alignement
				info("starting alignment algorithm...");
				alignment_transformations[i] = gradientAlgo(clients[c].main_cloud, sample, max_iterations, step_translation, alignment_accuracy, FAST);
				sample.deleteCloud();
			}
			info("computing best alignment transformation...");
			Isometry alignment_transformation = getBestTransformation(alignment_transformations, nb_spheres_alignment, nb_used_spheres_alignment);
			alignment_transformation.makeMatrix();
			
			// application de la transformation
			info("applying alignment transformation...");
			new_cloud.applyTransformation(alignment_transformation);
		}*/
		filtered_cloud.deleteCloud();
	}
	
	if  (((double)nb_deleted_points)/((double)clients[c].nb_new_points) < threshold_deleted_points)
	{
		// enregistrement du nouveau nuage
		info("saving cloud to file...");
		char file_name[200], char_last_file[20];
		longToChar(clients[c].last_file, char_last_file);
		strcpy(file_name, clients[c].server_path);
		strcat(file_name, "/");
		strcat(file_name, char_last_file);
		strcat(file_name, ".txt");
		new_cloud.saveChar(file_name);
		
		// fusion des 2 nuages
		clients[c].main_cloud.mergeCloud(&new_cloud);
		printf("file saved : %ld points added to model (%ld points total)\n", clients[c].nb_new_points, clients[c].main_cloud.nb_points);
	}
	else
	{
		info("Too many absurd points. Dataset must be incoherent or you're drunk ! Aborting...");
		new_cloud.deleteCloud();
	}
	clients[c].last_file++;  // meme en cas d'abort on incremente pour rester coherent avec le client
	
	return 0;
}
