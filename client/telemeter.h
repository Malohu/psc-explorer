#ifndef TELEMETER_H
#define TELEMETER_H

#include "../commons/point.h"

class Telemeter
{
	public :
		
	double interval_slices;  // intervalle (en degrés) entre deux tranches
	double offset_global_start, offset_global_stop;  // les décalages entre les positions extremes du telemetre et les positions effectives d'acquisition
	double offset_slices_start, offset_slices_stop;  // les décalages pour tronquer une acquisition du telemetre sur les bords
	double delay_step_acquisition, nb_points_acquisition, step_acquisition;
	double offset_phi;
	char device[200];
	//long current_pos;
	//long min_pos, max_pos;  // les positions (min, max et current) en microsecondes
	
		Telemeter();
		int init();  // retourne a la position par defaut
		void setDefault(double Delay_step_acquisition, double Nb_points_acquisition, double Step_acquisition, double Offset_phi, char* Device);  // définit la position par defaut
		Point* sliceAcquisition(long* nb_points);  // récupère une tranche
		Point* acquisition(long* nb_points);  // acquisition complète (retourne le nombre de points acquis dans nb_points)
		//void setPosition(double pos);
		//void turn();
		//void turn(double pos);
		void zero();
		void update();
};

#endif
