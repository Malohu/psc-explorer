#include "image.h"

Image::Image(long Width, long Height, int ColorDepth)
{
	width = Width;
	height = Height;
	color_depth = ColorDepth;
	points = new unsigned short**[width];
	for (long i = 0 ; i < width ; i++)
	{
		points[i] = new unsigned short*[height];
		for (long j = 0 ; j < height ; j++)
		{
			points[i][j] = new unsigned short[color_depth];
			for (int k = 0 ; k < color_depth ; k++) points[i][j][k] = 0;
		}
	}
}

Image::Image(long Width, long Height)
{
	Image(Width, Height, 3);  // profondeur de couleur par defaut = 3
}

// charge une image depuis un fichier
Image::Image(char* file_name)
{
	
}

// enregistre l'image et retourne la taille du fichier en octets (0 si erreur)
long Image::save(char* file_name)
{
	return 0;
}

// supprime une image
void Image::deleteImage()
{
	for (long i = 0 ; i < width ; i++)
	{
		for (long j = 0 ; j < height ; j++)
		{
			delete points[i][j];
		}
		delete points[i];
	}
	delete points;
	return;
}
