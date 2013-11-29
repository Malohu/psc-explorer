#include <stdio.h>
#include <stdlib.h>

#include "../commons/pointcloudnode.h"
#include "../commons/pointcloud.h"

// construit un arbre a partir d'un point
PointCloudNode::PointCloudNode(Point p, Vect3d Center, double Size)
{
	point = new Point();
	*point = p;
	is_point = true;
	depth = 0;
	center = Center;
	size = Size;
	nb_points = 1;
	
	next_000 = NULL;
	next_001 = NULL;
	next_010 = NULL;
	next_011 = NULL;
	next_100 = NULL;
	next_101 = NULL;
	next_110 = NULL;
	next_111 = NULL;
}

PointCloudNode::PointCloudNode(Point p, double X_center, double Y_center, double Z_center, double Size)
{
	point = new Point(&p);
	is_point = true;
	depth = 0;
	center.x = X_center;
	center.y = Y_center;
	center.z = Z_center;
	size = Size;
	nb_points = 1;
	
	next_000 = NULL;
	next_001 = NULL;
	next_010 = NULL;
	next_011 = NULL;
	next_100 = NULL;
	next_101 = NULL;
	next_110 = NULL;
	next_111 = NULL;
}

// construit un arbre a partir de coordonnees
PointCloudNode::PointCloudNode(double X, double Y, double Z, Vect3d Center, double Size)
{
	point = new Point(X, Y, Z);
	is_point = true;
	depth = 0;
	center = Center;
	size = Size;
	nb_points = 1;
	
	next_000 = NULL;
	next_001 = NULL;
	next_010 = NULL;
	next_011 = NULL;
	next_100 = NULL;
	next_101 = NULL;
	next_110 = NULL;
	next_111 = NULL;
}

PointCloudNode::PointCloudNode(double X, double Y, double Z, double W, Vect3d Center, double Size)
{
	point = new Point(X, Y, Z, W);
	is_point = true;
	depth = 0;
	center = Center;
	size = Size;
	nb_points = 1;
	
	next_000 = NULL;
	next_001 = NULL;
	next_010 = NULL;
	next_011 = NULL;
	next_100 = NULL;
	next_101 = NULL;
	next_110 = NULL;
	next_111 = NULL;
}

PointCloudNode::PointCloudNode(double X, double Y, double Z, int R, int G, int B, Vect3d Center, double Size)
{
	point = new Point(X, Y, Z, R, G, B);
	is_point = true;
	depth = 0;
	center = Center;
	size = Size;
	nb_points = 1;
	
	next_000 = NULL;
	next_001 = NULL;
	next_010 = NULL;
	next_011 = NULL;
	next_100 = NULL;
	next_101 = NULL;
	next_110 = NULL;
	next_111 = NULL;
}

PointCloudNode::PointCloudNode(double X, double Y, double Z, double W, int R, int G, int B, Vect3d Center, double Size)
{
	point = new Point(X, Y, Z, W, R, G, B);
	is_point = true;
	depth = 0;
	center = Center;
	size = Size;
	nb_points = 1;
	
	next_000 = NULL;
	next_001 = NULL;
	next_010 = NULL;
	next_011 = NULL;
	next_100 = NULL;
	next_101 = NULL;
	next_110 = NULL;
	next_111 = NULL;
}

// ajoute un point et retourne le nombre de points du sous-arbre
long PointCloudNode::addPoint(Point p)
{//if  (depth%100 == 0) printf("coucou %ld\n", depth);
	if  (depth > 100) return nb_points;
	if  (is_point)
	{
		is_point = false;
		if  (point->x < center.x && point->y < center.y && point->z < center.z) { next_000 = new PointCloudNode(*point, center.x-size/4, center.y-size/4, center.z-size/4, size/2); next_000->depth = depth+1;}
		if  (point->x < center.x && point->y < center.y && point->z >= center.z) { next_001 = new PointCloudNode(*point, center.x-size/4, center.y-size/4, center.z+size/4, size/2); next_001->depth = depth+1;}
		if  (point->x < center.x && point->y >= center.y && point->z < center.z) { next_010 = new PointCloudNode(*point, center.x-size/4, center.y+size/4, center.z-size/4, size/2); next_010->depth = depth+1;}
		if  (point->x < center.x && point->y >= center.y && point->z >= center.z) { next_011 = new PointCloudNode(*point, center.x-size/4, center.y+size/4, center.z+size/4, size/2); next_011->depth = depth+1;}
		if  (point->x >= center.x && point->y < center.y && point->z < center.z) { next_100 = new PointCloudNode(*point, center.x+size/4, center.y-size/4, center.z-size/4, size/2); next_100->depth = depth+1;}
		if  (point->x >= center.x && point->y < center.y && point->z >= center.z) { next_101 = new PointCloudNode(*point, center.x+size/4, center.y-size/4, center.z+size/4, size/2); next_101->depth = depth+1;}
		if  (point->x >= center.x && point->y >= center.y && point->z < center.z) { next_110 = new PointCloudNode(*point, center.x+size/4, center.y+size/4, center.z-size/4, size/2); next_110->depth = depth+1;}
		if  (point->x >= center.x && point->y >= center.y && point->z >= center.z) { next_111 = new PointCloudNode(*point, center.x+size/4, center.y+size/4, center.z+size/4, size/2); next_111->depth = depth+1;}
		
		addPoint(p);
	}
	else
	{
		if  (p.x < center.x && p.y < center.y && p.z < center.z)
		{
			if  (next_000 == NULL) { next_000 = new PointCloudNode(p, center.x-size/4, center.y-size/4, center.z-size/4, size/2); next_000->depth = depth+1;}
			else next_000 -> addPoint(p);
		}
		if  (p.x < center.x && p.y < center.y && p.z >= center.z)
		{
			if  (next_001 == NULL) { next_001 = new PointCloudNode(p, center.x-size/4, center.y-size/4, center.z+size/4, size/2); next_001->depth = depth+1;}
			else next_001 -> addPoint(p);
		}
		if  (p.x < center.x && p.y >= center.y && p.z < center.z)
		{
			if  (next_010 == NULL) { next_010 = new PointCloudNode(p, center.x-size/4, center.y+size/4, center.z-size/4, size/2); next_010->depth = depth+1;}
			else next_010 -> addPoint(p);
		}
		if  (p.x < center.x && p.y >= center.y && p.z >= center.z)
		{
			if  (next_011 == NULL) { next_011 = new PointCloudNode(p, center.x-size/4, center.y+size/4, center.z+size/4, size/2); next_011->depth = depth+1;}
			else next_011 -> addPoint(p);
		}
		if  (p.x >= center.x && p.y < center.y && p.z < center.z)
		{
			if  (next_100 == NULL) { next_100 = new PointCloudNode(p, center.x+size/4, center.y-size/4, center.z-size/4, size/2); next_100->depth = depth+1;}
			else next_100 -> addPoint(p);
		}
		if  (p.x >= center.x && p.y < center.y && p.z >= center.z)
		{
			if  (next_101 == NULL) { next_101 = new PointCloudNode(p, center.x+size/4, center.y-size/4, center.z+size/4, size/2); next_101->depth = depth+1;}
			else next_101 -> addPoint(p);
		}
		if  (p.x >= center.x && p.y >= center.y && p.z < center.z)
		{
			if  (next_110 == NULL) { next_110 = new PointCloudNode(p, center.x+size/4, center.y+size/4, center.z-size/4, size/2); next_110->depth = depth+1;}
			else next_110 -> addPoint(p);
		}
		if  (p.x >= center.x && p.y >= center.y && p.z >= center.z)
		{
			if  (next_111 == NULL) { next_111 = new PointCloudNode(p, center.x+size/4, center.y+size/4, center.z+size/4, size/2); next_111->depth = depth+1;}
			else next_111 -> addPoint(p);
		}
		
		point -> merge(p);
		nb_points++;
	}
	
	return nb_points;
}

// recherche de point de l'arbre le plus proche de reference et stocke la distance dans "dist" et renvoie le nombre de noeuds parcourus
long PointCloudNode::closestPoint(double* dist, Point* closest, Point reference)
{
	long nb_noeuds = 1;
	if  (is_point)
	{
		if  (point->distance2(reference) < *dist)
		{
			*dist = point->distance2(reference);
			*closest = *point;
			return nb_noeuds;
		}
		else return nb_noeuds;
	}
	else
	{
		double tmp = size*SQRT_3/2 + sqrt(*dist);
		if  (reference.distance2(center) < tmp*tmp)
		{
			if  (next_000 != NULL) nb_noeuds += next_000 -> closestPoint(dist, closest, reference);
			if  (next_001 != NULL) nb_noeuds += next_001 -> closestPoint(dist, closest, reference);
			if  (next_010 != NULL) nb_noeuds += next_010 -> closestPoint(dist, closest, reference);
			if  (next_011 != NULL) nb_noeuds += next_011 -> closestPoint(dist, closest, reference);
			if  (next_100 != NULL) nb_noeuds += next_100 -> closestPoint(dist, closest, reference);
			if  (next_101 != NULL) nb_noeuds += next_101 -> closestPoint(dist, closest, reference);
			if  (next_110 != NULL) nb_noeuds += next_110 -> closestPoint(dist, closest, reference);
			if  (next_111 != NULL) nb_noeuds += next_111 -> closestPoint(dist, closest, reference);
			return nb_noeuds;
		}
		else return nb_noeuds;
	}
}

// recherche des point de l'arbre les plus proches de "reference" situés dans un rayon rmax et stocke les distance dans "dist" et renvoie le nombre de noeuds parcourus
long PointCloudNode::closestPoints(double* dist, Point* closest, Point reference, double rmax, long nb_closest_points, long* nb_found)
{
	long nb_noeuds = 1;
	if  (is_point)
	{
		double distance = point->distance2(reference);
		if  (distance < rmax*rmax)
		{
			for (long i = 0 ; i < *nb_found ; i++)
			{
				if  (distance < dist[i])
				{
					for (long j = nb_closest_points-1 ; j > i ; j--)
					{
						closest[j] = closest[j-1];
						dist[j] = dist[j-1];
					}
					dist[i] = distance;
					closest[i] = *point;
					if  (*nb_found < nb_closest_points) (*nb_found)++;
					return nb_noeuds;
				}
			}
			if  (*nb_found < nb_closest_points)
			{
				closest[*nb_found] = *point;
				dist[*nb_found] = distance;
				(*nb_found)++;
			}
		}
		return nb_noeuds;
	}
	else
	{
		//double tmp = size*SQRT_3/2 + sqrt(dist[nb_closest_points-1]);
		double tmp = reference.distance2(center) - 3.0*size*size/4.0 - dist[(*nb_found)-1];
		//double tmp2 = reference.distance2(center) - 3.0*size*size/4.0 - rmax*rmax;
		//if  (reference.distance2(center) < tmp*tmp)
		if  (*nb_found == 0 || (tmp < 0 || tmp*tmp < 3.0*size*size*dist[(*nb_found)-1]))// && (tmp2 < 0 || tmp2 < SQRT_3*size*rmax))
		{
			if  (next_000 != NULL) nb_noeuds += next_000 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_001 != NULL) nb_noeuds += next_001 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_010 != NULL) nb_noeuds += next_010 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_011 != NULL) nb_noeuds += next_011 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_100 != NULL) nb_noeuds += next_100 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_101 != NULL) nb_noeuds += next_101 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_110 != NULL) nb_noeuds += next_110 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			if  (next_111 != NULL) nb_noeuds += next_111 -> closestPoints(dist, closest, reference, rmax, nb_closest_points, nb_found);
			return nb_noeuds;
		}
		else return nb_noeuds;
	}
}

// copie le sous-arbre de current dans le nuage root
void PointCloudNode::copyRec(PointCloud* root)
{
	if  (is_point)
	{
		root -> addPoint(*point);
	}
	else
	{
		if  (next_000 != NULL) next_000 -> copyRec(root);
		if  (next_001 != NULL) next_001 -> copyRec(root);
		if  (next_010 != NULL) next_010 -> copyRec(root);
		if  (next_011 != NULL) next_011 -> copyRec(root);
		if  (next_100 != NULL) next_100 -> copyRec(root);
		if  (next_101 != NULL) next_101 -> copyRec(root);
		if  (next_110 != NULL) next_110 -> copyRec(root);
		if  (next_111 != NULL) next_111 -> copyRec(root);
	}
	return;
}

// applique une transformation a tout le nuage et met a jour la structure d'arbre
void PointCloudNode::applyTransformation(Isometry t, PointCloud* new_cloud)
{
	if  (is_point) new_cloud -> addPoint(t.apply(*point));
	else
	{
		if  (next_000 != NULL) { next_000 -> applyTransformation(t, new_cloud); delete next_000;}
		if  (next_001 != NULL) { next_001 -> applyTransformation(t, new_cloud); delete next_001;}
		if  (next_010 != NULL) { next_010 -> applyTransformation(t, new_cloud); delete next_010;}
		if  (next_011 != NULL) { next_011 -> applyTransformation(t, new_cloud); delete next_011;}
		if  (next_100 != NULL) { next_100 -> applyTransformation(t, new_cloud); delete next_100;}
		if  (next_101 != NULL) { next_101 -> applyTransformation(t, new_cloud); delete next_101;}
		if  (next_110 != NULL) { next_110 -> applyTransformation(t, new_cloud); delete next_110;}
		if  (next_111 != NULL) { next_111 -> applyTransformation(t, new_cloud); delete next_111;}
	}
	delete point;
	return;
}

// applique une transformation a tout le nuage et retourne une copie de cet arbre
void PointCloudNode::applyTransformationCopy(Isometry t, PointCloud* new_cloud)
{
	if  (is_point) new_cloud -> addPoint(t.apply(*point));
	else
	{
		if  (next_000 != NULL) { next_000 -> applyTransformationCopy(t, new_cloud);}
		if  (next_001 != NULL) { next_001 -> applyTransformationCopy(t, new_cloud);}
		if  (next_010 != NULL) { next_010 -> applyTransformationCopy(t, new_cloud);}
		if  (next_011 != NULL) { next_011 -> applyTransformationCopy(t, new_cloud);}
		if  (next_100 != NULL) { next_100 -> applyTransformationCopy(t, new_cloud);}
		if  (next_101 != NULL) { next_101 -> applyTransformationCopy(t, new_cloud);}
		if  (next_110 != NULL) { next_110 -> applyTransformationCopy(t, new_cloud);}
		if  (next_111 != NULL) { next_111 -> applyTransformationCopy(t, new_cloud);}
	}
	return;
}

// calcule la distance moyenne entre un point du nuage et son image par la transformation t
double PointCloudNode::errorAlignment(Isometry t)
{
	double error = 0;
	if  (is_point) error += point -> distance(t.apply(*point));
	else
	{
		if  (next_000 != NULL) error += next_000 -> errorAlignment(t);
		if  (next_001 != NULL) error += next_001 -> errorAlignment(t);
		if  (next_010 != NULL) error += next_010 -> errorAlignment(t);
		if  (next_011 != NULL) error += next_011 -> errorAlignment(t);
		if  (next_100 != NULL) error += next_100 -> errorAlignment(t);
		if  (next_101 != NULL) error += next_101 -> errorAlignment(t);
		if  (next_110 != NULL) error += next_110 -> errorAlignment(t);
		if  (next_111 != NULL) error += next_111 -> errorAlignment(t);
	}
	return error;
}

// calcule la distance moyenne entre un point du nuage et son image par la transformation s o t
double PointCloudNode::errorAlignment(Isometry t, Isometry s)
{
	double error = 0;
	if  (is_point) error += point -> distance(s.apply(t.apply(*point)));
	else
	{
		if  (next_000 != NULL) error += next_000 -> errorAlignment(t, s);
		if  (next_001 != NULL) error += next_001 -> errorAlignment(t, s);
		if  (next_010 != NULL) error += next_010 -> errorAlignment(t, s);
		if  (next_011 != NULL) error += next_011 -> errorAlignment(t, s);
		if  (next_100 != NULL) error += next_100 -> errorAlignment(t, s);
		if  (next_101 != NULL) error += next_101 -> errorAlignment(t, s);
		if  (next_110 != NULL) error += next_110 -> errorAlignment(t, s);
		if  (next_111 != NULL) error += next_111 -> errorAlignment(t, s);
	}
	return error;
}

void PointCloudNode::deleteNode()
{
	if  (!is_point)
	{
		if  (next_000 != NULL) { next_000 -> deleteNode(); delete next_000;}
		if  (next_001 != NULL) { next_001 -> deleteNode(); delete next_001;}
		if  (next_010 != NULL) { next_010 -> deleteNode(); delete next_010;}
		if  (next_011 != NULL) { next_011 -> deleteNode(); delete next_011;}
		if  (next_100 != NULL) { next_100 -> deleteNode(); delete next_100;}
		if  (next_101 != NULL) { next_101 -> deleteNode(); delete next_101;}
		if  (next_110 != NULL) { next_110 -> deleteNode(); delete next_110;}
		if  (next_111 != NULL) { next_111 -> deleteNode(); delete next_111;}
	}
	delete point;
	return;
}

// parcourt l'arbre en profondeur et calcule l'erreur par rapport au nuage "reference" et stocke toutes les erreurs dans le tableau "errors"
void PointCloudNode::errorFunctionRec(PointCloud* reference, long nb_closest_neighbours, long* nb_points_included, double* errors)
{
	if  (is_point)
	{
		errors[*nb_points_included] = 0;
		double dists_points[nb_closest_neighbours];
		for (long i = 0 ; i < nb_closest_neighbours ; i++) dists_points[i] = INFTY;
		reference -> closestPoints(dists_points, *point, nb_closest_neighbours);
		for (long i = 0 ; i < nb_closest_neighbours ; i++)
		{
			errors[*nb_points_included] += (dists_points[i])/**(dists_points[i]+0.1)*/;
		}
		(*nb_points_included)++;
	}
	else
	{
		if  (next_000 != NULL) next_000 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_001 != NULL) next_001 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_010 != NULL) next_010 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_011 != NULL) next_011 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_100 != NULL) next_100 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_101 != NULL) next_101 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_110 != NULL) next_110 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
		if  (next_111 != NULL) next_111 -> errorFunctionRec(reference, nb_closest_neighbours, nb_points_included, errors);
	}
	return;
}

// extrait un sous-nuage de points randoms
void PointCloudNode::sampleRandom(PointCloud* sample_cloud, long nb_points_sample)
{
	if  (is_point)
	{
		if  (nb_points_sample == 0) sample_cloud -> addPoint(*point);
		else if  (rand()%(nb_points_sample) == 0) sample_cloud -> addPoint(*point);
	}
	else
	{
		if  (next_000 != NULL) next_000 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_001 != NULL) next_001 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_010 != NULL) next_010 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_011 != NULL) next_011 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_100 != NULL) next_100 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_101 != NULL) next_101 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_110 != NULL) next_110 -> sampleRandom(sample_cloud, nb_points_sample);
		if  (next_111 != NULL) next_111 -> sampleRandom(sample_cloud, nb_points_sample);
	}
	return;
}

// extrait tous les points situés dans une certaine sphere
void PointCloudNode::sampleSphere(PointCloud* sample_cloud, Vect3d center_sphere, double radius)
{
	if  (is_point)
	{
		if  (point->distance(center_sphere) < radius) sample_cloud -> addPoint(*point);
	}
	else
	{
		if  (next_000 != NULL) next_000 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_001 != NULL) next_001 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_010 != NULL) next_010 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_011 != NULL) next_011 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_100 != NULL) next_100 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_101 != NULL) next_101 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_110 != NULL) next_110 -> sampleSphere(sample_cloud, center_sphere, radius);
		if  (next_111 != NULL) next_111 -> sampleSphere(sample_cloud, center_sphere, radius);
	}
	return;
}

// enregistre le nuage de points dans un fichier sous forme lisible par un humain et retourne la taille du fichier en octets (0 en cas d'erreur)
long PointCloudNode::saveChar(FILE* f)
{
	if  (is_point)
	{
		fprintf(f, "%ld  %lf %lf %lf  %lf  %d %d %d\n", point->id, point->x, point->y, point->z, point->w, point->r, point->g, point->b);
	}
	else
	{
		if  (next_000 != NULL) next_000 -> saveChar(f);
		if  (next_001 != NULL) next_001 -> saveChar(f);
		if  (next_010 != NULL) next_010 -> saveChar(f);
		if  (next_011 != NULL) next_011 -> saveChar(f);
		if  (next_100 != NULL) next_100 -> saveChar(f);
		if  (next_101 != NULL) next_101 -> saveChar(f);
		if  (next_110 != NULL) next_110 -> saveChar(f);
		if  (next_111 != NULL) next_111 -> saveChar(f);
	}
	return 0;
}


// vide le nuage courant dans le nuage pc
long PointCloudNode::mergeAll(PointCloud* pc)
{
	long new_points = 0;
	if  (is_point)
	{
		pc -> addPoint(*point);
		new_points++;
	}
	else
	{
		if  (next_000 != NULL) new_points += next_000 -> mergeAll(pc);
		if  (next_001 != NULL) new_points += next_001 -> mergeAll(pc);
		if  (next_010 != NULL) new_points += next_010 -> mergeAll(pc);
		if  (next_011 != NULL) new_points += next_011 -> mergeAll(pc);
		if  (next_100 != NULL) new_points += next_100 -> mergeAll(pc);
		if  (next_101 != NULL) new_points += next_101 -> mergeAll(pc);
		if  (next_110 != NULL) new_points += next_110 -> mergeAll(pc);
		if  (next_111 != NULL) new_points += next_111 -> mergeAll(pc);
	}
	return new_points;
}

// filtre les points trop loins du nuage de référence et ajoute les points gardés au nuage destination et retourne le nombre de points du sous nuage de destination créé
long PointCloudNode::referenceFilter(PointCloud* reference, PointCloud* destination, double threshold, long nb_closest_points)
{
	long added_points = 0;
	
	if  (is_point)
	{
		double distance[nb_closest_points];
		reference -> closestPoints(distance, point, nb_closest_points);  // recupere les distance aux k points les plus proches
		double medium_distance = 0;
		for (int i = 0 ; i < nb_closest_points ; i++) medium_distance += sqrt(distance[i]);
		medium_distance /= nb_closest_points;
		if  (medium_distance < threshold)
		{
			destination -> addPoint(*point);
			added_points++;
		}
	}
	else
	{
		if  (next_000 != NULL) added_points += next_000 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_001 != NULL) added_points += next_001 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_010 != NULL) added_points += next_010 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_011 != NULL) added_points += next_011 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_100 != NULL) added_points += next_100 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_101 != NULL) added_points += next_101 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_110 != NULL) added_points += next_110 -> referenceFilter(reference, destination, threshold, nb_closest_points);
		if  (next_111 != NULL) added_points += next_111 -> referenceFilter(reference, destination, threshold, nb_closest_points);
	}
	return added_points;
}

// convertit l'arbre en tableau
void PointCloudNode::toArray(Point* t, long* index)
{
	if  (is_point) t[(*index)++] = *point;
	else
	{
		if  (next_000 != NULL) next_000 -> toArray(t, index);
		if  (next_001 != NULL) next_001 -> toArray(t, index);
		if  (next_010 != NULL) next_010 -> toArray(t, index);
		if  (next_011 != NULL) next_011 -> toArray(t, index);
		if  (next_100 != NULL) next_100 -> toArray(t, index);
		if  (next_101 != NULL) next_101 -> toArray(t, index);
		if  (next_110 != NULL) next_110 -> toArray(t, index);
		if  (next_111 != NULL) next_111 -> toArray(t, index);
	}
	return;
}
