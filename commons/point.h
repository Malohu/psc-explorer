#ifndef POINT_H
#define POINT_H

struct Vect3d;

class Point
{
	public :
	
	double x, y, z;
	double w;
	long id;
	unsigned short r, g, b;

		Point();
		Point(Point* p);
		Point(double X, double Y, double Z);
		Point(double X, double Y, double Z, double W);
		Point(double X, double Y, double Z, unsigned short R, unsigned short G, unsigned short B);
		Point(double X, double Y, double Z, double W, unsigned short R, unsigned short G, unsigned short B);
		void merge(Point p);  // fusionne avec un autre point
		double distance(Point p);  // retourne la distance avec le point p
		double distance(Vect3d v);  // retourne la distance avec le "point" v
		double distance2(Point p);  // retourne la distance au carre avec le point p
		double distance2(Vect3d v);  // retourne la distance au carre avec le "point" v
		double distance4(Point p);  // retourne la distance au carre au carre avec le point p
};

#include "geometry.h"

#endif
