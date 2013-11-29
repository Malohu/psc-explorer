#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "UrgCtrl.h"

#include "client.h"

using namespace qrk;
using namespace std;

int main(int argc, char* argv[])
{
	srand(time(NULL));
	
	client c("foo.bar");
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
