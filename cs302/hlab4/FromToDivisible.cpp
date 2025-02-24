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
#include <queue>
using namespace std;

/*
  FromToDivisible.cpp
  Ethan Maness
  April 17, 2022
  
  Implements the shortest() method, which calculates the shortest path that can be taken from an integer S to an integer T, given that you can only travel from one integer to another if the first is a multiple of a value in a and the second is a multiple of the value in b at the same index and is no larger than N.
*/

class FromToDivisible {
  public:
    int shortest(int N, int S, int T, vector <int> a, vector <int> b);
};

//Euclid's formula for finding the greatest common denominator
int GCD(long long x, long long y){

  long long a = x % y;
  if(a == 0) return y;
  return GCD(y, a);

}

//Finds the least common multiple
long long LCM(long long x, long long y){

  return x * y / GCD(x, y);

}

//Calculates the shortest path from S to T where positive integer multiples of values in a and b with the same indices that are no larger than N are connected
int FromToDivisible::shortest(int N, int S, int T, vector <int> a, vector <int> b)
{

  //adjLists[i] = list of nodes that i has an edge to
  //index of S = a.size()
  //index of T = a.size() + 1
  vector< list<int> > adjLists(a.size() + 2); 
  
  //Populate adjacency lists
  for(size_t i = 0; i < a.size(); i++){
  
    //internal node to internal node
    for(size_t j = 0; j < a.size(); j++){
      if(i != j && LCM(b[i], a[j]) <= N){
        adjLists[i].push_back(j);
        //printf("Edge from %d to %d\n", i, j); //---REMOVE---
      }
    }
    
    //S to internal node
    if(S % a[i] == 0){
      adjLists[a.size()].push_back(i);
      //printf("Edge from S to %d\n", i); //---REMOVE---
    }
    
    //internal node to T
    if(T % b[i] == 0){
      adjLists[i].push_back(a.size() + 1);
      //printf("Edge from %d to T\n", i); //---REMOVE---
    }
    
  }
  
  //BFS vars
  list<int>::iterator lit;
  vector<int> distances(a.size() + 2, -1);
  vector<int> backlinks(a.size() + 2, -1);
  queue<int> bfsq;
  int node;
  
  //Start at S
  distances[a.size()] = 0;
  bfsq.push(a.size());
  
  //Run BFS until T's distance is found
  while(bfsq.size() > 0 && distances[a.size() + 1] == -1){
    node = bfsq.front();
    bfsq.pop();
    
    /*printf("Node: %d", node); //---REMOVE---
    printf("   Distances: ");
    for(size_t i = 0; i < distances.size(); i++){
      printf("%2d ", distances[i]);
    }
    printf("   Backlinks: ");
    for(size_t i = 0; i < backlinks.size(); i++){
      printf("%2d ", backlinks[i]);
    }
    printf("\n");*/ //---REMOVE---
    
    for(lit = adjLists[node].begin(); lit != adjLists[node].end(); lit++){
      //printf("Edge to %d\n", *lit); //---REMOVE---
      if(distances[*lit] == -1){
        //printf("Pushing %d\n", *lit); //---REMOVE---
        distances[*lit] = distances[node] + 1;
        backlinks[*lit] = node;
        bfsq.push(*lit);
      }
    }
  }
  
  /*printf("Node: %d", node); //---REMOVE---
  printf("   Distances: ");
  for(size_t i = 0; i < distances.size(); i++){
    printf("%2d ", distances[i]);
  }
  printf("   Backlinks: ");
  for(size_t i = 0; i < backlinks.size(); i++){
    printf("%2d ", backlinks[i]);
  }
  printf("\n");*/ //---REMOVE---
  
  //distance in orig graph is distance in equivalent graph minus one
  int dist = distances[a.size() + 1] - 1;
  
  //if equivalent graph dist is -1, return -1 not -2;
  return (dist != -2) ? dist : -1;

}
