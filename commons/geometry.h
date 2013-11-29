#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "point.h"

#define Pi 3.141592
#define SQRT_3 1.732050808
#define INFTY 1e+42

#define GRAD 0
#define FAST 1

struct Vect3d
{
	double x, y, z;
		
		double norm();  // la norme
		Vect3d normalizeCopy();  // retourne une copie normalisee du vecteur
		void normalize();  // normalise le vecteur
		double distance(Vect3d v);  // distance au vecteur v
};

class Matrix3d  // une matrice
{
	public:
	
	double m11, m12, m13, m21, m22, m23, m31, m32, m33;  // la partie lineaire
	Vect3d translation;  // la translation
	
		Matrix3d();
		Matrix3d(double M11, double M12, double M13, double M21, double M22, double M23, double M31, double M32, double M33, Vect3d Translation);
		Vect3d apply(Vect3d v);
		Point apply(Point p);
		void apply(Point* p);
		double det();  // le determinant
};

class Isometry  // une isometrie
{
	public:
	
	double theta, phi, psi;  // la rotation (theta autour de Ox et phi autour de Oy et psi autour de Oz)
	Vect3d translation, rotation;  // la translation et le vecteur rotation
	Matrix3d matrix;  // la matrice
	bool angles;  // true si on utilisa le premier constructeur, false sinon
	
		Isometry();
		Isometry(double Theta, double Phi, double Psi, Vect3d Translation);
		Isometry(Vect3d Rotation, Vect3d Translation);
		Vect3d apply(Vect3d v);
		Point apply(Point p);
		void apply(Point* p);
		void makeMatrix();
};

// la position et l'orientation du robot
class Position
{
	public :
	
	Vect3d position;
	Vect3d orientation;
	
		Position();
		Position(Vect3d Position, Vect3d Orientation);
};

// fonctions geometriques de base
Vect3d add(Vect3d a, Vect3d b);
Vect3d soust(Vect3d a, Vect3d b);
Vect3d mult(Vect3d a, double lambda);
double dotProduct(Vect3d a, Vect3d b);
Vect3d vectorProduct(Vect3d a, Vect3d b);
double det(Vect3d a, Vect3d b, Vect3d c);

// fonctions arithmetiques
double Random(double mini, double maxi);
double abs(double x);
double min(double a, double b);
double max(double a, double b);

// fonctions avancées
Isometry getBestTransformation(Isometry* t, long n, long p);
void mergeSort(double* t, long n);
void mergeSortParam(double* t, long* r, long n);
#endif
