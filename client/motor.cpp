#include "motor.h"

Motor::Motor()
{
	init();
}

void Motor::init()
{
	rotation = 0;
	translation = 0;
	old_pos = new_pos;
	move_step = 0;
	turn_step = 0;
	speed = 0.25;  // <<-- A AJUSTER !!!
	rotation_speed = 2;
	
	return;
}

// prépare un déplacement
void Motor::prepareMove(Position Old_pos, Position New_pos)
{
	old_pos = Old_pos;
	new_pos = New_pos;
	
	Vect3d t = soust(new_pos.position, old_pos.position);
	//Vect3d r = soust(new_pos.orientation, old_pos.orientation);
	
	new_pos.orientation.z = 0;
	old_pos.orientation.z = 0;
	double n0 = old_pos.orientation.norm(), n1 = new_pos.orientation.norm();
	double c0 = old_pos.orientation.x / n0, c1 = new_pos.orientation.x / n1;
	double s0 = old_pos.orientation.y / n0, s1 = new_pos.orientation.y / n1;
	double a0 = acos(c0), a1 = acos(c1);
	if  (s0 < 0) a0 = -a0;
	if  (s1 < 0) a1 = -a1;
	rotation = a1 - a0;
	while (rotation > Pi) rotation -= 2*Pi;
	while (rotation <= -Pi) rotation += 2*Pi;
	t.z = 0;
	translation = t.norm();
	
	return;
}

// exécute un déplacement préprogrammé
double Motor::move()
{
	/* envoie le signal de debut de rotation */
	if  (abs(rotation) > 2/180*Pi)
	{
		if  (rotation < 0) system("echo \"c\" > /dev/ttyACM1");
		else system("echo \"d\" > /dev/ttyACM1");
		
		usleep((long)(abs(rotation)/rotation_speed*1000000));
		/* envoie le signal de fin de rotation */
		system("echo \"e\" > /dev/ttyACM1");
		
		sleep(1);
	}
	
	if  (abs(translation) > 0.02)
	{
		printf("%lf\n", translation);
		/* envoie le signal de debut de mouvement */
		system("echo \"f\" > /dev/ttyACM1");
		usleep((long)(abs(translation)/speed*1000000));
		/* envoie le signal de fin de mouvement */
		system("echo \"e\" > /dev/ttyACM1");
	}
	return translation;
}

double Motor::move(Position Old_pos, Position New_pos)
{
	prepareMove(Old_pos, New_pos);
	return move();
}
