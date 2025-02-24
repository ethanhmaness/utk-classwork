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

class Xylophone {
  public:
    int countKeys(vector <int> keys);
};

int Xylophone::countKeys(vector <int> keys)
{
	
	unsigned char b = 0;
	int sum = 0;

	for(int i = 0; i < keys.size(); i++){
		
		b |= 1 << (keys.at(i) % 7);
	
	}

	for(int i = 0; i < 7; i++){
		
		if((b >> i) % 2 == 1) sum++;

	}

	return sum;

}
