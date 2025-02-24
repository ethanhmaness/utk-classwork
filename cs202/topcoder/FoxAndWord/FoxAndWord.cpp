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

class FoxAndWord {
  public:
    int howManyPairs(vector <string> words);
};

int FoxAndWord::howManyPairs(vector <string> words)
{
	int count = 0;
	string tmp1;
	string tmp2;
	bool flag = false;

	for(int i = 0; i < words.size() - 1; i++){
		for(int j = i + 1; j < words.size(); j++){
			if(words.at(i).size() == words.at(j).size()){
				flag = false;
				for(int k = 1; k < words.at(i).size(); k++){
					tmp1 = words.at(i).substr(0,k);
					tmp2 = words.at(i).substr(k);
					if(tmp2 + tmp1 == words.at(j)) flag = true;
				}
				if(flag) count++;
			}
		}
	}

	return count;
}
