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

class InsertZ {
  public:
    string canTransform(string init, string goal);
};

string InsertZ::canTransform(string init, string goal)
{
	string noz;
	for(int i = 0; i < goal.size(); i++){
		if(goal.at(i) != 'z') noz += goal.at(i);
	}
	if(noz == init) return "Yes";
	else return "No";
}
