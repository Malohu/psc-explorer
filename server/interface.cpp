#include "server.h"

/* une interface console basique pour contrôler les clients
si quelqu'un se sent le courage de coder une interface graphique, qu'il ne s'en prive pas !! */
int server::interface()
{
	int current_client = 0, nb_words;
	bool running = true;
	char cmd[1000];
	char** words;
	printf("Explorateur 3D, serveur v0.9\n");
	while (running)
	{
		printf(">: ");
		gets(cmd);
		nb_words = parse(cmd, &words);
		//printf("nb_words = %d\n", nb_words);
		if  (nb_words == 0) continue;
		
		// affiche la liste des clients connectés
		if  (equal(words[0], "clients"))
		{
			printf("%d client(s) connected\n", nb_clients);
			for (int i = 0 ; i < MAX_CLIENTS ; i++)
			{
				if  (clients[i].used) printf("\t%d : %s\n", i, inet_ntoa(*((in_addr*)&clients[i].client_addr.sin_addr.s_addr)));
			}
		}
		
		// sélectionne un client
		if  (equal(words[0], "use"))
		{
			if  (nb_words >= 2)
			{
				int c = charToInt(words[1]);
				if  (c >= 0 && c < MAX_CLIENTS)
				{
					if  (clients[c].used)
					{
						current_client = c;
						printf("Current client set to %d\n", c);
					}
					else printf("Error : no such client\n");
				}
				else printf("Error : no such client\n");
			}
		}
		
		// déclenche une acquisition sur le client courant
		if  (equal(words[0], "get"))
		{
			if  (clients[current_client].used)
			{
				if  (clients[current_client].action == 0)
				{
					clients[current_client].action = 1;
					printf("Acquisition request sent to client\n");
				}
				else printf("Error : already an action in the queue for this client\n");
			}
			else printf("Error : no client selected\n");
		}
		
		// deplace le client courant
		if  (equal(words[0], "move"))
		{
			if  (nb_words >= 2)
			{
				int c = charToInt(words[1]);
				if  (c >= 0)
				{
					if  (clients[current_client].used)
					{
						if  (clients[current_client].action == 0)
						{
							clients[current_client].action = 2;
							
							double d = ((double)c)/1000;
							double n = sqrt(clients[current_client].position.orientation.x * clients[current_client].position.orientation.x + clients[current_client].position.orientation.y * clients[current_client].position.orientation.y);
							Vect3d p = {clients[current_client].position.orientation.x / n * d, clients[current_client].position.orientation.y / n * d, 0.0};
							clients[current_client].next_position.position = add(clients[current_client].position.position, p);
							clients[current_client].next_position.orientation = clients[current_client].position.orientation;
							
							printf("Move request sent to client\n");
						}
						else printf("Error : already an action in the queue for this client\n");
					}
					else printf("Error : no client selected\n");
				}
				else printf("Error : invalid move\n");
			}
		}
		
		// fait tourner le client courant
		if  (equal(words[0], "turn"))
		{
			if  (nb_words >= 3)
			{
				if  (equal(words[1], "left") || equal(words[1], "right"))
				{
					int c = charToInt(words[2]);
					if  (c >= 0 && c <= 180)
					{
						if  (clients[current_client].used)
						{
							if  (clients[current_client].action == 0)
							{
								if  (equal(words[1], "left")) c = -c;
								clients[current_client].action = 2;
								
								double d = ((double)c)*Pi/180;
								Vect3d p = {clients[current_client].position.orientation.x * cos(d) + clients[current_client].position.orientation.y * sin(d), clients[current_client].position.orientation.y * cos(d) - clients[current_client].position.orientation.x * sin(d), 0};
								clients[current_client].next_position.orientation = p;
								clients[current_client].next_position.position = clients[current_client].position.position;
								
								printf("Turn request sent to client\n");
							}
							else printf("Error : already an action in the queue for this client\n");
						}
						else printf("Error : no client selected\n");
					}
					else printf("Error : Invalid move\n");
				}
				else printf("Error : Invalid move\n");
			}
		}
		
		// enregistre le nuage de points
		if  (equal(words[0], "save"))
		{
			if  (nb_words >= 2)
			{
				if  (clients[current_client].used)
				{
					char path[200];
					strcpy(path, clients[current_client].server_path);
					strcat(path, "/");
					strcat(path, words[1]);
					if  (clients[current_client].main_cloud.saveChar(path) != 0)
					{
						printf("point cloud saved in file\n");
					}
					else printf("Error : cannot open file\n");
				}
				else printf("Error : no client selected\n");
			}
		}
		
		// eteint le client courant
		if  (equal(words[0], "stop"))
		{
			if  (clients[current_client].used)
			{
				if  (clients[current_client].action == 0)
				{
					clients[current_client].action = 3;
					printf("Shutdown request sent to client\n");
				}
				else printf("Error : already an action in the queue for this client\n");
			}
			else printf("Error : no client selected\n");
		}
		
		// eteint le serveur
		if  (equal(words[0], "exit"))
		{
			running = false;
		}
		
		// active / desactive l'algorithme d'exploration sur le client courant
		if  (equal(words[0], "auto"))
		{
		}
		
		// efface l'écran
		if  (equal(words[0], "clear"))
		{
			system("clear");
			printf("Explorateur 3D, serveur v0.9\n");
		}
		
		// deplace le servomoteur
		if  (equal(words[0], "servo"))
		{
			if  (nb_words >= 2)
			{
				int c = charToInt(words[1]);
				if  (c >= 0 && c <= 180)
				{
					if  (clients[current_client].used)
					{
						if  (clients[current_client].action == 0)
						{
							clients[current_client].action = 4;
							clients[current_client].next_telemeter_move = (double)c;
							
							printf("Servo request sent to client\n");
						}
						else printf("Error : already an action in the queue for this client\n");
					}
					else printf("Error : no client selected\n");
				}
				else printf("Error : invalid move for servomotor (range = 0 - 180\n");
			}
		}
		
		for (int i = 0 ; i < nb_words ; i++) delete words[i];
		delete words;
	}
	
	return 0;
}
