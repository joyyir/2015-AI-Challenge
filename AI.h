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
		
		//cerr << "action :: ���� �� ��ǥ : (" << x << ", " << y << ")" << endl;
		//cerr << "action :: ���� ���� ��ǥ : (" << opponentRow << ", " << opponentCol << ")" << endl;

		if((pathQueue.size() == 0 || getDistance(x, y, opponentRow, opponentCol) == 0) && getScoreAtPoint(x, y) > 0){
			//cerr << "action :: ����! ȹ�� ���� : " << getScoreAtPoint(x, y) << endl;
			act = "CONQUER";
		}
		else{
			//cerr << "action :: ���� ���Ѵ�." << endl;
			if(pathQueue.size() > 0){
				//cerr << "action :: ���� ������ : (" << maxScorePoint.x << ", " << maxScorePoint.y << "), (����, �Ÿ�, �Ÿ�������) = (" << maxScorePoint.score << ", " << maxScorePoint.distance << ", " << maxScorePoint.getScorePerDistance() << ")" << endl;
				//cerr << "action :: ������ ���ִ� ���� �����Ѵ�." << endl;
				if(!isInUnreachablePointList(opponentRow, opponentCol) && p.setupPath(x, y, opponentRow, opponentCol) == PathFinder::SUCCESS){
					int status = p.iteratePath();
					while(status == PathFinder::CONTINUE){
						status = p.iteratePath();
					}
					if(status == PathFinder::CANNOT_REACH){
						//cerr << "action :: ������ �� ���� ���Դϴ�. " << "(" << opponentRow << ", " << opponentCol << ")" << endl;
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
						int distance = pathList.size() - 1; // 0�� �� ������ ����.
						float scorePerDistance = score / (float)distance;
						//cerr << "action :: ���� ������ ���Դϴ�. " << "(" << opponentRow << ", " << opponentCol << "), �Ÿ������� : " << scorePerDistance << endl;
						if(scorePerDistance > maxScorePoint.getScorePerDistance()){
							//cerr << "action :: ���� ���� ��ġ�� ����. ����������!" << endl;
							maxScorePoint = Point(opponentRow, opponentCol, score, distance);
							putPathQueue(pathList);
						}
					}
					else if(status == PathFinder::HARD_TO_FIND){
						//cerr << "action :: ������" << "(" << opponentRow << ", " << opponentCol << ")" << "�� �ʹ� �־ ã�� ����ϴ�." << endl;
					}
				}
			}
			else{
				//cerr << "action :: pathQueue�� �����. ���� ���� ã��." << endl;
				float maxScorePerDistance = -99999;
				list<Point> maxScorePointList;
				list<Point> maxPathList;
				priority_queue<Point> pointQueue;
				int randomRow, randomCol, score, distance;

				//cerr << "action :: " << SAMPLE_NUM << "�� ���ø� �մϴ�." << endl;
				for(int i = 0; i < SAMPLE_NUM; i++){
					randomRow = rand() % n;
					randomCol = rand() % m;

					if(isBlock(randomRow, randomCol) || getDistance(x, y, randomRow, randomCol) == 0){
						//i--; // ���� ���� ��� �ּ� Ǯ ��
						continue;
					}
					
					score = getScoreAtPoint(randomRow, randomCol);
					distance = getDistance(x, y, randomRow, randomCol);
					pointQueue.push(Point(randomRow, randomCol, score, distance));
				}
				
				// ������ �� �ְ� �Ÿ���� �̵��� ���� ū ���� ���Ѵ�.
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
						//cerr << "action :: ������ �� ���� ���Դϴ�. " << "(" << point.x << ", " << point.y << ")" << endl;
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
						int distance = maxPathList.size() - 1; // �ڱ� �ڽ��� ����.
						maxScorePerDistance = score / (float)distance;
						maxScorePoint = Point(point.x, point.y, score, distance);
						//cerr << "action :: ���� ������ ���Դϴ�. " << "(" << point.x << ", " << point.y << "), �Ÿ������� : " << maxScorePerDistance << endl;
						break;
					}
					else if(status == PathFinder::HARD_TO_FIND){
						//cerr << "action :: ������" << "(" << point.x << ", " << point.y << ")" << "�� �ʹ� �־ ã�� ����ϴ�." << endl;
					}
				}
				//cerr << "action :: ���ø� ���, �ϴ��� (" << maxScorePoint.x << ", " << maxScorePoint.y << ")�� ����� �ߴ�." << endl;

				//cerr << "action :: " << SIZE_HARD_TO_FIND << "���� ������ ���� Ž���Ѵ�. " << endl;
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
						//cerr << "action :: �̰� �߸� �̻��Ѱ���..." << endl;
					}
					else if(status == PathFinder::HARD_TO_FIND || status == PathFinder::CANNOT_REACH){
						list<Point> pathList = p.finishPath();
						Node * node = p.getMaxScoreNode();
						float scorePerDistance = node->getScorePerDistance();
						//cerr << "action :: ������ �� �� ���� ��ġ�� ���� �� : " << "(" << node->x << ", " << node->y << "), �Ÿ������� : " << scorePerDistance << endl;
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
				//cerr << "action :: ���������� (" << maxScorePoint.x << ", " << maxScorePoint.y << ")�� ����� �ߴ�." << endl;
			}
			maxScorePoint.distance--; // �߰�
			act = pathQueue.front();
			pathQueue.pop();
			//cerr << "action :: pathQueue�� �ִ� �ൿ�� ���Ѵ� : " << act << endl;
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

	// pathQueue�� �� ��θ� ����ִ´�.
	void putPathQueue(list<Point>& maxPathList){
		//cerr << "putPathQueue :: ��θ� pathQueue�� �ִ´�." << endl;
		
		// pathQueue ����
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

	// start���� end�� ������ �Ÿ��� �ܼ��� ��ǥ���� ���� ���Ѵ�.
	int getDistance(int startX, int startY, int endX, int endY){
		return ((startX - endX) > 0 ? (startX - endX) : (endX - startX)) + ((startY - endY) > 0 ? (startY - endY) : (endY - startY));
	}

	// end���� start���� �������� ��� ���⿡ ��ġ���ִ����� ��ȯ�Ѵ�. (��, end���� start���� �̿��̴�.)
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

	// �� (x, y)�� block('X')�̰ų� ���� ������ ������ true�� ��ȯ�Ѵ�.
	static bool isBlock(int x, int y){
		return x < 0 || y < 0 || x >= n || y >= m || ((*tiles)[x][y] == 'X');
	}

	// isBlock �Լ��� �����̴�.
	static bool isValidCell(int x, int y){
		return !isBlock(x, y);
	}

	// �� (x, y)�� '������ �� ���� �� ���'�� ���ԵǾ������� true�� ��ȯ�Ѵ�.
	bool isInUnreachablePointList(int x, int y){
		bool isInUnreachablePointList = false;
		list<Point>::iterator iterEnd = unreachablePointList.end();
		for(list<Point>::iterator iterPos = unreachablePointList.begin(); 
		iterPos != iterEnd; ++iterPos){
			if(iterPos->x == x && iterPos->y == y){
				//cerr << "isInUnreachablePointList :: �� ���� ������ �� ���� ������ ����Ʈ�� �����ִ� ���Դϴ�." << endl;
				isInUnreachablePointList = true;
			}
		}

		return isInUnreachablePointList;
	}
};

#endif