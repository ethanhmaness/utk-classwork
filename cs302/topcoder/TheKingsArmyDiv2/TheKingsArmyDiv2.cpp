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

class TheKingsArmyDiv2 {
  public:
    int getNumber(vector <string> state);
};

int TheKingsArmyDiv2::getNumber(vector <string> state)
{
	size_t width = state.at(0).size();
	size_t height = state.size();
	bool Hs = false;

	for(size_t i = 0; i < height; i++){
		for(size_t j = 0; j < width; j++){
			if(state.at(i).at(j) == 'H'){
				Hs = true;
				if(i > 0)
					if(state.at(i-1).at(j) = 'H')
						return 0;
				if(i < height - 1)
					if(state.at(i+1).at(j) = 'H')
						return 0;
				if(j > 0)
					if(state.at(i).at(j-1) = 'H')
						return 0;
				if(j < width - 1)
					if(state.at(i).at(j+1) = 'H')
						return 0;
			}
		}
	}
	return Hs ? 1 : 2;
}
