#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

/*
	BearCries.cpp
	Ethan Maness
	May 9, 2022
	
	Implementation for calculating the number of possible ways to divide a string into substrings such that each substring is a crying emoticon ";__;" (the number of underscores in each emoticon may be any positive integer)
*/

class BearCries {
  public:
    string message;
    map<string, long long> cache;
    int count(string message);
    long long C(int i, int ewu, int ewou);
    long long memoize(int i, int ewu, int ewou);
};

//gets the recursive answers and memoizes before returning
long long BearCries::memoize(int index, int ewu, int ewou){

  stringstream ss("");

  //get the recursive solution
  long long rv = C(index, ewu, ewou);
  
  ss << index << "_" << ewu << "_" << ewou;
  
  //memoize the solution (unless the index is the message size -- its faster to just evaluate it than search the map)
  if(index != message.size()) cache.insert(make_pair(ss.str(), rv));
  //printf("C(%d,%d,%d) = %lld\n", index, ewu, ewou, rv); //testing code
  
  //return the solution
  return rv % 1000000007;

}

//recursive function for calculating the number of possible partitionings
long long BearCries::C(int index, int ewu, int ewou){

  //if there are more unfinished emoticons than remaining characters, there is no valid partitioning
  if(message.size() - index < ewu + ewou) return 0;
  
  //if there are no remaining characters and no unfinished emoticons, there is 1 valid partitioning
  if(index == message.size()) return 1;

  //check cache to see if this case is already solved
  stringstream ss("");
  ss << index << "_" << ewu << "_" << ewou; 
  map<string, long long>::iterator mit = cache.find(ss.str());
  if(mit != cache.end()) return mit->second;
  
  long long rv = 0;
  
  //when message[index] is underscore, you can move 1 unfinished emoticon from "without underscore" to "with underscore", or extend one with an underscore (trivial)
  if(message[index] == '_'){
  
    //if there are no unfinished emoticons, no valid partitioning can be reached
    if(ewu+ewou == 0) return 0;
    
    //else, sum partition count from each option
    if(ewu > 0) rv += ((long long) ewu) * memoize(index+1, ewu, ewou);
    if(ewou > 0) rv += ((long long) ewou) * memoize(index+1, ewu+1, ewou-1);
    return rv % 1000000007;
  }
  
  //when message[index] is semicolon, you can start 1 new emoticon or finish 1 emoticon with an underscore
  if(message[index] == ';'){
  
    //sum partition count for each option
    if(ewu > 0) rv += ((long long) ewu) * memoize(index+1, ewu-1, ewou);
    rv += memoize(index+1, ewu, ewou+1);
    return rv % 1000000007;
  
  }
  
  //if you somehow encounter a character that isn't ';' or '_', skip it because it will be omitted from any valid partitionings
  return memoize(index+1, ewu, ewou);

}

int BearCries::count(string message)
{
  this->message = message;
  long long rv = C(0,0,0) % 1000000007;
  //printf("C(0,0,0) = %lld\n", rv); //testing code
  return rv;
}
