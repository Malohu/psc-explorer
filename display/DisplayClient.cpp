#include "DisplayClient.h"

#include <GL/glx.h>    /* this includes the necessary X headers */
#include <GL/gl.h>

#include <GL/glu.h>

#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>

// cree un nouveau client
DisplayClient::DisplayClient(char* config_file, int Mode, char* Files_path, long ID_client)
{
	connected = false;
	nb_loops = 0;
	mode = Mode;
	strcpy(files_path, Files_path);
	
	max_display_lists = 1;
	index_display_lists = 1;
	display_lists = new int[max_display_lists];
	
	// pour l'instant tout est hardcodé et on crée un nouveau modèle a chaque lancement du client
	server_port = 18997;
	strcpy(server_ip[0], "42.42.42.1");
	id_client = ID_client;
	nb_new_points = 1000000;
	
	if  (loadConfig(config_file))
	{
		error("cannot load config file");
		exit(0);
	}
}

// parse une ligne du fichier : retourne 0 si la ligne est correcte
int parseLine(char* line, char* var, char* value)
{
	int u = 0, s_var = 0, s_value = 0;
	int cpt_equal = 0;
	while (line[u] != 0 && line[u] != ' ' && line[u] != '=')
	{
		var[s_var++] = line[u];
		if  (line[u++] == '=') cpt_equal++;
	}
	var[s_var] = 0;
	while (line[u] == ' ' || line[u] == '=')
	{
		if  (line[u++] == '=') cpt_equal++;
	}
	while (line[u] != 0 && line[u] != ' ')
	{
		value[s_value++] = line[u];
		if  (line[u++] == '=') cpt_equal++;
	}
	value[s_value] = 0;
	printf("%s = %s\n", var, value);
	
	if  (!s_value) return 1;
	if  (!s_var) return 2;
	if  (cpt_equal != 1) return 3;
	return 0;
}

// charge le fichier de config
int DisplayClient::loadConfig(char* config_file)
{
	FILE *f = fopen(config_file, "rt");
	if  (f == NULL) return 1;
	char c = 42;
	bool newline = true;
	while (newline)
	{
		int u = 0;
		char line[2000], var[2000], value[2000];
		while ((newline = fread(&c, 1, 1, f) != 0) && c != '\n') line[u++] = c;
		line[u] = 0;
		if  (u > 0)
		{
			if  (line[0] == '#') continue;  // comment
			if  (parseLine(line, var, value))
			{
				error("cannot read config file");
				return 2;
			}
			if  (!strcmp(var, "server")) strcpy(server_ip[0], value);
			else if  (!strcmp(var, "port")) sscanf(value, "%d", &server_port);
			else if  (!strcmp(var, "nb_new_points")) sscanf(value, "%ld", &nb_new_points);
		}
	}
	fclose(f);
	return 0;
}

// se connecte au serveur
int DisplayClient::connect()
{
	if  (mode == LOCAL) return 0;
	
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
    	
    	welcome.instruction = 5;
    	welcome.nb_param = 1;
    	welcome.param = new char*[1];
    	welcome.param[0] = new char[10];
    	welcome.param_size[0] = 4;
    	
    	welcome.param[0][0] = (id_client/0x1000000)%256;
    	welcome.param[0][1] = (id_client/0x10000)%256;
    	welcome.param[0][2] = (id_client/0x100)%256;
    	welcome.param[0][3] = (id_client/0x1)%256;
    	
    	if  (welcome.send(sock) != 0)
    	{
    		error("error while sending identification message (4)");
    		return 4;
    	}
    	
    	connected = true;
    	
    	return 0;
}

// boucle principale des actions
int DisplayClient::mainLoop()
{
	if  (mode == LOCAL)
	{
		if  (!nb_loops)
		{
			double x, y, z, w;
    			unsigned short r, g, b;
    			long id, i;
			
			long file_id = 0;
			while (1)
			{
				unsigned short r_color = (file_id*100)%256, g_color = (file_id*100+100)%256, b_color = (file_id*100+200)%256;
				char path[1000], fpath[100];
				strcpy(path, files_path);
				strcat(path, "/");
				longToChar(file_id++, fpath);
				strcat(path, fpath);
				strcat(path, ".txt");
				
				FILE *f = fopen(path, "rt");
				if  (f == NULL) break;
	
				long tmp_nb_points;
				double size, epsilon_pruning, X, Y, Z;
				fscanf(f, "%ld\n%lf\n%lf\n%lf %lf %lf\n", &tmp_nb_points, &size, &epsilon_pruning, &X, &Y, &Z);
				
				glNewList(index_display_lists++, GL_COMPILE_AND_EXECUTE);
				glColor3f(((double)r_color)/256,((double)g_color)/256,((double)b_color)/256);
				glBegin(GL_POINTS);
				for (i = 0 ; i < tmp_nb_points ; i++)
				{
					fscanf(f, "%ld  %lf %lf %lf  %lf  %d %d %d\n", &id, &x, &y, &z, &w, &r, &g, &b);
					glVertex3f(x, y, z);
				}
				glEnd();
				glEndList();
				fclose(f);
			}
		}
		nb_loops++;
		return 0;
	}
	
	Message* me = new Message();
	Message m;
	
	// requete de nouveaux points
	m = *me;
	if  (nb_loops)
	{
		m.instruction = 7;
		m.nb_param = 1;
		m.param = new char*[1];
		m.param[0] = new char[10];
		m.param_size[0] = 10;
		*((long*)m.param[0]) = nb_new_points;
	}
	else  // requete de tous les points existants
	{
		m.instruction = 6;
		m.nb_param = 0;
	}
	nb_loops++;
	
	if  (m.send(sock) != 0)
	{
		error("error while new points query (4)");
		connected = false;
		return 4;
	}
	m.deleteMessage();
	if  (m.recv(sock) != 0)
	{
		error("error while new points reply (5)");
		connected = false;
		return 5;
	}
	
	switch (m.return_value)
	{
		case 0 :  // si OK
		{
			info("New points received !");
			
			long nb_points = *((long*)m.return_param[0]);
			Point* points = new Point[nb_points];
			
			printf("%ld\n", nb_points);
			for (long i = 0 ; i < nb_points ; i++)
			{
				points[i].x = *((double*)(m.return_param[1]+i*42));
				points[i].y = *((double*)(m.return_param[1]+i*42+8));
				points[i].z = *((double*)(m.return_param[1]+i*42+16));
				points[i].w = *((double*)(m.return_param[1]+i*42+24));
				points[i].id = *((long*)(m.return_param[1]+i*42+32));
				points[i].r = *((unsigned short*)(m.return_param[1]+i*42+36));
				points[i].g = *((unsigned short*)(m.return_param[1]+i*42+38));
				points[i].b = *((unsigned short*)(m.return_param[1]+i*42+40));
			}
			
			m.deleteMessage();
			
			
			// cree la DisplayList
			if  (index_display_lists == max_display_lists)
			{
				int* tmp = new int[max_display_lists*2];
				for (int i = 0 ; i < max_display_lists ; i++) tmp[i] = display_lists[i];
				delete display_lists;
				max_display_lists *= 2;
				display_lists = tmp;
			}
			
			createDisplayList(points, nb_points, index_display_lists++, rand()%200+55, rand()%200+55,rand()%200+55);
			
			delete points;
			
			info("New display list created");
				
			break;
		}
		
		default :
			break;

	}
	
	//m.deleteMessage();
	
	delete me;
	return 0;
}
