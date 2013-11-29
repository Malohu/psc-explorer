#ifndef POINT_CLOUD_NODE_H
#define POINT_CLOUD_NODE_H

#include "../commons/geometry.h"
#include "../commons/point.h"


class PointCloud;

class PointCloudNode
{
	public :
	
	Point* point;
	PointCloudNode *next_000, *next_001, *next_010, *next_011, *next_100, *next_101, *next_110, *next_111;
	bool is_point;
	int depth;  // profondeur depuis la racine
	Vect3d center;  // le centre geometrique
	double size;  // le coté du cube
	long nb_points;  // nombre de points dans le sous-arbre

		PointCloudNode(Point p, Vect3d Center, double Size);
		PointCloudNode(Point p, double X_center, double Y_center, double Z_center, double Size);
		PointCloudNode(double X, double Y, double Z, Vect3d Center, double Size);
		PointCloudNode(double X, double Y, double Z, double W, Vect3d Center, double Size);
		PointCloudNode(double X, double Y, double Z, int R, int G, int B, Vect3d Center, double Size);
		PointCloudNode(double X, double Y, double Z, double W, int R, int G, int B, Vect3d Center, double Size);
		
		long addPoint(Point p);  // ajoute un point et retourne le nombre de points du sous-arbre
		long closestPoint(double* dist, Point* closest, Point reference);  // renvoie de point de l'arbre le plus proche de reference et stocke la distance dans "dist" et retourne le nombre de noeuds parcourus
		long closestPoints(double* dist, Point* closest, Point reference, double rmax, long nb_closest_points, long* nb_found);  // recherche des point de l'arbre les plus proches de "reference" situés dans un rayon rmax et stocke les distance dans "dist" et renvoie le nombre de noeuds parcourus
		void copyRec(PointCloud* root);  // copie le sous-arbre de current dans le nuage root
		long mergeAll(PointCloud* pc);  // vide le nuage courant dans le nuage pc
		void applyTransformation(Isometry t, PointCloud* new_cloud);  // applique une transformation a tout le nuage et met a jour la structure d'arbre
		void applyTransformationCopy(Isometry t, PointCloud* new_cloud);  // applique une transformation a tout le nuage et retourne une copie de cet arbre
		double errorAlignment(Isometry t);  // calcule la distance moyenne entre un point du nuage et son image par la transformation t
		double errorAlignment(Isometry t, Isometry s);  // calcule la distance moyenne entre un point du nuage et son image par la transformation s o t
		void deleteNode();
		void errorFunctionRec(PointCloud* reference, long nb_closest_neighbours, long* nb_points_included, double* errors);  // parcourt l'arbre en profondeur et calcule l'erreur par rapport au nuage "reference"
		void sampleRandom(PointCloud* sample_cloud, long nb_points_sample);  // extrait un sous-nuage de points
		void sampleSphere(PointCloud* sample_cloud, Vect3d center_sphere, double radius);  // extrait tous les points situés dans une certaine sphere
		long referenceFilter(PointCloud* reference, PointCloud* destination, double threshold, long nb_closest_points);  // filtre les points trop loins du nuage de référence et ajoute les points gardés au nuage destination et retourne le nombre de points du sous nuage de destination créé
		void toArray(Point* t, long* index);  // convertit l'arbre en tableau
		
		long saveChar(FILE* f);  // enregistre le nuage de points dans un format lisible par un humain
};

#endif
