#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

class SwapAndArithmetic {
  public:
    string able(vector <int> x);
};

string SwapAndArithmetic::able(vector <int> x)
{
  
	sort(x.begin(), x.end());
	int d = x[1] - x[0];
	for(size_t i = 2; i < x.size(); i++){

		if(x[i] - x[i-1] != d) return "Impossible";

	}

	return "Possible";

}
