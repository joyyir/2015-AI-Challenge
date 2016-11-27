#ifndef __NODE_H__
#define __NODE_H__

#include <list>
#include "Point.h"
using namespace std;

class Node : public Point{
public :
	int f, g, h;
	Node * parent;
	list<Node *> childern;

	Node(int x, int y) : Point(x, y){
		this->f = 0;
		this->g = 0;
		this->h = 0;
		this->parent = NULL;
		this->childern.clear();
	}

	void initialize(int distance, int g = 0){
		this->g = g;
		this->h = distance;
		this->f = this->g + this->h;
	}

	void setParent(Node * parent){
		this->parent = parent;
		if(parent != NULL)
			distance = parent->distance + 1;
	}

	bool operator<(const Node & node) const{
		return this->f < node.f;
	}

	bool operator>(const Node & node) const{
		return this->f > node.f;
	}
};

#endif