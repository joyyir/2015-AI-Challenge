#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__

#include <queue>
#include "Node.h"

#define SIZE_HARD_TO_FIND 200

class PathFinder{
private:
	int n, m;
	int startX, startY, goalX, goalY;
	bool running;
	bool (*cbValid)(int, int);
	Node * best;
	Node * maxScoreNode;
	Point * directions[4];
	int openSetMap[101][101], closedSetMap[101][101];
	int openSetCnt, closedSetCnt;
	Node * nodeMap[101][101];
	priority_queue<Node, vector<Node>, greater<Node>> pQueue;
	int sizeHardToFind;
	int mode;
public:
	// iteratePath() return values
	static const int CONTINUE = 0;
	static const int NOT_RUNNING = 1;
	static const int CANNOT_REACH = 2;
	static const int REACH = 3;
	static const int HARD_TO_FIND = 4;

	// setupPath() return values
	static const int ERROR = 0;
	static const int SUCCESS = 1;

	// mode values
	static const int TARGET_SEARCH = 0;
	static const int NEIGHBOR_SEARCH = 1;

	PathFinder(bool (*cbValid)(int, int), int n, int m, int mode = TARGET_SEARCH){
		this->startX = 0;
		this->startY = 0;
		this->goalX = 0;
        this->goalY = 0;
        this->cbValid = cbValid;
        this->running = false;
		this->n = n;
		this->m = m;
		setMode(mode);
        clearNodes(true);
		
		directions[0] = new Point(-1, 0);
		directions[1] = new Point(1, 0);
		directions[2] = new Point(0, -1);
		directions[3] = new Point(0, 1);
	}

	~PathFinder(){
		for(int i = 0; i < 4; i++){
			delete directions[i];
		}
		for(int i = 0; i < n; i++){
			for(int j = 0; j < m; j++){
				if(nodeMap[i][j] != NULL)
					delete nodeMap[i][j];
			}
		}
	}

	Node & makeNode(int x, int y){
		nodeMap[x][y] = new Node(x, y);
		return *nodeMap[x][y];
	}

	Node * getNode(int x, int y){
		if(nodeMap[x][y] != NULL)
			return nodeMap[x][y];
		else return NULL;
	}

	Node * findNodeFromOpenSet(int x, int y){
		if(openSetMap[x][y] == 1)
			return nodeMap[x][y];
		else return NULL;
	}

	Node * findNodeFromClosedSet(int x, int y){
		if(closedSetMap[x][y] == 1)
			return nodeMap[x][y];
		else return NULL;
	}

	void openSetPush(Node & node){
		openSetMap[node.x][node.y] = 1;
		openSetCnt++;
		pQueue.push(node);
	}

	void openSetPop(){
		openSetMap[pQueue.top().x][pQueue.top().y] = 0;
		openSetCnt--;
		pQueue.pop();
	}

	void closedSetPush(Node & node){
		closedSetMap[node.x][node.y] = 1;
		closedSetCnt++;
	}

	void setSizeHardToFind(int size){
		this->sizeHardToFind = size;
	}

	void setMode(int mode){
		this->mode = mode;

		if(mode == TARGET_SEARCH)
			setSizeHardToFind(10000);
		else
			setSizeHardToFind(SIZE_HARD_TO_FIND);
	}

	Node * getMaxScoreNode(){
		return maxScoreNode;
	}

	int getClosedSetMap(int x, int y){
		return closedSetMap[x][y];
	}

	void clearNodes(bool isCreated = false){
		for(int i = 0 ; i < n; i++){
			for(int j = 0; j < m; j++){
				openSetMap[i][j] = 0;
				closedSetMap[i][j] = 0;
				if(!isCreated && nodeMap[i][j] != NULL)
					delete nodeMap[i][j];
				nodeMap[i][j] = NULL;
			}
		}
		openSetCnt = 0;
		closedSetCnt = 0;
		best = NULL;

		// pQueue 비우기
		priority_queue<Node, vector<Node>, greater<Node>> empty;
		swap(pQueue, empty);
	}

	int setupPath(int startX, int startY, int goalX, int goalY){
		if(!cbValid(goalX, goalY)){
			return ERROR;
		}

		this->startX = startX;
		this->startY = startY;
		this->goalX = goalX;
		this->goalY = goalY;
		clearNodes();

		Node& startNode = makeNode(startX, startY);
		int distance = (goalX - startX)*(goalX - startX) + (goalY - startY)*(goalY - startY);
		startNode.initialize(distance);
		maxScoreNode = &startNode;

		openSetPush(startNode);
		//openSet.push_back(startNode);
		running = true;

		return SUCCESS;
	}

	int iteratePath(){
		if(!running)
			return NOT_RUNNING;
		if(openSetCnt == 0)
			return CANNOT_REACH;
		if(closedSetCnt > sizeHardToFind)
			return HARD_TO_FIND;

		best = getNode(pQueue.top().x, pQueue.top().y);
		//closedSetPush(pQueue.top());
		if(mode == NEIGHBOR_SEARCH && best->getScorePerDistance() > maxScoreNode->getScorePerDistance()){
			maxScoreNode = best;
		}
		closedSetPush(*best);
		openSetPop();

		/*closedSet.push_back(openSet.front());
		openSet.pop_front();
		best = &closedSet.back();*/
		/*if(best == NULL)
			return 0;*/

		if(mode == TARGET_SEARCH && best->x == goalX && best->y == goalY)
            return REACH;

		for(int i = 0; i < 4; i++){
			int x = best->x + directions[i]->x;
			int y = best->y + directions[i]->y;
			//int cost = sqrtf((directions[i]->x)^2 + (directions[i]->y)^2);
			int cost = 1;
			if(cbValid(x, y)){
				processAdjacent(best, x, y, cost);
			}
		}

		return CONTINUE;
	}

	void processAdjacent(Node * parent, int x, int y, int cost){
		int g = parent->g + cost;
		Node * checkNode = findNodeFromOpenSet(x, y);
		if(checkNode != NULL){
			parent->childern.push_front(checkNode);
			if(g < checkNode->g){
				checkNode->setParent(parent);
				checkNode->g = g;
				checkNode->f = g + checkNode->h;
			}
		}
		else{
			checkNode = findNodeFromClosedSet(x, y);
			if(checkNode != NULL){
				parent->childern.push_front(checkNode);
				if(g < checkNode->g){
					checkNode->setParent(parent);
					checkNode->g = g;
					checkNode->f = g + checkNode->h;
				}
			}
			else{
				Node & newNode = makeNode(x, y);
				newNode.setParent(parent);
				newNode.initialize((goalX-x)*(goalX-x)+(goalY-y)*(goalY-y), g);
				openSetPush(newNode);
				/*openSet.push_front(newNode);
				openSet.sort();*/
			}
		}
	}

	list<Point> finishPath(){
		Node * node;

		if(mode == TARGET_SEARCH)
			node = best;
		else
			node = maxScoreNode;

		list<Point> path;
		while(node != NULL){
			path.push_front(Point(node->x, node->y));
			//cerr << "(" << node->x << ", " << node->y << ", 점수 : " << node->score << ", 거리 : " << node->distance << ", 거리당점수 : " << node->getScorePerDistance() << ")" << endl;
			node = node->parent;
		};
		running = false;

		// for debug - start
		/*cerr << "PathFinder :: " << endl;
		list<Point>::iterator iterEnd = path.end();
		for(list<Point>::iterator iterPos = path.begin(); 
		iterPos != iterEnd; ++iterPos){
			cerr << "(" << iterPos->x << ", " << iterPos->y << ") -> ";
		}
		cerr << "end" << endl;*/
		// for debug - end

		return path;
	}
};

#endif