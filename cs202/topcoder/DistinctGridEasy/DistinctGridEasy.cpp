#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

class DistinctGridEasy {
  public:
    string checkGrid(int n, int k, vector <int> grid);
};

string DistinctGridEasy::checkGrid(int n, int k, vector <int> grid)
{
	set<int> s;
	set<int>::const_iterator sit;
	int i, j;

	for(i = 0; i < n; i++){
		s.clear();
		for(j = 0; j < n; j++){
			s.insert(grid.at(i*n + j));
		}
		if(s.size() != k) return "Bad";
	}
	for(i = 0; i < n; i++){
        s.clear();
        for(j = 0; j < n; j++){
            s.insert(grid.at(i + j*n));
        }
        if(s.size() != k) return "Bad";
    }
	return "Good";
}
