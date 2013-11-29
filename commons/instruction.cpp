#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "../commons/instruction.h"

// 3 variantes du constructeur
Message::Message()
{
	instruction = 0;
	return_value = 0;
	nb_param = 0;
	return_nb_param = 0;
	for (int i = 0 ; i < MAX_PARAM ; i++) param_size[i] = 0;
	for (int i = 0 ; i < RETURN_MAX_PARAM ; i++) return_param_size[i] = 0;
	param = NULL;
	return_param = NULL;
}

Message::Message(int Instruction, int Nb_param)
{
	instruction = Instruction;
	return_value = 0;
	nb_param = Nb_param;
	return_nb_param = 0;
	for (int i = 0 ; i < MAX_PARAM ; i++) param_size[i] = 0;
	for (int i = 0 ; i < RETURN_MAX_PARAM ; i++) return_param_size[i] = 0;
	param = NULL;
	return_param = NULL;
}

Message::Message(int Instruction, int Nb_param, long* Param_size)
{
	instruction = Instruction;
	return_value = 0;
	nb_param = Nb_param;
	return_nb_param = 0;
	for (int i = 0 ; i < MAX_PARAM ; i++) param_size[i] = Param_size[i];
	for (int i = 0 ; i < RETURN_MAX_PARAM ; i++) return_param_size[i] = 0;
	param = NULL;
	return_param = NULL;
}

// detruit le message
void Message::deleteMessage()
{
	if  (nb_param != 0)
	{
		for (int i = 0 ; i < nb_param ; i++) delete param[i];
		delete param;
		param = NULL;
	}
	if  (return_nb_param != 0)
	{
		for (int i = 0 ; i < return_nb_param ; i++) delete return_param[i];
		delete return_param;
		return_param = NULL;
	}
	return;
}

// envoie sur le socket : le message est suppose être consistant
int Message::send(int socket)
{
	// calcule la taille du message en octets
	long size = 0;
	size += 4 * sizeof(long);
	if  (nb_param != 0)
	{
		for (int i = 0 ; i < nb_param ; i++) size += param_size[i] + sizeof(long);
	}
	if  (return_nb_param != 0)
	{
		for (int i = 0 ; i < return_nb_param ; i++) size += return_param_size[i] + sizeof(long);
	}
	
	// construit le buffer a envoyer
	char* buf = new char[size];
	long cpt = 16;
	buf[0] = (char)(instruction / 0x1000000);
	buf[1] = (char)((instruction / 0x10000)%256);
	buf[2] = (char)((instruction / 0x100)%256);
	buf[3] = (char)((instruction / 0x1)%256);
	buf[4] = (char)(return_value / 0x1000000);
	buf[5] = (char)((return_value / 0x10000)%256);
	buf[6] = (char)((return_value / 0x100)%256);
	buf[7] = (char)((return_value / 0x1)%256);
	buf[8] = (char)(nb_param / 0x1000000);
	buf[9] = (char)((nb_param / 0x10000)%256);
	buf[10] = (char)((nb_param / 0x100)%256);
	buf[11] = (char)((nb_param / 0x1)%256);
	buf[12] = (char)(return_nb_param / 0x1000000);
	buf[13] = (char)((return_nb_param / 0x10000)%256);
	buf[14] = (char)((return_nb_param / 0x100)%256);
	buf[15] = (char)((return_nb_param / 0x1)%256);
	for(long i = 0 ; i < nb_param ; i++)
	{
		buf[cpt++] = (char)(param_size[i] / 0x1000000);
		buf[cpt++] = (char)((param_size[i] / 0x10000)%256);
		buf[cpt++] = (char)((param_size[i] / 0x100)%256);
		buf[cpt++] = (char)((param_size[i] / 0x1)%256);
		for (long j = 0 ; j < param_size[i] ; j++) buf[cpt++] = param[i][j];
	}
	for(long i = 0 ; i < return_nb_param ; i++)
	{
		buf[cpt++] = (char)(return_param_size[i] / 0x1000000);
		buf[cpt++] = (char)((return_param_size[i] / 0x10000)%256);
		buf[cpt++] = (char)((return_param_size[i] / 0x100)%256);
		buf[cpt++] = (char)((return_param_size[i] / 0x1)%256);
		for (long j = 0 ; j < return_param_size[i] ; j++) buf[cpt++] = return_param[i][j];
	}
	if  (cpt != size)
	{
		error("mismatch in send buffer size");
		delete buf;
		return 1;
	}
	
	// envoie ...
	long n = 0;
	do {
		n += write(socket, buf+n, cpt-n);
	   } while (n < cpt);
	
	delete buf;
	if  (n != cpt)
	{
		error("incorrect number of bytes sent");
		return 2;
	}
	
	return 0;
}

// recoit sur le socket : le message dans lequel on écrit est supposé être vide (pointeurs nulls)
int Message::recv(int socket)
{
	char buf[16];
	long cpt = 0;
	long n = read(socket, buf, 16);
	if  (n != 16)
	{
		error("incorrect number of bytes received (1)");
		return 1;
	}
	instruction = ((unsigned char)buf[0]) * 0x1000000 + ((unsigned char)buf[1]) * 0x10000 + ((unsigned char)buf[2]) * 0x100 + ((unsigned char)buf[3]);
	return_value = ((unsigned char)buf[4]) * 0x1000000 + ((unsigned char)buf[5]) * 0x10000 + ((unsigned char)buf[6]) * 0x100 + ((unsigned char)buf[7]);
	nb_param = ((unsigned char)buf[8]) * 0x1000000 + ((unsigned char)buf[9]) * 0x10000 + ((unsigned char)buf[10]) * 0x100 + ((unsigned char)buf[11]);
	return_nb_param = ((unsigned char)buf[12]) * 0x1000000 + ((unsigned char)buf[13]) * 0x10000 + ((unsigned char)buf[14]) * 0x100 + ((unsigned char)buf[15]);
	cpt = 16;
	
	if  (nb_param < 0 || return_nb_param < 0)
	{
		error("invalid number of parameters (42)");
		return 42;
	}
	param = new char*[nb_param];
	return_param = new char*[return_nb_param];
	
	for (long i = 0 ; i < nb_param ; i++)
	{
		n = read(socket, buf, 4);
		if  (n != 4)
		{
			error("incorrect number of bytes received (2)");
			return 2;
		}
		param_size[i] = ((unsigned char)buf[0]) * 0x1000000 + ((unsigned char)buf[1]) * 0x10000 + ((unsigned char)buf[2]) * 0x100 + ((unsigned char)buf[3]);
		if  (param_size[i] <= 0)
		{
			error("invalid param size (3)");
			return 3;
		}
		param[i] = new char[param_size[i]];
		
		n = 0;
		do {
			n += read(socket, param[i]+n, param_size[i]-n);
		   } while(n < param_size[i]);
		
		if  (n != param_size[i])
		{
			error("incorrect number of bytes received (4)");
			delete param[i];
			return 4;
		}
	}
	
	for (long i = 0 ; i < return_nb_param ; i++)
	{
		n = read(socket, buf, 4);
		if  (n != 4)
		{
			error("incorrect number of bytes received (5)");
			return 5;
		}
		return_param_size[i] = ((unsigned char)buf[0]) * 0x1000000 + ((unsigned char)buf[1]) * 0x10000 + ((unsigned char)buf[2]) * 0x100 + ((unsigned char)buf[3]);
		if  (return_param_size[i] <= 0)
		{
			error("invalid param size (6)");
			return 6;
		}
		return_param[i] = new char[return_param_size[i]];
		
		n = 0;
		do {
			n += read(socket, return_param[i]+n, return_param_size[i]-n);
		   } while (n < return_param_size[i]);
		
		if  (n != return_param_size[i])
		{
			error("incorrect number of bytes received (7)");
			delete return_param[i];
			return 7;
		}
	}
	
	return 0;
}
