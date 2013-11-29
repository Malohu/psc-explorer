#include "client.h"

// cree un nouveau modele
client::client(char* config_file)
{
	working = true;
	connected = false;
	
	// pour l'instant tout est hardcodé et on crée un nouveau modèle a chaque lancement du client
	server_port = 18997;
	strcpy(server_ip[0], "129.104.201.99");
	id_client = rand()%10000 + 10000 * (rand()%10000);
	longToChar(id_client, server_path);
	default_max_world_size = 100.0;
	default_epsilon = 0.01;
	sleep_server_busy = 10;
	last_file = 0;
	delay_step_acquisition = 250000;  // en microsecondes
	nb_points_acquisition = 1081;
	step_acquisition = 0.5;
	offset_phi = 0;
	strcpy(telemeter_device, "/dev/ttyACM0");
	Position* p = new Position();
	position = *p;
	delete p;
	tel.setDefault(delay_step_acquisition, nb_points_acquisition, step_acquisition, offset_phi, telemeter_device);
	motor.init();
	
	// nuage de points vide
	PointCloud* pc = new PointCloud(default_max_world_size, default_epsilon);
	main_cloud = *pc;
	delete pc;
}

// charge un ancien modele
/*client::client(char* config_file)
{
	
}*/

// se connecte au serveur
int client::connect()
{
	connected = false;
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if  (sock < 0)
	{
		error("cannot open socket (1)");
		return 1;
	}
	server = gethostbyname(server_ip[0]);
    	if (server == NULL)
    	{
    		error("gethostbyname (2)");
        	return 2;
    	}
    	serv_addr.sin_family = AF_INET;
    	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    	serv_addr.sin_port = htons(server_port);
    	
    	if (::connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    	{
    		error("cannot connect to server (3)");
    		return 3;
    	}
    	
    	// envoie les infos d'identification
    	Message* w = new Message();
    	Message welcome = *w;
    	delete w;
    	
    	welcome.instruction = 1;
    	welcome.nb_param = 4;
    	welcome.param = new char*[4];
    	welcome.param[0] = new char[10];
    	welcome.param[1] = new char[200];
    	welcome.param[2] = new char[10];
    	welcome.param[3] = new char[sizeof(Position)+1];
    	welcome.param_size[0] = 4;
    	welcome.param_size[1] = 200;
    	welcome.param_size[2] = 4;
    	welcome.param_size[3] = sizeof(Position)+1;
    	
    	welcome.param[0][0] = (id_client/0x1000000)%256;
    	welcome.param[0][1] = (id_client/0x10000)%256;
    	welcome.param[0][2] = (id_client/0x100)%256;
    	welcome.param[0][3] = (id_client/0x1)%256;
    	strcpy(welcome.param[1], server_path);
    	welcome.param[2][0] = (last_file/0x1000000)%256;
    	welcome.param[2][1] = (last_file/0x10000)%256;
    	welcome.param[2][2] = (last_file/0x100)%256;
    	welcome.param[2][3] = (last_file/0x1)%256;
    	*((Position*)(welcome.param[3])) = position;
    	
    	if  (welcome.send(sock) != 0)
    	{
    		error("error while sending identification message (4)");
    		return 4;
    	}
    	
    	connected = true;
    	
    	return 0;
}

// boucle principale des actions
int client::mainLoop()
{
	Message* me = new Message();
	Message m;
	
	// requete de position
	m = *me;
	m.instruction = 2;
	
	if  (m.send(sock) != 0)
	{
		error("error while sending position query (1)");
		connected = false;
		return 1;
	}
	if  (m.recv(sock) != 0)
	{
		error("error while receiving position reply (2)");
		connected = false;
		return 2;
	}
	
	if  (m.return_value != 0) error("server won't give position (3)");
	else position = *((Position*)(m.return_param[0]));
	
	m.deleteMessage();
	
	// requete d'action
	m = *me;
	m.instruction = 3;
	
	if  (m.send(sock) != 0)
	{
		error("error while sending action query (4)");
		connected = false;
		return 4;
	}
	m.deleteMessage();
	if  (m.recv(sock) != 0)
	{
		error("error while receiving action reply (5)");
		connected = false;
		return 5;
	}
	
	switch (m.return_value)
	{
		case 0 :
		{
			//info("Nothing to do !!");
			break;
		}
		case 1 :
		{
			info("Should make an acquisition ...");
			
			long nb_points;
			Point* points = tel.acquisition(&nb_points);  // fait une acquisition
			
			Message acq = *me;
			info("Sending point cloud to server ...");
			
			do {
				if  (acq.return_value == 1)
				{
					acq.return_value = 0;
					info("Server busy, waiting ...");
					sleep(sleep_server_busy);
					info("Retry ...");
				}
				
				acq.instruction = 4;
				acq.nb_param = 3;
				acq.param = new char*[3];
				acq.param[0] = new char[10];
				acq.param[1] = new char[sizeof(Position)+1];
				acq.param[2] = new char[nb_points*sizeof(Point)+1];
				acq.param_size[0] = 10;
				acq.param_size[1] = sizeof(Position);
				acq.param_size[2] = nb_points*sizeof(Point);
				*((long*)acq.param[0]) = nb_points;
				*((Position*)acq.param[1]) = position;
				for (long i = 0 ; i < nb_points ; i++) *((Point*)(acq.param[2]+i*sizeof(Point))) = points[i];
				
				if  (acq.send(sock) != 0)
				{
					error("error while sending point cloud to server (7)");
					connected = false;
					acq.deleteMessage();
					delete points;
					return 7;
				}
				
				acq.deleteMessage();
				
				if  (acq.recv(sock) != 0)
				{
					error("error while receiving point cloud reply (8)");
					connected = false;
					delete points;
					return 8;
				}
			   } while (acq.return_value == 1);
			   
			   acq.deleteMessage();
			   delete points;
			   last_file++;
			   info("point cloud sent to server");
				
			break;
		}
		case 2 :
		{
			next_position = *((Position*)(m.return_param[0]));
			printf("Info : Should go to position (%lf, %lf, %lf) and orientation (%lf, %lf, %lf) ...\n", next_position.position.x, next_position.position.y, next_position.position.z, next_position.orientation.x, next_position.orientation.y, next_position.orientation.z);
			motor.move(position, next_position);
			break;
		}
		case 3 :  // quitte proprement le programme
		{
			working = false;
			connected = false;
			break;
		}
		case 4 :  // deplace le servomoteur a la position indiquee (DEBUG)
		{
		 	double pos_servo = *((double*)(m.return_param[0]));
		 	printf("Info : Should turn telemeter to position %lf\n", pos_servo);
		 	if  (pos_servo == 0) tel.zero();
		 	else tel.update();
		 	break;
		}
		default :
		{
			error("invalid action sent by server (6)");
		}
	}
	
	m.deleteMessage();
	
	delete me;
	return 0;
}
