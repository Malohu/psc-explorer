#include "../commons/geometry.h"

/* fonctions geometriques */

Vect3d add(Vect3d a, Vect3d b)
{
	Vect3d v = {a.x + b.x, a.y + b.y, a.z + b.z};
	return v;
}

Vect3d soust(Vect3d a, Vect3d b)
{
	Vect3d v = {a.x - b.x, a.y - b.y, a.z - b.z};
	return v;
}

Vect3d mult(Vect3d a, double lambda)
{
	Vect3d v = {lambda * a.x, lambda * a.y, lambda * a.z};
	return v;
}

double dotProduct(Vect3d a, Vect3d b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Vect3d vectorProduct(Vect3d a, Vect3d b)
{
	Vect3d v = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
	return v;
}

double det(Vect3d a, Vect3d b, Vect3d c)
{
	return (a.x*b.y*c.z + b.x*c.y*a.z + c.x*a.y*b.z - a.z*b.y*c.x - b.z*c.y*a.x - c.z*a.y*b.x);
}

/* fonctions arithmetiques */

double Random(double mini, double maxi)
{
	return ((double)(rand()%10000+10000*(rand()%10000))) / 100000000.0 * (maxi-mini) + mini;
}

double abs(double x)
{
	if  (x > 0) return x;
	else return (-x);
}

double min(double a, double b)
{
	if  (a < b) return a;
	else return b;
}

double max(double a, double b)
{
	if  (a > b) return a;
	else return b;
}

/* fonctions avancees */

// calcule la meilleure isometrie parmi les n passees en entree, en faisant des moyennes;
Isometry getBestTransformation(Isometry* t, long n, long p)
{
	for (long i = 0 ; i < n-p+1 ; i++)
	{
		Isometry m(0,0,0, {0,0,0});
		for (int j = 0 ; j < n-i ; j++)
		{
			m.theta += t[j].theta;
			m.phi += t[j].phi;
			m.psi += t[j].psi;
			m.translation = add(m.translation, t[j].translation);
		}
		m.theta /= (n-i);
		m.phi /= (n-i);
		m.psi /= (n-i);
		m.translation.x /= (n-i);
		m.translation.y /= (n-i);
		m.translation.z /= (n-i);
		
		if  (i == n-p) return m;  // condition d'arret
		
		m.rotation.x = m.theta;
		m.rotation.y = m.phi;
		m.rotation.z = m.psi;
		double rotation_norm = m.rotation.norm();
		double translation_norm = m.translation.norm();
		if  (rotation_norm != 0) m.rotation.normalize();
		else rotation_norm = 1;
		if  (translation_norm != 0) m.translation.normalize();
		else translation_norm = 1;
		Isometry normalized[n-i];
		for (int j = 0 ; j < n-i ; j++)
		{
			normalized[j].rotation.x = t[j].theta / rotation_norm;
			normalized[j].rotation.y = t[j].phi / rotation_norm;
			normalized[j].rotation.z = t[j].psi / rotation_norm;
			normalized[j].translation.x = t[j].translation.x / translation_norm;
			normalized[j].translation.y = t[j].translation.y / translation_norm;
			normalized[j].translation.z = t[j].translation.z / translation_norm;
		}
		
		double max_dist = 0;
		long index_max = 0;
		for (int j = 0 ; j < n-i ; j++)
		{
			double dist = m.rotation.distance(normalized[j].rotation) + m.translation.distance(normalized[j].translation);
			if  (dist > max_dist)
			{
				max_dist = dist;
				index_max = j;
			}
		}
		t[index_max] = t[n-i-1];
	}
	return t[0];
}

/* la classe Vect3d */

// la norme
double Vect3d::norm()
{
	return sqrt(x*x + y*y + z*z);
}

// retourne une copie normalisee du vecteur
Vect3d Vect3d::normalizeCopy()
{
	double n = norm();
	Vect3d result = {x/n, y/n, z/n};
	return result;
}

// normalise le vecteur
void Vect3d::normalize()
{
	double n = norm();
	x /= n;
	y /= n;
	z /= n;
	return;
}

// distance au vecteur v
double Vect3d::distance(Vect3d v)
{
	return sqrt((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
}

/* la classe Matrix3d */

// constructeur
Matrix3d::Matrix3d()
{
	
}

Matrix3d::Matrix3d(double M11, double M12, double M13, double M21, double M22, double M23, double M31, double M32, double M33, Vect3d Translation)
{
	m11 = M11;
	m12 = M12;
	m13 = M13;
	m21 = M21;
	m22 = M22;
	m23 = M23;
	m31 = M31;
	m32 = M32;
	m33 = M33;
	translation = Translation;
}

// applique a un vecteur et retourne une autre instance
Vect3d Matrix3d::apply(Vect3d v)
{
	Vect3d v2 = {m11*v.x + m12*v.y + m13 * v.z, m21*v.x + m22*v.y + m23*v.z, m31*v.x + m32*v.y + m33*v.z};
	return add(v2, translation);
}

// applique a un point et retourne une autre instance de ce point
Point Matrix3d::apply(Point p)
{
	Point res = p;
	Vect3d v = {p.x, p.y, p.z};
	v = apply(v);
	res.x = v.x;
	res.y = v.y;
	res.z = v.z;
	
	return res;
}

// applique a un point
void Matrix3d::apply(Point* p)
{
	Vect3d v = {p->x, p->y, p->z};
	v = apply(v);
	p->x = v.x;
	p->y = v.y;
	p->z = v.z;
	
	return;
}
// determinant d'une matrice
double Matrix3d::det()
{
	return (m11*m22*m33 + m12*m23*m31 + m13*m21*m32 - m31*m22*m13 - m21*m12*m33 - m11*m32*m23);
}

/* la classe isometry */

Isometry::Isometry(double Theta, double Phi, double Psi, Vect3d Translation)
{
	theta = Theta;
	phi = Phi;
	psi = Psi;
	translation = Translation;
	angles = true;
}

Isometry::Isometry(Vect3d Rotation, Vect3d Translation)
{
	rotation = Rotation;
	translation = Translation;
	angles = false;
}

Isometry::Isometry()
{
}

Vect3d Isometry::apply(Vect3d v)
{
	return matrix.apply(v);
}

Point Isometry::apply(Point p)
{
	return matrix.apply(p);
}

void Isometry::apply(Point* p)
{
	matrix.apply(p);
	return;
}

void Isometry::makeMatrix()
{
	matrix.translation = translation;
	if  (angles)
	{
		matrix.m11 = cos(phi)*cos(psi);
		matrix.m12 = -cos(phi)*sin(psi);
		matrix.m13 = -sin(phi);
		matrix.m21 = cos(theta)*sin(psi)-sin(theta)*sin(phi)*cos(psi);
		matrix.m22 = cos(theta)*cos(psi)+sin(theta)*sin(phi)*sin(psi);
		matrix.m23 = -sin(theta)*cos(phi);
		matrix.m31 = cos(theta)*sin(phi)*cos(psi)+sin(theta)*sin(psi);
		matrix.m32 = sin(theta)*cos(psi)-cos(theta)*sin(phi)*sin(psi);
		matrix.m33 = cos(theta)*cos(phi);
	}
	else
	{
		matrix.m11 = 0;
		matrix.m12 = -rotation.z;
		matrix.m13 = rotation.y;
		matrix.m21 = rotation.z;
		matrix.m22 = 0;
		matrix.m23 = -rotation.x;
		matrix.m31 = -rotation.y;
		matrix.m32 = rotation.x;
		matrix.m33 = 0;
	}
	
	return;
}

/* la classe position / orientation */
Position::Position(Vect3d Position, Vect3d Orientation)
{
	position = Position;
	orientation = Orientation;
}

Position::Position()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	orientation.x = 1.0;
	orientation.y = 0;
	orientation.z = 0;
}

// tri fusion
void mergeSort(double* t, long n)
{
	if  (n <= 1) return;
	double* g = new double[n/2+1], *d = new double[n/2+1];
	for (long i = 0;  i < n/2 ; i++) g[i] = t[i];
	for (long i = n/2 ; i < n ; i++) d[i-n/2] = t[i];
	mergeSort(g, n/2);
	mergeSort(d, n-n/2);
	long i = 0, j = 0, k = 0;
	while (i < n/2 && j < n-n/2)
	{
		if  (g[i] < d[j]) t[k++] = g[i++];
		else t[k++] = d[j++];
	}
	while (i < n/2) t[k++] = g[i++];
	while (j < n-n/2) t[k++] = d[j++];
	delete g;
	delete d;
	
	return;
}

// tri fusion a parametre
void mergeSortParam(double* t, long* r, long n)
{
	if  (n <= 1) return;
	double* g = new double[n/2+1], *d = new double[n/2+1];
	long* gr = new long[n/2+1], *dr = new long[n/2+1];
	for (long i = 0;  i < n/2 ; i++) { g[i] = t[i]; gr[i] = r[i]; }
	for (long i = n/2 ; i < n ; i++) { d[i-n/2] = t[i]; dr[i-n/2] = r[i]; }
	mergeSortParam(g, gr, n/2);
	mergeSortParam(d, dr, n-n/2);
	long i = 0, j = 0, k = 0;
	while (i < n/2 && j < n-n/2)
	{
		if  (g[i] < d[j]) { r[k] = gr[i]; t[k++] = g[i++]; }
		else { r[k] = dr[j]; t[k++] = d[j++]; }
	}
	while (i < n/2) { r[k] = gr[i]; t[k++] = g[i++]; }
	while (j < n-n/2) { r[k] = dr[j]; t[k++] = d[j++]; }
	delete g;
	delete d;
	delete gr;
	delete dr;
	
	return;
}

