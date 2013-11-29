#include <stdio.h>
#include <stdlib.h>

#include "../commons/pointcloud.h"
#include "../commons/error.h"
#include "../commons/benchmark.h"

PointCloud::PointCloud()
{
	
}

PointCloud::PointCloud(double Epsilon, double Size)
{
	epsilon_pruning = Epsilon;
	nb_points = 0;
	root = NULL;
	size = Size;
	Vect3d v = {0,0,0};
	center = v;
}

// ajoute un point et renvoie le nombre de points du nuage
long PointCloud::addPoint(Point p)
{
	if  (root == NULL)  root = new PointCloudNode(p, center, size);
	else nb_points = root -> addPoint(p);
	
	return nb_points;
}

void PointCloud::setEpsilon(double e)
{
	epsilon_pruning = e;
	return;
}


// vide le nuage pc dans le nuage courant (et detruit le nuage pc)
long PointCloud::mergeCloud(PointCloud* pc)
{
	if  (pc -> root == NULL) return nb_points;
	if  (root == NULL)
	{
		root = pc -> root;
		nb_points = pc -> nb_points;
		return nb_points;
	}
	pc -> root -> mergeAll(this);
	pc -> deleteCloud();
	return nb_points;
}

// enregistre le nuage de points dans un fichier sous forme lisible par un humain et retourne la taille du fichier en octets (0 en cas d'erreur)
long PointCloud::saveChar(char* file_name)
{
	FILE *f = fopen(file_name, "w+t");
	if  (f == NULL) return 0;
	
	fprintf(f, "%ld\n%lf\n%lf\n", nb_points, size, epsilon_pruning);
	fprintf(f, "%lf %lf %lf\n", center.x, center.y, center.z);
	
	if  (root != NULL) root -> saveChar(f);
	
	fclose(f);
	
	return nb_points;
}

// constructeur qui charge un nuage de points a partir d'un fichier lisible par un humain
PointCloud::PointCloud(char* file_name, double Epsilon, double Size)
{
	epsilon_pruning = Epsilon;
	nb_points = 0;
	root = NULL;
	size = Size;
	Vect3d v = {0,0,0};
	center = v;
	
	FILE *f = fopen(file_name, "rt");
	if  (f != NULL)
	{
		long tmp_nb_points;
		Point p;
		fscanf(f, "%ld\n%lf\n%lf\n%lf %lf %lf\n", &tmp_nb_points, &size, &epsilon_pruning, &center.x, &center.y, &center.z);
		for (long i = 0 ; i < tmp_nb_points ; i++)
		{
			fscanf(f, "%ld  %lf %lf %lf  %lf  %d %d %d\n", &p.id, &p.x, &p.y, &p.z, &p.w, &p.r, &p.g, &p.b);
			addPoint(p);
		}
		fclose(f);
	}
}

// renvoie de point de l'arbre le plus proche de reference et stocke la distance dans dist
Point PointCloud::closestPoint(double* dist, Point reference)
{
	Point start;
	*dist = INFTY;
	if  (root == NULL) return start;
	else
	{
		long nb_noeuds = root -> closestPoint(dist, &start, reference);
		//printf("Nombre de noeuds parcourus : %ld\n", nb_noeuds);
		return start;
	}
}

// renvoie les points de l'arbre les plus proches de reference et stocke les distances dans dist
void PointCloud::closestPoints(double* dist, Point reference, long nb_closest_points)
{
	Point start[nb_closest_points];
	if  (root == NULL) return;
	else
	{
		long nb_found_points = 0;
		for (long i = 0 ; i < nb_closest_points ; i++) dist[i] = INFTY;
		long nb_noeuds = root -> closestPoints(dist, start, reference, INFTY, nb_closest_points, &nb_found_points);
		if  (nb_found_points < nb_closest_points)
		{
			error("Not enough points found in closestPoints function. Risk of crash...");
			printf("nb_found = %ld   nb_closest = %ld\n", nb_found_points, nb_closest_points);
		}
		//printf("Nombre de noeuds parcourus : %ld\n", nb_noeuds);
		return;
	}
}

// renvoie tous les points situes a une distance inferieure a rmax de reference (dans la limite de max_points) et retourne le nombre de points trouvés
long PointCloud::nClosestPoints(double* dist, Point* points, Point reference, double rmax, long max_points)
{
	if  (root == NULL) return 0;
	else
	{
		long result = 0;
		long nb_nodes = root -> closestPoints(dist, points, reference, rmax, max_points, &result);
		return result;
	}
}

// renvoie de point de l'arbre le plus proche de reference
Point PointCloud::closestPoint(Point reference)
{
	Point start;
	double dist = INFTY;
	if  (root == NULL) return start;
	else
	{
		long nb_noeuds = root -> closestPoint(&dist, &start, reference);
		return start;
	}
}

// renvoie une copie integrale du nuage de points
PointCloud PointCloud::copy()
{
	PointCloud res(epsilon_pruning, size);
	if  (root == NULL) return res;
	else
	{
		root -> copyRec(&res);
		return res;
	}
}

// applique une transformation a tout le nuage
void PointCloud::applyTransformation(Isometry t)
{
	if  (root != NULL)
	{
		PointCloudNode* old_cloud = root;
		root = NULL;
		nb_points = 0;
		old_cloud -> applyTransformation(t, this);
		delete old_cloud;
	}
	return;
}

// applique une transformation a tout le nuage et renvoie une copie dudit nuage
PointCloud PointCloud::applyTransformationCopy(Isometry t)
{
	PointCloud new_cloud(epsilon_pruning, size);
	if  (root != NULL) root -> applyTransformationCopy(t, &new_cloud);
	return new_cloud;
}

// calcule la distance moyenne entre un point du nuage et son image par la transformation t
double PointCloud::errorAlignment(Isometry t)
{
	if  (root == NULL) return INFTY;
	return (root -> errorAlignment(t)) / nb_points;
}

// calcule la distance moyenne entre un point du nuage et son image par la transformation s o t
double PointCloud::errorAlignment(Isometry t, Isometry s)
{
	if  (root == NULL) return INFTY;
	return (root -> errorAlignment(t, s)) / nb_points;
}

void PointCloud::deleteCloud()
{
	if (root != NULL)
	{
		root -> deleteNode();
		delete root;
	}
	nb_points = 0;
	return;
}

// retourne un sous-nuage de points choisi aléatoirement
PointCloud PointCloud::sampleRandom(long nb_points_sample)
{
	PointCloud result(epsilon_pruning, size);
	if  (root != NULL)
	{
		if  (nb_points_sample == 0) root -> sampleRandom(&result, 1);  // pas d'échantillonage
		else if  (nb_points / nb_points_sample == 0 )root -> sampleRandom(&result, 1);
		else root -> sampleRandom(&result, nb_points / nb_points_sample);
	}
	return result;
}

// retourne un sous-nuage composé des points situés dans une certaine sphère
PointCloud PointCloud::sampleSphere(Vect3d sphere_center, double radius)
{
	PointCloud result(epsilon_pruning, size);
	if  (root != NULL) root -> sampleSphere(&result, sphere_center, radius);
	return result;
}

// parcourt l'arbre en profondeur et calcule l'erreur par rapport au nuage "reference"
double PointCloud::errorFunction(PointCloud* reference, long nb_closest_neighbours, double max_dist)
{
	if  (root == NULL) return INFTY;
	long nb_points_included = 0;
	double* errors = new double[nb_points];
	root -> errorFunctionRec(reference, nb_closest_neighbours, &nb_points_included, errors);
	mergeSort(errors, nb_points_included);
	double error = 0;
	for (long i = 0 ; i < nb_points*85/100 ; i++) error += errors[i];
	return error;
}

/*// filtre les points trop excentrés et retourne le nombre de points supprimés
long gaussianFilter(double threshold)
{
	if  (root == NULL) return 0;
	
}*/

// filtre les points trop loins du nuage de référence et retourne le nuage épuré
PointCloud PointCloud::referenceFilter(PointCloud* reference, double threshold, long nb_closest_points)
{
	PointCloud pc(epsilon_pruning, size);
	if  (root == NULL) return pc;
	root -> referenceFilter(reference, &pc, threshold, nb_closest_points);
	return pc;
}

// convertit l'arbre en tableau
Point* PointCloud::toArray()
{
	if  (root == NULL) return NULL;
	Point* p = new Point[nb_points];
	long index = 0;
	root -> toArray(p, &index);
	return p;
}
long cpr = 0;
// calcule une caracteristique geometrique en un point p du nuage
double PointCloud::integralFeature(Point p, double rmin, double rmax, long nb_steps, long max_points)
{
	Point closest_points[max_points+1];
	double dist[max_points+1];
	long nb_closest_points = 0;
	
	double step = 0.01, accuracy = 0.05;
	double max_iter = 40;
	
	nb_closest_points = nClosestPoints(dist, closest_points, p, rmax, max_points+1);
	long nb_closest_points2 = nClosestPoints(dist, closest_points, p, rmax*2, max_points+1);
	return (((double)nb_closest_points2) / ((double)nb_closest_points));
	
	do{
	nb_closest_points = nClosestPoints(dist, closest_points, p, INFTY, max_points+1);
	if  (nb_closest_points < 4)
	{
		rmax *= 1.5;
		info("Not enough points to compute geometric feature. Retrying with larger rmax...");
		cpr++;
	}
	}while (nb_closest_points < 4);
	printf("%ld points trouves a proximite\n", nb_closest_points-1);
	
	// calcule le plan tangent a la surface au point p
	Vect3d tp = tangentPlane(p,closest_points+1, nb_closest_points-1, max_iter, step, accuracy);
	printf("%Tangent plane : (%lf, %lf, %lf)\n", tp.x, tp.y, tp.z);
	
	Vect3d new_coord2[nb_closest_points-1], new_coord[nb_closest_points-1];
	for (long i = 0 ; i < nb_closest_points-1 ; i++)
	{
		new_coord2[i].x = closest_points[i+1].x - closest_points[0].x;
		new_coord2[i].y = closest_points[i+1].y - closest_points[0].y;
		new_coord2[i].z = closest_points[i+1].z - closest_points[0].z;
	}
	// calcule les vecteurs de base dans la nouvelle base (X et Y sont dans le plan tangent)
	Vect3d Z = tp;
	Z.normalize();
	Vect3d X = soust(new_coord2[0], mult(Z, dotProduct(new_coord2[0], Z)));
	X.normalize();
	Vect3d Y = vectorProduct(Z,X);
	Y.normalize();
	
	for (long i = 0 ; i < nb_closest_points-1 ; i++)
	{
		new_coord[i].x = dotProduct(new_coord2[i], X);
		new_coord[i].y = dotProduct(new_coord2[i], Y);
		new_coord[i].z = dotProduct(new_coord2[i], Z);
	}
	
	Vect3d hess = hessian(new_coord, nb_closest_points-1, max_iter, step, accuracy);
	
	printf("Hessian : a = %lf, b = %lf, c = %lf\n", hess.x, hess.y, hess.z);
	
	double theta = atan(2*hess.z/(hess.x-hess.y))/2;
	double c = cos(theta), s = sin(theta);
	double A = hess.x*c*c + hess.y*s*s + 2*hess.z*c*s;
	double B = hess.x*s*s + hess.y*c*c - 2*hess.z*c*s;
	
	printf("A = %lf, B = %lf, rmax = %lf\n", A, B, rmax);
	
	double R2 = min(abs(1/(2*A)), abs(1/(2*B)));
	//printf(" - %lf\n", max(abs(1/(2*A)), abs(1/(2*B))));
	//return (2/3*Pi*rmax*rmax*rmax - Pi/(4*R2)*rmax*rmax*rmax*rmax) / (4/3*Pi*rmax*rmax*rmax);
	return R2;//(3/16*rmax/R2);
}
