#include <string>
#include <cmath>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iterator>
using namespace std;

/*
  RadioRange.cpp
  Ethan Maness
  Feb 13, 2022
  
  Determines what proportion of circles centered at 0, 0 of radii between 0 and Z would not intersect any of the given vectors of circles of positions X, Y and radii R
  The program does this by "flattening" the problem into a 1-D representation of good and bad ranges of double values (since all that matters is distance from origin)
  Then, it simply sums the sizes of all good ranges and divides by Z
*/

class RadioRange {
  public:
    double RadiusProbability(vector <int> X, vector <int> Y, vector <int> R, int Z);
};

double min(double a, double b){
  return (a > b) ? b : a;
}

double RadioRange::RadiusProbability(vector<int> X, vector<int> Y, vector<int> R, int Z){
  
  double d;
  unsigned int n;
  
  //city thresholds (true indicates entering, false indicates leaving)
  map<double, bool> T;
  map<double, bool>::iterator mit;
  
  //ranges of good radii
  vector<pair<double, double>> G;
  
  //populate T
  for(size_t i = 0; i < X.size(); i++){
  
    //distance from radio center to close edge of city is X^2 + Y^2 - R (will be negative if radio station is within city)
    d = sqrt(pow(X[i], 2) + pow(Y[i], 2)) - R[i];
    
    //ignore any cities which are entirely outside of the range
    if(d > Z) continue;
    T.insert(pair<double, bool>(d, true));
    
    d = sqrt(pow(X[i], 2) + pow(Y[i], 2)) + R[i];
    
    //distance from radio center to far edge of city is X^2 + Y^2 + R
    T.insert(pair<double, bool>( min(Z, d), false));
    
  }
  
  //if no cities in range, all radii are valid
  if(T.size() == 0) return 1;
  
  //populate G
  
  n = 0; //represents the number of cities overlapping the current circle radius
  if(T.begin()->first > 0) G.push_back(pair<double, double>(0, min(Z, T.begin()->first)));
  
  for(mit = T.begin(); mit != T.end(); mit++){
  
    if(T.begin()->first >= Z) break;
  
    if(mit->second) n++;
    else n--;
    
    //adds a range from the current threshold to the next (which must be an "entering-city" threshold)
		if(n == 0) G.push_back(pair<double, double>(mit->first, min(Z, (next(mit) == T.end()) ? Z : next(mit)->first)));
  
  }
  
  //sum the sizes of all good ranges
  
  d = 0;
  for(size_t i = 0; i < G.size(); i++){
  
    d += G[i].second - G[i].first;
    cout << "Good from " << G[i].first << " to " << G[i].second << endl;
  
  }
  
  cout << "Total good radii range of " << d << " out of total range of " << Z << endl;
  
  //probability = summed size of good ranges / size of whole possible range
  return d / Z;

}