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

class FauxPalindromes {
  public:
    string classifyIt(string word);
};

string FauxPalindromes::classifyIt(string word)
{
	string drow = "";
	
	for(int i = 0; i < word.size(); i++) drow += word.at(word.size() - 1 - i);
	if(drow.compare(word) == 0) return "PALINDROME";
	
	string drow2 = "";
	for(int i = 0; i < drow.size(); i++){
		if(i == drow.size() - 1 || drow.at(i) != drow.at(i+1)) drow2 += drow.at(i);
	}

	bool flag = true;
	for(int i = 0; i < drow2.size() / 2; i++){
		if(drow2.at(i) != drow2.at(drow2.size() - 1 - i)) flag = false;
	}

	return (flag) ? "FAUX" : "NOT EVEN FAUX";
}
