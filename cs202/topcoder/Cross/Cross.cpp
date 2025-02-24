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

class Cross {
  public:
    string exist(vector <string> board);
};

string Cross::exist(vector <string> B)
{
	for(size_t i = 1; i < B.size() - 1; i++){
		for(size_t j = 1; j < B.at(i).size() - 1; j++){
			if(B.at(i-1).at(j) == '#' && B.at(i).at(j-1) == '#' && B.at(i).at(j) == '#' && B.at(i).at(j+1) == '#' && B.at(i+1).at(j) == '#'){
				return "Exist";
			}
		}
	}
	return "Does not exist";
}
