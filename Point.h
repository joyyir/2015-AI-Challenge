#ifndef __POINT_H__
#define __POINT_H__

extern int getScoreAtPoint(int x, int y);

class Point{
public:
	int x, y;
	int score, distance;

	Point(int x, int y){
		this->x = x;
		this->y = y;
		this->score = getScoreAtPoint(x, y);
		this->distance = 0;
	}

	Point(int x, int y, int score, int distance){
		this->x = x;
		this->y = y;	
		this->score = score;
		this->distance = distance;
	}

	float getScorePerDistance() const{
		if(distance > 0)
			return score / (float)distance;
		else{
			//cerr << "Node :: getScorePerDistance() :: 0 또는 음수로 나누었다." << endl;
			return -99999;
		}
	}

	bool operator<(const Point & p) const{
		return this->getScorePerDistance() < p.getScorePerDistance();
	}
};

#endif