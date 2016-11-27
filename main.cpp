#include <iostream>
#include <string>
#include <vector>
#include "AI.h"
using namespace std;

// DO NOT TOUCH ANYTHING {{{
int main() { 
	AI* instance = new AI();
	int n, m, T, x, y;
	cin >> n >> m >> T >> x >> y;

	instance->init(n, m, T, x, y);

	cout << "READY" << endl;
	for(int turn = 1; turn <= T; ++ turn) {
		vector<string> S1;
		for(int i=0; i<	n; ++i) {
			string buf;
			cin >> buf;
			S1.push_back(buf);
		}
		vector< vector<int> > S2;
		for(int i = 0; i < n; ++ i) {
			S2.push_back(vector<int>(m));
			for(int j = 0; j < m; ++ j) {
				cin >> (S2.back()[j]);
			}
		}
		int ox, oy;
		cin >> ox >> oy;
		string act = instance->action(S1, S2, ox, oy);
		cout << act.c_str() << endl;
	}
	delete instance;
	return 0;
}
// END OF DO NOT TOUCH ANYTHING }}}