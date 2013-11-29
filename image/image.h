#ifndef IMAGE_H
#define IMAGE_H

class Image
{
	public :
	
	long width, height;
	int color_depth;
	unsigned short*** points;
		
		Image(long Width, long Height, int ColorDepth);
		Image(long Width, long Height);
		Image(char* file_name);  // charge une image depuis un fichier
		
		long save(char* file_name);  // enregistre l'image et retourne la taille du fichier en octets (0 si erreur)
		void deleteImage();  // supprime une image
};

#endif
