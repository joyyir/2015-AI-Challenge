#include "AI.h"

int AI::n, AI::m;
vector<string> *AI::tiles;
vector<vector<int>> *AI::scores;

// 점 (x, y)의 점수를 구한다. 
int getScoreAtPoint(int x, int y){
	int s = 0;

	if(x<0 || y<0 || x>=AI::n || y>=AI::m) return -99999;

	for(int i = x - 1; i <= x + 1; ++i) {
		for(int j = y - 1; j <= y + 1; ++j) {
			if(i < 0 || j < 0 || i >= AI::n || j >= AI::m)
				continue;
			int score = (*AI::scores)[i][j];
			char tile = (*AI::tiles)[i][j];
			if(tile == 'M') score = -1;
			else if(tile == 'O'){
				//score--; // 1버전
				//score = (score - 1 > 0) ? (2 * (score - 1)) : (score - 1); // 2버전
				score = 2 * score - 1; // 3버전
			}
			if(tile == 'M' || tile == 'O' || tile == 'U')
				s += score;
		}
	}
	return s;
}

