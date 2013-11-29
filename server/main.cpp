#include <stdio.h>
#include <stdlib.h>
#include "server.h"

using namespace std;

int main(int argc, char* argv[])
{
	server s("foo.bar");
	s.loadConfig();
	s.startListening();
	s.interface();
	s.stopListening();
	
	return 0;
}
