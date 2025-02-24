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

class SellingFruits {
  public:
    int maxDays(int x, int f, int d, int p);
};

int SellingFruits::maxDays(int x, int f, int d, int p)
{
	int day = 0;
	while(d >= x && f > 0){
		d -= x;
		f--;
		day++;
		if(f == 0){
			d -= p;
			f++;
		}
	}
	return day;
}
