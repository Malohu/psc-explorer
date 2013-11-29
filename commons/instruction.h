#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "../commons/error.h"

#define MAX_PARAM 5
#define RETURN_MAX_PARAM 5

// un message a passer a travers le reseau
class Message
{
	public :
	
	long instruction, return_value;
	long nb_param, return_nb_param;
	long param_size[MAX_PARAM], return_param_size[RETURN_MAX_PARAM];
	char **param, **return_param;
	
		Message();
		Message(int Instruction, int Nb_param);
		Message(int Instruction, int Nb_param, long* Param_size);
		
		void deleteMessage();  // detruit le message
		int send(int socket);  // envoie sur le socket
		int recv(int socket);  // recoit sur le socket
};

#endif
