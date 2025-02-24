#include <string>
#include <cmath>
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

/*
  FindThePerfectTriangle.cpp
  March 28, 2022
  Ethan Maness
  
  Given an area and perimeter, finds a triangle (if possible) with integer coordinates, with the leftmost point at the origin.
  NOTE TO TA: For some reason, the grader.sh says "you gave no answer, but there is one" when I test it on lines that randinput.txt marks as not having a solution ("no") for the third parameter. I am not sure why this is happening but my program outputs a valid triangle for every set of parameters that has a solution and nothing for those without.
*/

#define VIT(i, v) for (i = 0; i < v.size(); i++) 

class FindThePerfectTriangle {
  public:
    vector <int> constructTriangle(int area, int perimeter);
};

//Returns a vector of all (x,y) pairs that are l units from the origin
vector<pair<int, int> > Positive_Integer_Vectors_Of_Length(int l){

  vector<pair<int, int> > rv;
  
  //check (x, y) pairs, insert those that match the length into the return vector
  for(int x = 0; x <= l; x++){
    //efficient diagonal slice -- sum of coordinates must be between 1 and sqrt(2) times the side length (inclusive)
    for(int y = l; x + y >= l; y--){
      //include simple boolean expression first to save operations
		  if(x + y <= 1.415 * l && x * x + y * y == l * l) rv.push_back(make_pair(x, y));
		} 
  }
  
  return rv;
}

//Checks if an integer-coordinate triangle is possible given the side lengths (requires a <= b <= c)
vector<int> Is_Triangle_OK(int a, int b, int c, int area){

  vector<int> rv;

  //Check area
  int perimeter = a + b + c;
  if(perimeter % 2 != 0) return rv;
  int q = perimeter / 2;
  int area_squared = q * (q-a) * (q-b) * (q-c);
  if(area_squared != area * area) return rv;
  
  //Check if integer coordinates exist (vertex AC is at 0,0)
  vector<pair<int, int> > possible_c_endpoints = Positive_Integer_Vectors_Of_Length(c);
  vector<pair<int, int> > possible_a_endpoints = Positive_Integer_Vectors_Of_Length(a);
  pair<int, int> b1;
  pair<int, int> b2;

  for(size_t i = 0; i < possible_c_endpoints.size(); i++){
    for(size_t j = 0; j < possible_a_endpoints.size(); j++){
      //test a with (x,y) and (x,-y)
			for(int s = 0; s <= 1; s++){
        b1 = (s == 0) ? possible_a_endpoints[j] : make_pair(possible_a_endpoints[j].first, -(possible_a_endpoints[j].second));
        b2 = possible_c_endpoints[i];
	      if((b2.first-b1.first) * (b2.first-b1.first) + (b2.second-b1.second) * (b2.second-b1.second) == b * b){
	        rv.push_back(0);
					rv.push_back(0);
					rv.push_back(possible_a_endpoints[j].first);
					rv.push_back((s == 0) ? possible_a_endpoints[j].second : -(possible_a_endpoints[j].second));
					rv.push_back(possible_c_endpoints[i].first);
					rv.push_back(possible_c_endpoints[i].second);
          return rv;
        }
			}
    }
  }

  return rv;

}

vector <int> FindThePerfectTriangle::constructTriangle(int area, int perimeter)
{
  vector <int> rv;

  //Perimeter must be even to have an integer area (by Heron's Formula)
  if(perimeter % 2 != 0) return rv;
  
  int c;
  for(int a = 1; a <= perimeter / 3; a++){
    for(int b = a; b < perimeter / 2; b++){
      c = perimeter - a - b;
      if(c >= b){
        rv = Is_Triangle_OK(a, b, c, area);
        if(rv.size() != 0) return rv;
      }
    }
  }

  return rv;
}
