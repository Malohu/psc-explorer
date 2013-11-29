#include <string.h>
#include <unistd.h>

#include "telemeter.h"
#include "UrgCtrl.h"
#include <cstdlib>
#include <cstdio>

#include "../commons/benchmark.h"
#include "../commons/error.h"

using namespace qrk;
using namespace std;

Telemeter::Telemeter()
{
	setDefault(1000000, 1081, 5, 0, "/dev/ttyACM0");
}

void Telemeter::setDefault(double Delay_step_acquisition, double Nb_points_acquisition, double Step_acquisition, double Offset_phi, char* Device)
{
	offset_global_start = 0;
	offset_global_stop = 0;
	offset_slices_start = 0;
	offset_slices_stop = 0;
	
	delay_step_acquisition = Delay_step_acquisition;
	nb_points_acquisition = Nb_points_acquisition;
	step_acquisition = Step_acquisition;
	offset_phi = Offset_phi;
	strcpy(device, Device);
	
	/*min_pos = 500;     //  <-- configuration par defaut du servomoteur
	max_pos = 2500;
	current_pos = min_pos;*/
	return;
}

Point* Telemeter::sliceAcquisition(long* nb_points)
{
	*nb_points = 0;
	return NULL;
}

long cpt_files_telemeter = 0;

// acquisition complète (retourne le nombre de points acquis)
Point* Telemeter::acquisition(long* nb_points)
{
	/* pour l'instant cette fonction est pipo, bien évidemment */
	
	Point* points = new Point[(int)(((180/step_acquisition)+1)*nb_points_acquisition)];
	*nb_points = 0;
	
	zero();
	for (int i = 0 ; i < 180/step_acquisition ; i++)
	{
		update();
		usleep(delay_step_acquisition);
		
		double phi = ((double)(-i)) * step_acquisition + offset_phi;
		
		UrgCtrl urg;
  		if (! urg.connect(device))
  		{
    			error("Cannot connect to telemeter. Will self-destroy in 10 seconds (1)");
    			continue;
  		}

  		vector<long> data;
  		long timestamp = 0;
  		int n = urg.capture(data, &timestamp);
  		printf("Tranche %d\n", i);
  		if (n < 0)
  		{
    			printf("Telemeter acquisition failed. You lost the game (2)");
    			continue;
  		}
		
  		Point slice[2000];
  		double theta = (-270.0)/2.0;
  		for (int j = 0 ; j < n ; j++)
  		{
  			slice[j].y = 0;
  			slice[j].x = ((double)data[j]) * sin(theta*Pi/180) / 1000.0;
  			slice[j].z = ((double)data[j]) * cos(theta*Pi/180) / 1000.0;
  			theta += 270.0/(nb_points_acquisition-1);
  		}
  		Isometry rotation(0, 0, phi*Pi/180, {0,0,0});
  		rotation.makeMatrix();
  		for (int j = 0 ; j < n ; j++) points[(*nb_points)++] = rotation.apply(slice[j]);
	}
	zero();
	
	/*long n = 50000;
	Point* points = randomPoints(n, 2.0);  // genere n points random 
	*nb_points = n;
	
	Vect3d translation = {Random(-1,1)/2, Random(-1,1)/2, Random(-1,1)/2};
	Isometry transformation(Random(-Pi,Pi)/8, Random(-Pi,Pi)/8, Random(-Pi,Pi)/8, translation);
	transformation.makeMatrix();
	
	for (long i = 0 ; i < n ; i++) transformation.apply(&points[i]);*/
	
	/*long real_points = 0;
	char file_addr[200];
	Point* points;
	points = new Point[2000000];
	
	cpt_files_telemeter = 0;
	Point center_empty(Random(-5,5), Random(-0.1,0.1), Random(-5,5));
	Point center_empty2(Random(-5,5), Random(-0.1,0.1), Random(-5,5));
	double radius = 2.0;
	for (int i = 0 ; i < 9 ; i++){
	
	double phi = 0, psi = 0, theta = 0;//((double)i)/180*Pi*40;
	Isometry trans(theta, phi, psi, {0,0,0});
	trans.makeMatrix();
	
	strcpy(file_addr, "input/scan");
	file_addr[10] = (cpt_files_telemeter/100)%10 + 48;
	file_addr[11] = (cpt_files_telemeter/10)%10 + 48;
	file_addr[12] = (cpt_files_telemeter/1)%10 + 48;
	file_addr[13] = 0;
	strcat(file_addr, ".3d");
	printf("%s\n", file_addr);
	
	FILE *f = fopen(file_addr, "rb");
	if  (f != NULL)
	{
		
		Point p(0,0,0,1,0,0,0);
		char buf[1000];
		while (readLine(f, buf) != 0)
		{
			sscanf(buf, "%lf %lf %lf", &p.x, &p.y, &p.z);
			p.x /= 1000;
			p.y /= 1000;
			p.z /= 1000;
			if  (rand()%50 == 0 && p.distance(center_empty) > radius && p.distance(center_empty2) > radius) {points[real_points++] = p;
			trans.apply(&(points[real_points-1]));}
		}
		fclose(f);
		
	}
	cpt_files_telemeter++;
	
	}
	
	Vect3d translation = {Random(-1,1)/5, Random(-1,1)/5, Random(-1,1)/5};
	Isometry transformation(Random(-Pi,Pi)/10, Random(-Pi,Pi)/10, Random(-Pi,Pi)/10, translation);
	transformation.makeMatrix();
	
	for (long i = 0 ; i < real_points ; i++) transformation.apply(&points[i]);
	
	*nb_points = real_points;
	printf("nb_points = %ld\n", *nb_points);*/
	
	return points;
}

// change la position du telemetre en commandant le servomoteur QVB
/*void Telemeter::setPosition(double pos)
{
	current_pos = min_pos + (long)(((double)(max_pos - min_pos)) * pos / 180.0);
	return;
}

void Telemeter::turn()
{
	// blah blah a envoyer au servomoteur
	
	return;
}

void Telemeter::turn(double pos)
{
	setPosition(pos);
	turn();
	return;
}*/

// remet le telemetre a sa position d'origine
void Telemeter::zero()
{
	system("echo \"a\" > /dev/ttyACM1");
	return;
}

// fait tourner le telemetre d'un cran
void Telemeter::update()
{
	system("echo \"b\" > /dev/ttyACM1");
	return;
}
