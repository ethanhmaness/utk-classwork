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

class DistanceBetweenStrings {
  public:
    int getDistance(string a, string b, string letterSet);
};

int DistanceBetweenStrings::getDistance(string a, string b, string l)
{
	string al;
	string bl;
	string ll;
	double dist = 0;
	int cina;
	int cinb;
	
	for(int i = 0; i < a.size(); i++){
		if(a.at(i) >= 'A' && a.at(i) <= 'Z') al += a.at(i) - 'A' + 'a';
		else al += a.at(i);
	}
	for(int i = 0; i < b.size(); i++){
        if(b.at(i) >= 'A' && b.at(i) <= 'Z') bl += b.at(i) - 'A' + 'a';
        else bl += b.at(i);
    }
	for(int i = 0; i < l.size(); i++){
		if(l.at(i) >= 'A' && l.at(i) <= 'Z') ll += l.at(i) - 'A' + 'a';
        else ll += l.at(i);
    }

	for(int i = 0; i < ll.size(); i++){
		cina = 0;
		cinb = 0;
		for(int j = 0; j < al.size(); j++){
			if(al.at(j) == ll.at(i)) cina++;
		}
		for(int j = 0; j < bl.size(); j++){
            if(bl.at(j) == ll.at(i)) cinb++;
        }
		dist += pow(cinb - cina, 2);
	}

	return static_cast<int>(dist);
}
