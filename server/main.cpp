#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server.h"

using namespace std;

int main(int argc, char* argv[])
{
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
		strcpy(config_file, "server.conf");
	}
	
	server s(config_file);
	s.loadConfig();
	s.startListening();
	s.interface();
	s.stopListening();
	
	return 0;
}
