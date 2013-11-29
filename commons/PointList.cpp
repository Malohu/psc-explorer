#include "../commons/PointList.h"

PointCell::PointCell(Point P, PointCell* n)
{
	point = P;
	next = n;
}

PointList::PointList()
{
	size = 0;
	version = 0;
	first = NULL;
	last = NULL;
}

void PointList::addLast(Point p)
{
	PointCell* c = new PointCell(p, NULL);
	if  (!size) first = c;
	else last->next = c;
	last = c;
	size++;
	return;
}

void PointList::addFirst(Point p)
{
	PointCell* c = new PointCell(p, first);
	first = c;
	if  (!size) last = c;
	size++;
	return;
}

int PointList::length()
{
	return size;
}

Point PointList::pop()
{
	Point p = first->point;
	PointCell* c = first;
	first = first->next;
	delete c;
	size--;
	if  (!size) last = NULL;
	return p;
}

bool PointList::isEmpty()
{
	return (!size);
}

void PointList::flush()
{
	while (size) pop();
	return;
}
