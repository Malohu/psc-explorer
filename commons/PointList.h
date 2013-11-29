#ifndef POINT_LIST_H
#define POINT_LIST_H

#include <stdlib.h>
#include "../commons/point.h"

class PointCell
{
	public :
	Point point;
	PointCell* next;
		
		PointCell(Point P, PointCell* n);
};

class PointList
{
	PointCell *first, *last;
	int size;
	int version;
	
	public :
		PointList();
		void addLast(Point p);
		void addFirst(Point p);
		int length();
		Point pop();
		bool isEmpty();
		void flush();
};

#endif
