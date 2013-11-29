#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "geometry.h"
#include "point.h"
#include "pointcloudnode.h"
#include "pointcloud.h"

Point* randomPoints(long n, double size);  // cree un tableau de n points randoms dans le cube de cote 2*size centré en 0
void splitX(Point* p, long n, PointCloud* p1, PointCloud* p2, double start, double stop);  // decoupe un tableau de points comme il faut en 2 arbres
void splitX(Point* p, long n, Point** p1, Point** p2, long* n1, long* n2, double start, double stop);  // decoupe le tableau en 2 tableaux
double quadratic(Point* p, Point* p2, long n2);  // calcule l'ecart quadratique moyen entre les points de p2 et leur position de référence dans p
double errorFunction(Point* p1, Point* p2, long n1, long n2, double max_dist);  // fonction d'erreur pour l'algo du gradient
Isometry gradientAlgo(PointCloud p1, PointCloud p2, long max_iter, double step, double accuracy, long algo);  // algorithme du gradient sur l'ensemble des transformations possibles

double errorFunctionPlane(Vect3d c, Point p, Point* neighbours, long n);  // la fonction d'erreur pour l'algo de moindres carrés (calcul du plan tangent a la surface)
Vect3d tangentPlane(Point p, Point* neighbours, long n, long max_iter, double step, double accuracy);  // calcule l'équation du plan tangent a la surface au point p par la méthode des moindes carrés
double errorFunctionHess(Vect3d c, Vect3d* p, long n);  // la fonction d'erreur pour l'algo de moindres carrés (calcul de la hessienne a la surface)
Vect3d hessian(Vect3d* neighbours, long n, long max_iter, double step, double accuracy);  // calcule la hessienne de la surface tangente au point p par la méthode des moindes carrés

class BackTrack
{
	Point *p1, *p2;
	PointCloud reference_, new_cloud_;
	long max_null, min_matches, current_max_depth;
	long long nb_calls, nb_ends;
	double factor_min, error;
	long *assoc, *best_assoc;  // tableau d'association entre les points
	double* max_partial;  // tableau contenant l'erreur maxi a chaque profondeur
	
	public :
		BackTrack(PointCloud reference, PointCloud align, long sample1, long sample2, long Min_matches, double Factor_min);
		Isometry exec();
		void BackTrackRec(long p, long current_null, double partial);
		Isometry getIsometry(long max_iter, double step, double accuracy);
		double errorFunction(Point* p1, Point* p2, Isometry iso, long n);
		void deleteBackTrack();
		
		Isometry algoN();
		double getError();
};

#endif
