#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include "UrgCtrl.h"

#include "client.h"

//using namespace qrk;
using namespace std;

int main(int argc, char* argv[])
{
	srand(time(NULL));
	
	char* config_file = NULL;
	int opt;
	while ((opt = getopt (argc, argv, "c:")) != -1)
	{
		switch (opt)
		{
			case 'c' :
				config_file = optarg;
				break;

			default :
				printf("Error : invalid argument\n");
				exit(0);
		}
	}
	
	if  (config_file == NULL)
	{
		config_file = new char[1000];
		strcpy(config_file, "client.conf");
	}
	
	client c(config_file);
	while (c.working)
	{
		c.connect();
		while (c.working && c.connected)
		{
			sleep(1);
			c.mainLoop();
		}
		close(c.sock);
		sleep(1);
	}
	
	// enregistre la config
	
	return 0;
}
