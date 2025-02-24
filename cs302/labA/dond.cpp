#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>
using namespace std;

/*
  dond.cpp
  Ethan Maness
  May 9, 2022
  
  
*/

class DoND {
  public:
    map<string, double> cache;
    double probability(int s, int t, int last_roll);
};

double DoND::probability(int s, int t, int last_roll){

  //Base cases (one roll remaining)
  if(t == 1){
    if(last_roll == -1) return 1;
    if(s >= 2 && (last_roll == 0 || last_roll == s-1)) return (s-2)/(double)s;
    if(s >= 3) return (s-3)/(double)s;
    return 0;
  }

  //create cache key for this call
  string key;
  stringstream ss;
  ss << s << "_" << t << "_" << last_roll;
  key = ss.str();
  
  //check cache for key, return solution if present
  map<string, double>::iterator mit;
  mit = cache.find(key);
  if(mit != cache.end()) return mit->second;
  
  //otherwise, solve recursively and add to cache
  double rv = 0;
  for(int i = 0; i < s; i++){
    if(last_roll == -1 || abs(i-last_roll) > 1) rv += probability(s, t-1, i);
  }
  rv /= s;
  cache.insert(make_pair(key, rv));
  return rv;

}

int main(int argc, char** argv){

  if(argc != 4) return 1;

  stringstream ss;
  int s;
  int t;
  int last_roll;
  DoND dond;
  
  ss.str(string(argv[1]));
  if(!(ss >> s)) return 1;
  ss.clear();
  
  ss.str(string(argv[2]));
  if(!(ss >> t)) return 1;
  ss.clear();
  
  ss.str(string(argv[3]));
  if(!(ss >> last_roll)) return 1;
  ss.clear();
  
  cout << dond.probability(s, t, last_roll) << endl;

}