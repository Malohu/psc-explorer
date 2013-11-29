#include "../commons/error.h"

// traite les erreurs
int error(char* msg)
{
	printf("Error : %s\n", msg);
	return 0;
}

// traite les messages d'information
int info(char* msg)
{
	printf("Info : %s\n", msg);
}

// convertit un long en chaine de caracteres
void longToChar(long l, char* str)
{
	str[0] = (l/1000000000)%10 + 48;
	str[1] = (l/100000000)%10 + 48;
	str[2] = (l/10000000)%10 + 48;
	str[3] = (l/1000000)%10 + 48;
	str[4] = (l/100000)%10 + 48;
	str[5] = (l/10000)%10 + 48;
	str[6] = (l/1000)%10 + 48;
	str[7] = (l/100)%10 + 48;
	str[8] = (l/10)%10 + 48;
	str[9] = (l/1)%10 + 48;
	str[10] = 0;
	
	return;
}

// teste l'égalite de 2 chaines de caracteres
bool equal(char* a, char* b)
{
	long i = 0;
	while (a[i] != 0 && b[i] != 0)
	{
		if  (a[i] != b[i]) return false;
		i++;
	}
	if  (a[i] != b[i]) return false;
	
	return true;
}

// découpe une chaine de caracteres en mots selon les espaces
int parse(char* cmd, char*** words)
{
	int i = 0, j = 0, k;
	int nb_words = 0;
	while (cmd[i] != 0)
	{
		while (cmd[i] != ' ' && cmd[i] != 0) i++;
		nb_words++;
		while (cmd[i] == ' ') i++;
	}
	
	*words = new char*[nb_words];
	i = 0;
	while (cmd[i] != 0)
	{
		k = 0;
		(*words)[j] = new char[1000];
		while (cmd[i] != 0 && cmd[i] != ' ')
		{
			(*words)[j][k++] = cmd[i++];
		}
		(*words)[j][k++] = 0;
		while (cmd[i] == ' ') i++;
		j++;
	}
	
	return j;
}

long charToLong(char* a)
{
	long r = 0, i = 0;
	while (a[i] != 0)
	{
		r = r*10+(a[i++]-48);
	}
	return r;
}

int charToInt(char* a)
{
	int r = 0, i = 0;
	while (a[i] != 0)
	{
		r = r*10+(a[i++]-48);
	}
	return r;
}

// lit une ligne du fichier f ouvert en mode binaire, la stocke dans buf et retourne le nombre de caracteres
long readLine(FILE* f, char* buf)
{
	long i = 0;
	char c;
	while (fread(&c, 1, 1, f) != 0 && c != '\n') buf[i++] = c;
	buf[i] = 0;
	return i;
}
