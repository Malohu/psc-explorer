#include "../commons/point.h"

long current_id_points = 0;

// les constructeurs
Point::Point()
{
	
}

Point::Point(Point* p)
{
	x = p->x;
	y = p->y;
	z = p->z;
	w = p->w;
	r = p->r;
	g = p->g;
	b = p->b;
	id = p->id;
}

Point::Point(double X, double Y, double Z)
{
	x = X;
	y = Y;
	z = Z;
	w = 1;
	r = 255;
	g = 255;
	b = 255;
	id = current_id_points++;
}

Point::Point(double X, double Y, double Z, double W)
{
	x = X;
	y = Y;
	z = Z;
	w = W;
	r = 255;
	g = 255;
	b = 255;
	id = current_id_points++;
}

Point::Point(double X, double Y, double Z, unsigned short R, unsigned short G, unsigned short B)
{
	x = X;
	y = Y;
	z = Z;
	r = R;
	g = G;
	b = B;
	w = 1;
	id = current_id_points++;
}

Point::Point(double X, double Y, double Z, double W, unsigned short R, unsigned short G, unsigned short B)
{
	x = X;
	y = Y;
	z = Z;
	r = R;
	g = G;
	b = B;
	w = W;
	id = current_id_points++;
}

// fusionne le point avec un autre
void Point::merge(Point p)
{
	double t = p.w / (w + p.w);
	x = (1-t) * x + t * p.x;
	y = (1-t) * y + t * p.y;
	z = (1-t) * z + t * p.z;
	r = (unsigned short)((1-t) * r + t * p.r);
	g = (unsigned short)((1-t) * g + t * p.g);
	b = (unsigned short)((1-t) * b + t * p.b);
	w += p.w;
	
	return;
}

// retourne la distance avec le point p
double Point::distance(Point p)
{
	return sqrt((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z));
}

// retourne la distance au carre avec le point p
double Point::distance2(Point p)
{
	return ((x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z));
}

// retourne la distance au carre au carre avec le point p
double Point::distance4(Point p)
{
	double tmp = (x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) + (z-p.z)*(z-p.z);
	return (tmp * tmp);
}

// retourne la distance avec le "point" v
double Point::distance(Vect3d v)
{
	return sqrt((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z));
}

// retourne la distance au carre avec le "point" v
double Point::distance2(Vect3d v)
{
	return ((x-v.x)*(x-v.x) + (y-v.y)*(y-v.y) + (z-v.z)*(z-v.z));
}
