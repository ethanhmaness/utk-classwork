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

class MiddleCode {
  public:
    string encode(string s);
};

string MiddleCode::encode(string s)
{
	char temp;
	string t = "";
	
	while(s.size() > 0){
		if(s.size() % 2 == 1){
		    temp = s.at(s.size()/2);
		    s = s.erase(s.size()/2, 1);
		} else {
		    if(s.at(s.size()/2) < s.at(s.size()/2 - 1)){
				temp = s.at(s.size()/2);
				s = s.erase(s.size()/2, 1);
			} else {
				temp = s.at(s.size()/2 - 1);
				s = s.erase(s.size()/2 - 1, 1);
			}
		}
		t.push_back(temp);
	}

	return t;
}
