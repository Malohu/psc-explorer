#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../commons/geometry.h"
#include "../commons/benchmark.h"
#include "../commons/point.h"
#include "../commons/error.h"

using namespace std;

int main(int argc, char* argv[])
{
	srand(time(NULL));
	
	Point *p1, *p2, *p2_init;
	long n1, n2, n, max_iter;
	double start = -1.5, stop = 1.5, step_translation, accuracy;
	Vect3d translation = {Random(-1,1), Random(-1,1), Random(-1,1)};
	Isometry transformation(Random(-Pi,Pi)/4, Random(-Pi,Pi)/4, Random(-Pi,Pi)/4, translation);
	//Isometry transformation(0.3, -0.22, -0.44, translation);
	transformation.makeMatrix();
	
	printf("Nombre de points : ");
	scanf("%ld", &n);
	printf("Nombre d'iterations maxi : ");
	scanf("%ld", &max_iter);
	printf("Deplacement dans l'espace des parametres : ");
	scanf("%lf", &step_translation);
	printf("Precision : ");
	scanf("%lf", &accuracy);
	
	Point* points = randomPoints(n, 2.0);  // genere n points random
	/*splitX(points, n, &p1, &p2, &n1, &n2, start, stop);  // separe en 2 nuages a intersection non vide
	printf("%ld points dans le nuage 1, %ld points dans le nuage 2\n", n1, n2);
	p2_init = new Point[n2];
	for (long i = 0 ; i < n2 ; i++) p2_init[i] = p2[i];  // sauvegarde la position initiale du nuage 2
	for (long i = 0 ; i < n2 ; i++) transformation.apply(&(p2[i]));  // deplace uniquement le second nuage
	double erreur = quadratic(p2_init, p2, n2);  // calcule l'erreur quadratique avant recalage
	Isometry transformation_recalage = gradientAlgo(p1, p2, n1, n2, max_iter, step_translation, step_rotation);  // algo de recalage
	transformation_recalage.makeMatrix();
	for (long i = 0 ; i < n2 ; i++) transformation_recalage.apply(&(p2[i]));  // recale le second nuage
	double erreur2 = quadratic(p2_init, p2, n2);  // calcule l'erreur quadratique apres recalage*/
	
	// la meme chose avec les arbres
	PointCloud t1(0.01, 100), t2(0.01 ,100), t2_moved(0.01, 100), t2_sampled(0.01, 100);
	splitX(points, n, &t1, &t2, start, stop);
	delete points;
	printf("%ld points dans le nuage 1, %ld points dans le nuage 2\n", t1.nb_points, t2.nb_points);
	t2_moved = t2.applyTransformationCopy(transformation);
	t2_sampled = t2_moved.sampleRandom(500);
	double erreur = t2.errorAlignment(transformation);
	
	//BackTrack(t1, t2_moved, 1000, 0.2);
	BackTrack bt(t1, t2_moved, 30, 8, 5, 1.0);
	Isometry transformation_recalage = bt.algoN();
	bt.deleteBackTrack();
	
	/*PointCloud petits(0.01,100), grands(0.01,100);
	
	t2_moved = t1;
	Point* points_tab = t2_moved.toArray();
		double* features = new double[t2_moved.nb_points];
		printf("...\n");
		double rmin = 0.0001;
		double rmax = 0.05;
		long nb_steps = 5;
		long max_points = 100;
		
		for (long i = 0 ; i < t2_moved.nb_points ; i++) {features[i] = t2_moved.integralFeature(points_tab[i], rmin, rmax, nb_steps, max_points); printf("%ld - %lf\n", i, features[i]);}
		for (long i = 0 ; i < t2_moved.nb_points ; i++) printf("feature %ld = %lf\n", i, features[i]);
		
		// calcule la distribution des points par geometric feature
		long hist_width = 3.5*0.4*sqrt(t2_moved.nb_points);
		long* eff = new long[hist_width+1];
		for (long i = 0 ; i < hist_width ; i++) eff[i] = 0;
		for (long i = 0 ; i < t2_moved.nb_points ; i++)
		{
			if  (features[i] > 0 && features[i] < 10) eff[(long)(features[i]*((double)hist_width)/10)]++;
			if  (features[i] > 4) petits.addPoint(points_tab[i]);
			else grands.addPoint(points_tab[i]);
		}
		for (long i = 0 ; i < hist_width ; i++) printf("%lf <= feature < %lf : Effectif : %ld\n", ((double)i)/((double)hist_width)*10, ((double)(i+1))/((double)hist_width)*10, eff[i]);
		
		delete eff;
		delete features;
		delete points_tab;*/	
	
	//Isometry transformation_recalage = gradientAlgo(t1, t2_sampled, max_iter, step_translation, accuracy, FAST);
	transformation_recalage.makeMatrix();
	double erreur2 = t2.errorAlignment(transformation, transformation_recalage);
	t2.deleteCloud();
	t2_sampled.deleteCloud();
	PointCloud t3 = t2_moved.applyTransformationCopy(transformation_recalage);
	
	printf("\nDeplacement initial : (%lf, %lf, %lf)\n", transformation.translation.x, transformation.translation.y, transformation.translation.z);
	printf("Deplacement final : (%lf, %lf, %lf)\n", transformation_recalage.translation.x, transformation_recalage.translation.y, transformation_recalage.translation.z);
	printf("Erreur relative moyenne avant : %lf\nErreur relative moyenne apres : %lf\n", erreur, erreur2);
	
	t1.saveChar("aaa.txt");
	t2_moved.saveChar("bbb.txt");
	t3.saveChar("ccc.txt");
	/*petits.saveChar("aaa.txt");
	grands.saveChar("bbb.txt");*/
	
	
	return 0;
}
