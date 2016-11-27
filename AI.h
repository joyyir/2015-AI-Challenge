#ifndef __AI_H__
#define __AI_H__

#include <vector>
#include <string>
#include <list>
#include <queue>
#include <ctime>
#include "Point.h"
#include "PathFinder.h"

#define SAMPLE_NUM 1000

using namespace std;

int getScoreAtPoint(int x, int y);

struct AI
{
	static int n, m; // map size (row, column)
	int T; // total turns
	int x, y; // my position
	static vector<string> *tiles;
	static vector<vector<int>> *scores;
	queue<string> pathQueue;
	list<Point> unreachablePointList;
	Point maxScorePoint;

	AI() : maxScorePoint(-1, -1){}

	void init(int n, int m, int turns, int myRow, int myCol){
		// recommended way to initialize random
		srand(clock());

		this->n = n;
		this->m = m;
		this->T = turns;
		this->x = myRow;
		this->y = myCol;
	}

	string action(vector<string> tiles, vector<vector<int>> scores, int opponentRow, int opponentCol){
		this->tiles = &tiles;
		this->scores = &scores;

		string act = "STAY";
		PathFinder p = PathFinder(isValidCell, n, m);
		
		//cerr << "action :: 현재 내 좌표 : (" << x << ", " << y << ")" << endl;
		//cerr << "action :: 현재 상대방 좌표 : (" << opponentRow << ", " << opponentCol << ")" << endl;

		if((pathQueue.size() == 0 || getDistance(x, y, opponentRow, opponentCol) == 0) && getScoreAtPoint(x, y) > 0){
			//cerr << "action :: 정복! 획득 점수 : " << getScoreAtPoint(x, y) << endl;
			act = "CONQUER";
		}
		else{
			//cerr << "action :: 정복 안한다." << endl;
			if(pathQueue.size() > 0){
				//cerr << "action :: 현재 목적지 : (" << maxScorePoint.x << ", " << maxScorePoint.y << "), (점수, 거리, 거리당점수) = (" << maxScorePoint.score << ", " << maxScorePoint.distance << ", " << maxScorePoint.getScorePerDistance() << ")" << endl;
				//cerr << "action :: 상대방이 서있는 땅을 감시한다." << endl;
				if(!isInUnreachablePointList(opponentRow, opponentCol) && p.setupPath(x, y, opponentRow, opponentCol) == PathFinder::SUCCESS){
					int status = p.iteratePath();
					while(status == PathFinder::CONTINUE){
						status = p.iteratePath();
					}
					if(status == PathFinder::CANNOT_REACH){
						//cerr << "action :: 도달할 수 없는 점입니다. " << "(" << opponentRow << ", " << opponentCol << ")" << endl;
						for(int i = 0; i < n; i++){
							for(int j = 0; j < m; j++){
								if(p.getClosedSetMap(i, j) == 0 && tiles[i][j] != 'X'){
									unreachablePointList.push_back(Point(i, j));
								}
							}
						}
						return "STAY";
					}
					else if(status == PathFinder::REACH){
						list<Point> pathList = p.finishPath();
						int score = getScoreAtPoint(opponentRow, opponentCol);
						int distance = pathList.size() - 1; // 0이 될 위험은 없다.
						float scorePerDistance = score / (float)distance;
						//cerr << "action :: 도달 가능한 점입니다. " << "(" << opponentRow << ", " << opponentCol << "), 거리당점수 : " << scorePerDistance << endl;
						if(scorePerDistance > maxScorePoint.getScorePerDistance()){
							//cerr << "action :: 상대방 땅이 가치가 높다. 뺏으러가자!" << endl;
							maxScorePoint = Point(opponentRow, opponentCol, score, distance);
							putPathQueue(pathList);
						}
					}
					else if(status == PathFinder::HARD_TO_FIND){
						//cerr << "action :: 도착점" << "(" << opponentRow << ", " << opponentCol << ")" << "이 너무 멀어서 찾기 힘듭니다." << endl;
					}
				}
			}
			else{
				//cerr << "action :: pathQueue가 비었다. 넣을 것을 찾자." << endl;
				float maxScorePerDistance = -99999;
				list<Point> maxScorePointList;
				list<Point> maxPathList;
				priority_queue<Point> pointQueue;
				int randomRow, randomCol, score, distance;

				//cerr << "action :: " << SAMPLE_NUM << "개 샘플링 합니다." << endl;
				for(int i = 0; i < SAMPLE_NUM; i++){
					randomRow = rand() % n;
					randomCol = rand() % m;

					if(isBlock(randomRow, randomCol) || getDistance(x, y, randomRow, randomCol) == 0){
						//i--; // 넓은 맵일 경우 주석 풀 것
						continue;
					}
					
					score = getScoreAtPoint(randomRow, randomCol);
					distance = getDistance(x, y, randomRow, randomCol);
					pointQueue.push(Point(randomRow, randomCol, score, distance));
				}
				
				// 도달할 수 있고 거리대비 이득이 가장 큰 점을 정한다.
				maxScorePerDistance = -99999;
				while(true){
					Point point = pointQueue.top();
					pointQueue.pop();

					if(isInUnreachablePointList(point.x, point.y))
						continue;

					if(p.setupPath(x, y, point.x, point.y) != PathFinder::SUCCESS)
						continue;
					
					//QueryPerformanceCounter(&liCounter1); // Start
					int status = p.iteratePath();
					while(status == PathFinder::CONTINUE){
						status = p.iteratePath();
					}
					//QueryPerformanceCounter(&liCounter2); // End
					//cerr << "action :: Time : " << ((double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart) * 1000 << "ms" << endl;
					
					if(status == PathFinder::CANNOT_REACH){
						//cerr << "action :: 도달할 수 없는 점입니다. " << "(" << point.x << ", " << point.y << ")" << endl;
						for(int i = 0; i < n; i++){
							for(int j = 0; j < m; j++){
								if(p.getClosedSetMap(i, j) == 0 && tiles[i][j] != 'X'){
									unreachablePointList.push_back(Point(i, j));
								}
							}
						}
						return "STAY";
					}
					else if(status == PathFinder::REACH){
						maxPathList = p.finishPath();
						int score = getScoreAtPoint(point.x, point.y);
						int distance = maxPathList.size() - 1; // 자기 자신은 뺀다.
						maxScorePerDistance = score / (float)distance;
						maxScorePoint = Point(point.x, point.y, score, distance);
						//cerr << "action :: 도달 가능한 점입니다. " << "(" << point.x << ", " << point.y << "), 거리당점수 : " << maxScorePerDistance << endl;
						break;
					}
					else if(status == PathFinder::HARD_TO_FIND){
						//cerr << "action :: 도착점" << "(" << point.x << ", " << point.y << ")" << "이 너무 멀어서 찾기 힘듭니다." << endl;
					}
				}
				//cerr << "action :: 샘플링 결과, 일단은 (" << maxScorePoint.x << ", " << maxScorePoint.y << ")로 가기로 했다." << endl;

				//cerr << "action :: " << SIZE_HARD_TO_FIND << "개의 인접한 점을 탐색한다. " << endl;
				p.setMode(PathFinder::NEIGHBOR_SEARCH);
				while(true){
					Point point = pointQueue.top();
					pointQueue.pop();

					if(p.setupPath(x, y, point.x, point.y) != PathFinder::SUCCESS)
						continue;

					int status = p.iteratePath();
					while(status == PathFinder::CONTINUE){
						status = p.iteratePath();
					}

					if(status == PathFinder::REACH){
						//cerr << "action :: 이게 뜨면 이상한거임..." << endl;
					}
					else if(status == PathFinder::HARD_TO_FIND || status == PathFinder::CANNOT_REACH){
						list<Point> pathList = p.finishPath();
						Node * node = p.getMaxScoreNode();
						float scorePerDistance = node->getScorePerDistance();
						//cerr << "action :: 인접한 점 중 가장 가치가 높은 점 : " << "(" << node->x << ", " << node->y << "), 거리당점수 : " << scorePerDistance << endl;
						if(scorePerDistance > maxScorePerDistance){
							maxScorePerDistance = scorePerDistance;
							maxScorePoint = Point(node->x, node->y, node->score, node->distance);
							maxPathList = pathList;
						}
					}
					break;
				}
				p.setMode(PathFinder::TARGET_SEARCH);
				putPathQueue(maxPathList);
				//cerr << "action :: 최종적으로 (" << maxScorePoint.x << ", " << maxScorePoint.y << ")로 가기로 했다." << endl;
			}
			maxScorePoint.distance--; // 추가
			act = pathQueue.front();
			pathQueue.pop();
			//cerr << "action :: pathQueue에 있는 행동을 취한다 : " << act << endl;
		}

		if(act == "L") y--;
		if(act == "U") x--;
		if(act == "R") y++;
		if(act == "D") x++;

		// deal with 'OUT OF GRID'
		if(y < 0) y = 0;
		if(x < 0) x = 0;
		if(x >= n) x = n - 1;
		if(y >= m) y = m - 1;

		return act;
	}

	// pathQueue에 새 경로를 집어넣는다.
	void putPathQueue(list<Point>& maxPathList){
		//cerr << "putPathQueue :: 경로를 pathQueue에 넣는다." << endl;
		
		// pathQueue 비우기
		queue<string> empty;
		swap(pathQueue, empty);

		Point prevPoint = maxPathList.front();
		maxPathList.pop_front();
		Point nowPoint(-1, -1);
		while(maxPathList.size() > 0){
			nowPoint = maxPathList.front();
			maxPathList.pop_front();
			pathQueue.push(getDirection(prevPoint.x, prevPoint.y, nowPoint.x, nowPoint.y));
			prevPoint = nowPoint;
		}
	}

	// start점과 end점 사이의 거리를 단순히 좌표값의 차로 구한다.
	int getDistance(int startX, int startY, int endX, int endY){
		return ((startX - endX) > 0 ? (startX - endX) : (endX - startX)) + ((startY - endY) > 0 ? (startY - endY) : (endY - startY));
	}

	// end점이 start점을 기준으로 어느 방향에 위치해있는지를 반환한다. (단, end점과 start점은 이웃이다.)
	string getDirection(int startX, int startY, int endX, int endY){
		if(getDistance(startX, startY, endX, endY) != 1)
			return "STAY";
		else{
			if(startX - endX == 1)
				return "U";
			if(endX - startX == 1)
				return "D";
			if(startY - endY == 1)
				return "L";
			if(endY - startY == 1)
				return "R";
			else
				return "STAY";
		}
	}

	// 점 (x, y)가 block('X')이거나 맵의 범위를 넘으면 true를 반환한다.
	static bool isBlock(int x, int y){
		return x < 0 || y < 0 || x >= n || y >= m || ((*tiles)[x][y] == 'X');
	}

	// isBlock 함수의 반전이다.
	static bool isValidCell(int x, int y){
		return !isBlock(x, y);
	}

	// 점 (x, y)가 '도달할 수 없는 점 목록'에 포함되어있으면 true를 반환한다.
	bool isInUnreachablePointList(int x, int y){
		bool isInUnreachablePointList = false;
		list<Point>::iterator iterEnd = unreachablePointList.end();
		for(list<Point>::iterator iterPos = unreachablePointList.begin(); 
		iterPos != iterEnd; ++iterPos){
			if(iterPos->x == x && iterPos->y == y){
				//cerr << "isInUnreachablePointList :: 이 점은 도달할 수 없는 점들의 리스트에 속해있는 점입니다." << endl;
				isInUnreachablePointList = true;
			}
		}

		return isInUnreachablePointList;
	}
};

#endif