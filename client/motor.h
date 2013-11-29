#ifndef MOTORS_H
#define MOTORS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../commons/geometry.h"

class Motor
{
	public:
	
	double speed, rotation_speed;  // vitesse moyenne du robot en m/s et rad/s
	long move_step, turn_step;  // valeurs a passer a la carte de puissance
	Position old_pos, new_pos;  // les ancienne et nouvelle position
	double rotation, translation;  // valeurs du deplacement
	
		Motor();
		void init();
		void prepareMove(Position Old_pos, Position New_pos);
		double move();
		double move(Position Old_pos, Position New_pos);
};

#endif
