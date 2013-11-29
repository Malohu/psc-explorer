#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

#include "../commons/geometry.h"
#include "../commons/point.h"
#include "../commons/pointcloudnode.h"

class PointCloud
{
	public :
	
	PointCloudNode* root;
	long nb_points;
	double epsilon_pruning;
	Vect3d center;  // le centre (a priori (0,0,0))
	double size;  // la taille du cube de la racine

		PointCloud();
		PointCloud(double Epsilon, double Size);
		PointCloud(char* file_name, double Epsilon, double Size);  // constructeur qui charge un nuage de points a partir d'un fichier lisible par un humain
		
		long addPoint(Point p);  // ajoute un point et renvoie le nombre de points du nuage
		//long delPoint(long id);  // supprime un point et renvoie le nombre de points restants
		//long delPoints(Vect3d v, long d);  // supprime tous les points dans un rayon d autour de v
		//long prune(); // elague tout l'octree a epsilon_prune pres a l'aide d'un double parcours en profondeur et renvoie le nombre de points 
		void setEpsilon(double e);
		long mergeCloud(PointCloud* pc);  // vide le nuage pc dans le nuage courant (et detruit le nuage pc)
		long saveChar(char* file_name);  // enregistre le nuage de points dans un fichier et retourne la taille du fichier en octets (0 en cas d'erreur)
		//long downSize(long n); // supprime assez de points de l'octree pour qu'il en reste moins que n
		Point closestPoint(double* dist, Point reference);  // renvoie de point de l'arbre le plus proche de reference et stocke la distance dans dist
		void closestPoints(double* dist, Point reference, long nb_closest_points);  // renvoie les points de l'arbre les plus proches de reference et stocke les distances dans dist
		Point closestPoint(Point reference);  // renvoie de point de l'arbre le plus proche de reference
		long nClosestPoints(double* dist, Point* points, Point reference, double rmax, long max_points);  // renvoie tous les points situes a une distance inferieure a rmax de reference (dans la limite de max_points) et retourne le nombre de points trouvés
		PointCloud copy();  // renvoie une copie integrale du nuage de points
		void applyTransformation(Isometry t);  // applique une transformation a tout le nuage
		PointCloud applyTransformationCopy(Isometry t);  // applique une transformation a tout le nuage et renvoie une copie dudit nuage
		double errorAlignment(Isometry t);  // calcule la distance moyenne entre un point du nuage et son image par la transformation t
		double errorAlignment(Isometry t, Isometry s);  // calcule la distance moyenne entre un point du nuage et son image par la transformation (s o t)
		void deleteCloud();  // supprime le nuage
		PointCloud sampleRandom(long nb_points_sample);  // retourne un sous-nuage de points choisi aléatoirement
		PointCloud sampleSphere(Vect3d sphere_center, double radius);  // retourne un sous-nuage composé des points situés dans une certaine sphère
		double errorFunction(PointCloud* reference, long nb_closest_neighbours, double max_dist);  // parcourt l'arbre en profondeur et calcule l'erreur par rapport au nuage "reference"
		PointCloud referenceFilter(PointCloud* reference, double threshold, long nb_closest_points);  // filtre les points trop loins du nuage de référence et retourne le nuage épuré
		Point* toArray();  // convertit l'arbre en tableau
		double integralFeature(Point p, double rmin, double rmax, long nb_steps, long max_points);  // calcule une caracteristique geometrique en un point p du nuage
		
};

#endif
