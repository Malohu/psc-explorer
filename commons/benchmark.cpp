#include <stdio.h>
#include <stdlib.h>

#include "benchmark.h"
#include <math.h>

// cree un tableau de n points randoms dans le cube de cote size centré en 0
Point* randomPoints(long n, double size)
{
	Point* p = new Point[n];
	for (long i = 0 ; i < n/3 ; i++)
	{
		p[i].x = Random(-size/2, size/2);
		p[i].y = Random(-size/2, size/2);
		p[i].z = Random(-size/2, size/2)/120;
		p[i].w = 1;
		p[i].r = 255;
		p[i].g = 255;
		p[i].b = 255;
		p[i].id = i;
	}
	for (long i = n/3 ; i < 2*n/3 ; i++)
	{
		p[i].x = Random(-size/2, size/2)/120;
		p[i].y = Random(-size/2, size/2);
		p[i].z = Random(-size/2, size/2)*2+size;
		p[i].w = 1;
		p[i].r = 255;
		p[i].g = 255;
		p[i].b = 255;
		p[i].id = i;
	}
	
	for (long i = 2*n/3 ; i < n ; i++)
	{
		p[i].x = Random(0, size/2);
		p[i].y = Random(-size/2, size/2)/120+size/2;
		p[i].z = Random(-size/2, size/2)+size/2;
		p[i].w = 1;
		p[i].r = 255;
		p[i].g = 255;
		p[i].b = 255;
		p[i].id = i;
	}
	
	return p;
}

// decoupe un tableau de points comme il faut
void splitX(Point* p, long n, Point** p1, Point** p2, long* n1, long* n2, double start, double stop)
{
	*p1 = new Point[n];
	*p2 = new Point[n];
	long j = 0, k = 0;
	
	for (long i = 0 ; i < n ; i++)
	{
		if  (p[i].x < start)  (*p1)[j++] = p[i];  // si a gauche de la premiere barriere
		else if  (p[i].x > stop)  (*p2)[k++] = p[i];  // si a droite de la seconde barriere
		else  // si entre les deux barrieres
		{
			if  (rand()%2 == 0) (*p1)[j++] = p[i];
			else (*p2)[k++] = p[i];
		}
	}
	*n1 = j;
	*n2 = k;
	
	return;
}

// decoupe un tableau de points comme il faut
void splitX(Point* p, long n, PointCloud* t1, PointCloud* t2, double start, double stop)
{
	for (long i = 0 ; i < n ; i++)
	{
		if  (p[i].x < start)  t1 -> addPoint(p[i]);  // si a gauche de la premiere barriere
		else if  (p[i].x > stop)  t2 -> addPoint(p[i]);  // si a droite de la seconde barriere
		else  // si entre les deux barrieres
		{
			if  (rand()%2 == 0) t2 -> addPoint(p[i]);
			else t1 -> addPoint(p[i]);
		}
	}
	
	return;
}

// calcule l'ecart quadratique moyen entre les points de p2 et leur position de référence dans p
double quadratic(Point* p, Point* p2, long n2)
{
	double sum = 0;
	for (long i = 0 ; i < n2 ; i++)
	{
		sum += p[i].distance(p2[i]);
	}
	return (sum / n2);
}

// fonction d'erreur
double errorFunction(Point* p1, Point* p2, long n1, long n2)
{
	double error = 0.0;
	long nb_closest_points = 50;
	
	for (long i = 0 ; i < n2 ; i++)
	{
		double min_dist[nb_closest_points];
		for (long k = 0 ; k < nb_closest_points ; k++) min_dist[k] = INFTY;
		for (long j = 0 ; j < n1 ; j++)
		{
			double d = p1[j].distance2(p2[i]);
			for (long k = 0 ; k < nb_closest_points ; k++)
			{
				if  (d < min_dist[k])
				{
					for (long l = nb_closest_points - 1 ; l > k ; l--) min_dist[l] = min_dist[l-1];
					min_dist[k] = d;
					break;
				}
			}
		}
		for (long k = 0 ; k < nb_closest_points ; k++) error += min_dist[k];
	}
	
	for (long i = 0 ; i < n1 ; i++)
	{
		double min_dist[nb_closest_points];
		for (long k = 0 ; k < nb_closest_points ; k++) min_dist[k] = INFTY;
		for (long j = 0 ; j < n2 ; j++)
		{
			double d = p2[j].distance2(p1[i]);
			for (long k = 0 ; k < nb_closest_points ; k++)
			{
				if  (d < min_dist[k])
				{
					for (long l = nb_closest_points - 1 ; l > k ; l--) min_dist[l] = min_dist[l-1];
					min_dist[k] = d;
					break;
				}
			}
		}
		for (long k = 0 ; k < nb_closest_points ; k++) error += min_dist[k];
	}
	
	return error;
}

// fonction d'erreur avec des nuages (arbres)
double errorFunction(PointCloud c1, PointCloud c2, double max_dist)
{
	return (/*c1.errorFunction(&c2, 5) +*/ c2.errorFunction(&c1, 5, max_dist));
}

/*void* errorFunctionThread(void* p_data);
PointCloud p2_transforme_threads[MAX_THREADS];
long nb_running_threads = 0;*/

// algorithme du gradient sur l'ensemble des transformations possibles
Isometry gradientAlgo(PointCloud p1, PointCloud p2, long max_iter, double step, double accuracy, long algo)
{
	Isometry transformation(0, 0, 0, {0,0,0});
	Isometry best(0,0,0,{0,0,0});
	PointCloud p2_transforme;
	double error = INFTY;
	double dx_translation, dy_translation, dz_translation, d_theta, d_phi, d_psi, norme_gradient;
	double first_diff = INFTY;
	double second_diff = 1.0, df_min, df_max;
	double error_mini = INFTY;
	double step_translation = step, step_rotation = step;
	double max_dist = INFTY;
	
	if  (algo != FAST)
	{	
		for (long i = 0 ; i < max_iter ; i++)
		{
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			error = errorFunction(p1, p2_transforme, max_dist);  // calcule la valeur de l'erreur au point considere
			p2_transforme.deleteCloud();
		
			// calcule la derivee de l'erreur par rapport a translation.x;
			transformation.translation.x += step_translation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.translation.x -= step_translation;
			dx_translation = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a translation.y;
			transformation.translation.y += step_translation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.translation.y -= step_translation;
			dy_translation = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a translation.z;
			transformation.translation.z += step_translation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.translation.z -= step_translation;
			dz_translation = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a theta;
			transformation.theta += step_rotation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.theta -= step_rotation;
			d_theta = (errorFunction(p1, p2_transforme, max_dist) - error) / step_rotation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a phi;
			transformation.phi += step_rotation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.phi -= step_rotation;
			d_phi = (errorFunction(p1, p2_transforme, max_dist) - error) / step_rotation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a psi;
			transformation.psi += step_rotation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.psi -= step_rotation;
			d_psi = (errorFunction(p1, p2_transforme, max_dist) - error, max_dist) / step_rotation;
			p2_transforme.deleteCloud();
		
			// calcule la norme du gradient
			norme_gradient = sqrt(dx_translation * dx_translation + dy_translation * dy_translation + dz_translation * dz_translation + d_theta * d_theta + d_phi * d_phi + d_psi * d_psi);
			
			// se deplace dans l'espace des parametres
			transformation.translation.x -= dx_translation / norme_gradient * step_translation;
			transformation.translation.y -= dy_translation / norme_gradient * step_translation;
			transformation.translation.z -= dz_translation / norme_gradient * step_translation;
			transformation.theta -= d_theta / norme_gradient * step_rotation;
			transformation.phi -= d_phi / norme_gradient * step_rotation;
			transformation.psi -= d_psi / norme_gradient * step_rotation;
			
			printf("Iteration %ld, Erreur totale : %lf, Translation : (%lf, %lf, %lf), (%lf, %lf, %lf)\n", i, error, transformation.translation.x, transformation.translation.y, transformation.translation.z, transformation.theta, transformation.phi, transformation.psi);
			
			if  (error < error_mini)
			{
				error_mini = error;
				best = transformation;
			}
		}
	}
	else
	{
		for (long i = 0 ; i < max_iter && abs(first_diff / second_diff) > accuracy ; i++)
		{
			//if  (i > 0) {step_translation = min(step,abs(first_diff/second_diff)/20); step_rotation = step_translation;}
			max_dist = INFTY;//max(error*100, accuracy*accuracy*10);
		
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			error = errorFunction(p1, p2_transforme, max_dist);  // calcule la valeur de l'erreur au point considere
			p2_transforme.deleteCloud();
		
			// calcule la derivee de l'erreur par rapport a translation.x;
			transformation.translation.x += step_translation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.translation.x -= step_translation;
			dx_translation = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a translation.y;
			transformation.translation.y += step_translation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.translation.y -= step_translation;
			dy_translation = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a translation.z;
			transformation.translation.z += step_translation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.translation.z -= step_translation;
			dz_translation = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a theta;
			transformation.theta += step_rotation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.theta -= step_rotation;
			d_theta = (errorFunction(p1, p2_transforme, max_dist) - error) / step_rotation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a phi;
			transformation.phi += step_rotation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.phi -= step_rotation;
			d_phi = (errorFunction(p1, p2_transforme, max_dist) - error) / step_rotation;
			p2_transforme.deleteCloud();
			
			// calcule la derivee de l'erreur par rapport a psi;
			transformation.psi += step_rotation;
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			transformation.psi -= step_rotation;
			d_psi = (errorFunction(p1, p2_transforme, max_dist) - error) / step_rotation;
			p2_transforme.deleteCloud();
		
			// calcule la norme du gradient
			norme_gradient = sqrt(dx_translation * dx_translation + dy_translation * dy_translation + dz_translation * dz_translation + d_theta * d_theta + d_phi * d_phi + d_psi * d_psi);
			
			// calcule la derivee vers le minimum
			transformation.translation.x -= dx_translation / norme_gradient * step_translation;
			transformation.translation.y -= dy_translation / norme_gradient * step_translation;
			transformation.translation.z -= dz_translation / norme_gradient * step_translation;
			transformation.theta -= d_theta / norme_gradient * step_rotation;
			transformation.phi -= d_phi / norme_gradient * step_rotation;
			transformation.psi -= d_psi / norme_gradient * step_rotation;
			
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			df_min = (-(errorFunction(p1, p2_transforme, max_dist) - error) / step_translation);
			p2_transforme.deleteCloud();
			
			transformation.translation.x += 2 * dx_translation / norme_gradient * step_translation;
			transformation.translation.y += 2 * dy_translation / norme_gradient * step_translation;
			transformation.translation.z += 2 * dz_translation / norme_gradient * step_translation;
			transformation.theta += 2 * d_theta / norme_gradient * step_rotation;
			transformation.phi += 2 * d_phi / norme_gradient * step_rotation;
			transformation.psi += 2 * d_psi / norme_gradient * step_rotation;
			
			// calcule la derivee vers le maximum
			transformation.makeMatrix();
			p2_transforme = p2.applyTransformationCopy(transformation);
			df_max = (errorFunction(p1, p2_transforme, max_dist) - error) / step_translation;
			p2_transforme.deleteCloud();
			
			// retourne en position initiale
			transformation.translation.x -= dx_translation / norme_gradient * step_translation;
			transformation.translation.y -= dy_translation / norme_gradient * step_translation;
			transformation.translation.z -= dz_translation / norme_gradient * step_translation;
			transformation.theta -= d_theta / norme_gradient * step_rotation;
			transformation.phi -= d_phi / norme_gradient * step_rotation;
			transformation.psi -= d_psi / norme_gradient * step_rotation;
			
			// calcule les derivees premiere et seconde
			first_diff = df_min;
			second_diff = (df_max - df_min) / step_translation;
			double final_step = min(abs(first_diff / second_diff), 0.1);
			
			// se deplace
			transformation.translation.x -= dx_translation / norme_gradient * final_step;
			transformation.translation.y -= dy_translation / norme_gradient * final_step;
			transformation.translation.z -= dz_translation / norme_gradient * final_step;
			transformation.theta -= d_theta / norme_gradient * final_step;
			transformation.phi -= d_phi / norme_gradient * final_step;
			transformation.psi -= d_psi / norme_gradient * final_step;
			
			printf("Iteration %ld, Erreur totale : %lf, Translation : (%lf, %lf, %lf), (%lf, %lf, %lf)\n", i, error, transformation.translation.x, transformation.translation.y, transformation.translation.z, transformation.theta, transformation.phi, transformation.psi);
			//printf("%lf, %lf\n", first_diff,second_diff);
			
			if  (error < error_mini)
			{
				error_mini = error;
				best = transformation;
			}
		}
	}
	
	return best;
}

/*void* errorFunctionThread(void* p_data)
{
	
}*/

// la fonction d'erreur pour l'algo de moindres carrés (calcul du plan tangent a la surface)
double errorFunctionPlane(Vect3d c, Point p, Point* neighbours, long n)
{
	c.normalize();
	double d = (-(c.x*p.x+c.y*p.y+c.z*p.z));
	double err = 0;
	for (long i = 0 ; i < n ; i++) err +=(c.x*neighbours[i].x + c.y*neighbours[i].y + c.z*neighbours[i].z +d) * (c.x*neighbours[i].x + c.y*neighbours[i].y + c.z*neighbours[i].z +d);
	return err;
}

// la fonction d'erreur pour l'algo de moindres carrés (calcul de la hessienne a la surface)
double errorFunctionHess(Vect3d c, Vect3d* p, long n)
{
	double err = 0;
	for (long i = 0 ; i < n ; i++) err += (c.x*p[i].x*p[i].x + c.y*p[i].y*p[i].y + 2*c.z*p[i].x*p[i].y - p[i].z) * (c.x*p[i].x*p[i].x + c.y*p[i].y*p[i].y + 2*c.z*p[i].x*p[i].y - p[i].z);
	return err;
}

// calcule l'équation du plan tangent a la surface au point p par la méthode des moindes carrés
Vect3d tangentPlane(Point p, Point* neighbours, long n, long max_iter, double step, double accuracy)
{
	Vect3d current = {Random(-1,1),Random(-1,1),Random(-1,1)};
	current.normalize();
	Vect3d best = current;
	double err = INFTY;
	double value, df_plus = INFTY, df_moins, d2f = 1;
	Vect3d nabla;
	
	for (long i = 0 ; i < max_iter && abs(df_plus/d2f) > accuracy ; i++)
	{
		current.normalize();
		value = errorFunctionPlane(current, p, neighbours, n);
		
		current.x += step;
		nabla.x = (errorFunctionPlane(current, p, neighbours, n) - value) / step;
		current.x -= step;
		
		current.y += step;
		nabla.y = (errorFunctionPlane(current, p, neighbours, n) - value) / step;
		current.y -= step;
		
		current.z += step;
		nabla.z = (errorFunctionPlane(current, p, neighbours, n) - value) / step;
		current.z -= step;
		
		nabla.normalize();
		current.x += step * nabla.x;
		current.y += step * nabla.y;
		current.z += step * nabla.z;
		df_plus = (errorFunctionPlane(current, p, neighbours, n) - value) / step;
		current.x -= step * nabla.x * 2;
		current.y -= step * nabla.y * 2;
		current.z -= step * nabla.z * 2;
		df_moins = (-errorFunctionPlane(current, p, neighbours, n) + value) / step;
		current.x += step * nabla.x;
		current.y += step * nabla.y;
		current.z += step * nabla.z;
		
		d2f = (df_plus - df_moins) / step;
		
		current.x -= nabla.x * df_plus / d2f;
		current.y -= nabla.y * df_plus / d2f;
		current.z -= nabla.z * df_plus / d2f;
		
		if  (value < err)
		{
			err = value;
			best = current;
		}
		
		//printf("Calcul du plan tangent : Iteration %ld, erreur = %lf, normale = (%lf, %lf, %lf)\n", i, value, current.x, current.y, current.z);
		//printf("Gradient : (%lf, %lf, %lf), df_plus = %lf, df_moins = %lf, d2f = %lf\n", nabla.x, nabla.y, nabla.z, df_plus, df_moins, d2f);
	}
	best.normalize();
	return best;
}

// calcule la hessienne de la surface tangente au point p par la méthode des moindes carrés
Vect3d hessian(Vect3d* neighbours, long n, long max_iter, double step, double accuracy)
{
	Vect3d current = {0,0,0};
	Vect3d best = current;
	double err = INFTY;
	double value, df_plus = INFTY, df_moins, d2f = 1;
	Vect3d nabla;
	
	for (long i = 0 ; i < max_iter && abs(df_plus/d2f) > accuracy ; i++)
	{
		value = errorFunctionHess(current, neighbours, n);
		
		current.x += step;
		nabla.x = (errorFunctionHess(current, neighbours, n) - value) / step;
		current.x -= step;
		
		current.y += step;
		nabla.y = (errorFunctionHess(current, neighbours, n) - value) / step;
		current.y -= step;
		
		current.z += step;
		nabla.z = (errorFunctionHess(current, neighbours, n) - value) / step;
		current.z -= step;
		
		nabla.normalize();
		current.x += step * nabla.x;
		current.y += step * nabla.y;
		current.z += step * nabla.z;
		df_plus = (errorFunctionHess(current, neighbours, n) - value) / step;
		current.x -= step * nabla.x * 2;
		current.y -= step * nabla.y * 2;
		current.z -= step * nabla.z * 2;
		df_moins = (-errorFunctionHess(current, neighbours, n) + value) / step;
		current.x += step * nabla.x;
		current.y += step * nabla.y;
		current.z += step * nabla.z;
		
		d2f = (df_plus - df_moins) / step;
		
		current.x -= nabla.x * df_plus / d2f;
		current.y -= nabla.y * df_plus / d2f;
		current.z -= nabla.z * df_plus / d2f;
		
		if  (value < err)
		{
			err = value;
			best = current;
		}
		
		//printf("Calcul de la hessienne : Iteration %ld, erreur = %lf, D2x = %lf, D2y = %lf, DxDy = %lf\n", i, value, current.x, current.y, current.z);
		//printf("Gradient : (%lf, %lf, %lf), df_plus = %lf, df_moins = %lf, d2f = %lf\n", nabla.x, nabla.y, nabla.z, df_plus, df_moins, d2f);
	}
	return best;
}

// nouvel algo
BackTrack::BackTrack(PointCloud reference, PointCloud align, long sample1, long sample2, long Min_matches, double Factor_min)
{
	reference_ = reference.sampleRandom(sample1);
	new_cloud_ = align.sampleRandom(sample2);
	p1 = reference_.toArray();
	p2 = new_cloud_.toArray();
	assoc = new long[new_cloud_.nb_points];
	best_assoc = new long[new_cloud_.nb_points];
	max_partial = new double[new_cloud_.nb_points];
	min_matches = Min_matches;
	factor_min = Factor_min;
	current_max_depth = 0;
	nb_calls = 0;
	nb_ends = 0;
}

Isometry BackTrack::exec()
{
	printf("\nStarting backtrack with %d points\n", min_matches);
	max_null = new_cloud_.nb_points - min_matches;
	for (long i = 0 ; i < new_cloud_.nb_points ; i++)
	{
		max_partial[i] = INFTY;
		assoc[i] = -1;
		best_assoc[i] = -1;
	}
	BackTrackRec(0, 0, 0);
	return getIsometry(100, 0.001, 0.001);
}

Isometry BackTrack::algoN()
{
	bool changed = true;
	for (long i = 0 ; i < max_null ; i++) assoc[i] = 0;
	for (long i = max_null ; i < new_cloud_.nb_points ; i++) assoc[i] = 1;
	long nb_used = min_matches;
	double best_error = INFTY;
	
	while (changed)
	{
		changed = false;
		for (long i = 0 ; i < new_cloud_.nb_points ; i++)
		{
			if  (assoc[i] == 0)
			{
				assoc[i] = 1;
				nb_used++;
				double error = getError() / nb_used;
				if  (error < best_error)
				{
					best_error = error;
					changed = true;
				}
				else
				{
					assoc[i] = 0;
					nb_used--;
				}
			}
			else if  (nb_used > min_matches)
			{
				assoc[i] = 0;
				nb_used--;
				double error = getError() / nb_used;
				if  (error < best_error)
				{
					best_error = error;
					changed = true;
				}
				else
				{
					assoc[i] = 1;
					nb_used++;
				}
			}
		}
	}
	
	PointCloud pc2(0.01,100);
	for (long i = 0 ; i < new_cloud_.nb_points ; i++)
	{
		if  (assoc[i] == 1) pc2.addPoint(p2[i]);
	}
	Isometry iso = gradientAlgo(reference_, pc2, 100, 0.001, 0.001, FAST);
	pc2.deleteCloud();
	return iso;
}

double BackTrack::getError()
{
	PointCloud pc2(0.01,100);
	for (long i = 0 ; i < new_cloud_.nb_points ; i++)
	{
		if  (assoc[i] == 1) pc2.addPoint(p2[i]);
	}
	Isometry iso = gradientAlgo(reference_, pc2, 100, 0.001, 0.001, FAST);
	iso.makeMatrix();
	pc2.applyTransformation(iso);
	double error = ::errorFunction(reference_, pc2, INFTY);
	pc2.deleteCloud();
	return error;
}

void BackTrack::BackTrackRec(long p, long current_null, double partial)
{
	if  (p > current_max_depth) current_max_depth = p;
	if  (++nb_calls%100 == 0)
	{
		printf("\r");
		for (long i = 0 ; i < 10 ; i++) printf("%ld  ", assoc[i]);
		printf("profondeur = %ld (max : %ld), %lld Iterations", p, current_max_depth, nb_calls);
	}
	
	if  (new_cloud_.nb_points == p)
	{
		printf("End reached %lld times\n", ++nb_ends);
		for (long i = 0 ; i < new_cloud_.nb_points ; i++) best_assoc[i] = assoc[i];
		return;
	}
	/*if  (p-current_null >= 3)
	{
		printf("\nglouglou");
		return;
	}*/
	
	// on autorise le point a ne correspondre a rien
	if  (current_null < max_null)
	{
		assoc[p] = -1;
		BackTrackRec(p+1, current_null+1, partial);
	}
	
	double partials[reference_.nb_points];
	long order[reference_.nb_points];
	
	for (long i = 0 ; i < reference_.nb_points ; i++)
	{
		assoc[p] = i;
		double add_partial = 0;
		for (long j = 0 ; j < p ; j++)
		{
			if  (assoc[j] != -1)
			{
				add_partial += abs(p2[j].distance(p2[p]) - p1[assoc[j]].distance(p1[assoc[p]]));
			}
		}
		partials[i] = add_partial;
		order[i] = i;
	}
	//mergeSortParam(partials, order, reference_.nb_points);
	//printf("results:  \n");
	//for (long i = 0 ; i < reference_.nb_points ; i++) printf("%lf : %ld\n", partials[i], order[i]);
	for (long i = 0 ; i < reference_.nb_points ; i++)
	{
		assoc[p] = order[i];
		partial += partials[i];
		bool ok = true;
		for (long j = 0 ; j < p && ok ; j++)
		{
			if  (assoc[j] == assoc[p]) ok = false;
		}
		if  (ok && ((p != new_cloud_.nb_points-1 /*&& partial <= (max_partial[p])*factor_min*/) || (p == new_cloud_.nb_points-1 && partial <= max_partial[p])))
		{
			if  (partial < max_partial[p]) max_partial[p] = partial;
			//printf("partial = %lf   |   %lf\n", partial, partials[i]);
			BackTrackRec(p+1, current_null, partial);
		}
		partial -= partials[i];
	}
	
	return;
}

// calcule l'erreur liée a une certaine isométrie
double BackTrack::errorFunction(Point* p1, Point* p2, Isometry iso, long n)
{
	double error = 0;
	for (long i = 0 ; i < n ; i++)
	{
		error += iso.apply(p2[i]).distance2(p1[i]);
	}
	return error;
}

// un algo de gradient (encore un!) pour régresser vers une isométrie a partir d'un match de points
Isometry BackTrack::getIsometry(long max_iter, double step, double accuracy)
{
	Isometry transformation(0, 0, 0, {0,0,0});
	Isometry best(0,0,0,{0,0,0});
	long nb_points_used = min_matches;
	
	Point p1_rest[nb_points_used], p2_rest[nb_points_used];
	long i = 0, nb = 0;
	
	for (long k = 0 ; k < nb_points_used ; k++)
	{
		while (best_assoc[i] == -1 && i < new_cloud_.nb_points) i++;
		if  (i < new_cloud_.nb_points) 
		{
			p1_rest[k] = p1[best_assoc[i]];
			p2_rest[k] = p2[i++];
			nb++;
		}
	}
	
	// si n'a pas assez de points
	if  (nb < nb_points_used)
	{
		printf("Not enough points to compute transformation\n");
		return best;
	}
	
	double error = INFTY, error_mini = INFTY;;
	double dx_translation, dy_translation, dz_translation, d_theta, d_phi, d_psi, norme_gradient;
	double first_diff = INFTY;
	double second_diff = 1.0, df_min, df_max;
	double step_translation = step, step_rotation = step;
		
	for (long i = 0 ; i < max_iter && abs(first_diff / second_diff) > accuracy ; i++)
	{
			transformation.makeMatrix();
			error = errorFunction(p1_rest, p2_rest, transformation, nb_points_used);
		
			// calcule la derivee de l'erreur par rapport a translation.x;
			transformation.translation.x += step_translation;
			transformation.makeMatrix();
			dx_translation = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_translation;
			transformation.translation.x -= step_translation;
			
			// calcule la derivee de l'erreur par rapport a translation.y;
			transformation.translation.y += step_translation;
			transformation.makeMatrix();
			dy_translation = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_translation;
			transformation.translation.y -= step_translation;
			
			// calcule la derivee de l'erreur par rapport a translation.z;
			transformation.translation.z += step_translation;
			transformation.makeMatrix();
			dz_translation = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_translation;
			transformation.translation.z -= step_translation;
			
			// calcule la derivee de l'erreur par rapport a theta;
			transformation.theta += step_rotation;
			transformation.makeMatrix();
			d_theta = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_rotation;
			transformation.theta -= step_rotation;
			
			// calcule la derivee de l'erreur par rapport a phi;
			transformation.phi += step_rotation;
			transformation.makeMatrix();
			d_phi = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_rotation;
			transformation.phi -= step_rotation;
			
			// calcule la derivee de l'erreur par rapport a psi;
			transformation.psi += step_rotation;
			transformation.makeMatrix();
			d_psi = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_rotation;
			transformation.psi -= step_rotation;
			
			// calcule la norme du gradient
			norme_gradient = sqrt(dx_translation * dx_translation + dy_translation * dy_translation + dz_translation * dz_translation + d_theta * d_theta + d_phi * d_phi + d_psi * d_psi);
			
			// calcule la derivee vers le minimum
			transformation.translation.x -= dx_translation / norme_gradient * step_translation;
			transformation.translation.y -= dy_translation / norme_gradient * step_translation;
			transformation.translation.z -= dz_translation / norme_gradient * step_translation;
			transformation.theta -= d_theta / norme_gradient * step_rotation;
			transformation.phi -= d_phi / norme_gradient * step_rotation;
			transformation.psi -= d_psi / norme_gradient * step_rotation;
			
			transformation.makeMatrix();
			df_min = (-(errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_translation);
			
			transformation.translation.x += 2 * dx_translation / norme_gradient * step_translation;
			transformation.translation.y += 2 * dy_translation / norme_gradient * step_translation;
			transformation.translation.z += 2 * dz_translation / norme_gradient * step_translation;
			transformation.theta += 2 * d_theta / norme_gradient * step_rotation;
			transformation.phi += 2 * d_phi / norme_gradient * step_rotation;
			transformation.psi += 2 * d_psi / norme_gradient * step_rotation;
			
			// calcule la derivee vers le maximum
			transformation.makeMatrix();
			df_max = (errorFunction(p1_rest, p2_rest, transformation, nb_points_used) - error) / step_translation;
			
			// retourne en position initiale
			transformation.translation.x -= dx_translation / norme_gradient * step_translation;
			transformation.translation.y -= dy_translation / norme_gradient * step_translation;
			transformation.translation.z -= dz_translation / norme_gradient * step_translation;
			transformation.theta -= d_theta / norme_gradient * step_rotation;
			transformation.phi -= d_phi / norme_gradient * step_rotation;
			transformation.psi -= d_psi / norme_gradient * step_rotation;
			
			// calcule les derivees premiere et seconde
			first_diff = df_min;
			second_diff = (df_max - df_min) / step_translation;
			double final_step = min(abs(first_diff / second_diff), 0.1);
			
			// se deplace
			transformation.translation.x -= dx_translation / norme_gradient * final_step;
			transformation.translation.y -= dy_translation / norme_gradient * final_step;
			transformation.translation.z -= dz_translation / norme_gradient * final_step;
			transformation.theta -= d_theta / norme_gradient * final_step;
			transformation.phi -= d_phi / norme_gradient * final_step;
			transformation.psi -= d_psi / norme_gradient * final_step;
			
			//printf("Iteration %ld, Erreur totale : %lf, Translation : (%lf, %lf, %lf), (%lf, %lf, %lf)\n", i, error, transformation.translation.x, transformation.translation.y, transformation.translation.z, transformation.theta, transformation.phi, transformation.psi);
			//printf("%lf, %lf\n", first_diff,second_diff);
			
			if  (error < error_mini)
			{
				error_mini = error;
				best = transformation;
			}
	}
	return best;
}

void BackTrack::deleteBackTrack()
{
	reference_.deleteCloud();
	new_cloud_.deleteCloud();
	delete p1, p2;
	delete assoc, max_partial;
}
