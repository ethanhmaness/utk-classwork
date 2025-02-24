#include <vector>
#include <math.h>
#include "city_map.hpp"

/*
  city_map.cpp
  Ethan Maness
  April 10, 2022
  
  Implements the constructor and pathfinding algorithm methods for city_map.hpp, which allows for finding the fastest route through a city given a constant speed limit and traffic light timings
*/

//for building a Road_Segment (I just didn't want to type out the assignment lines 4 times)
Road_Segment* Build_Road(Road_Type type, int number, double distance, Intersection* from, Intersection* to){
  Road_Segment* road = new Road_Segment();
  road->type = type;
  road->number = number;
  road->distance = distance;
  road->from = from;
  road->to = to;
  return road;
}

//calculates the wait time for the light at the end of a road segment given the scenario (avg/best/worst)
double Wait_Time(Road_Segment road, int scenario){

  if(scenario == 'B') return 0;

  double red_time = (road.type == STREET) ? road.to->green[1] : road.to->green[0];
  
  if(scenario == 'W') return red_time;
  
  double green_time = (road.type == STREET) ? road.to->green[0] : road.to->green[1];
  
  if(scenario == 'A') return red_time * red_time / (2 * (red_time + green_time));
  
  //this should never run
  return -1;

}

//calculates distance between two intersections
double Dist(Intersection* i1, Intersection* i2){

  return sqrt(pow(i1->x - i2->x, 2) + pow(i1->y - i2->y, 2));

}

/*
  Constructor
*/
City_Map::City_Map(){

  //vars
  int street;
  int avenue;
  double x;
  double y;
  double green_street;
  double green_avenue;
  Intersection* intersection;
  Intersection* from;
  Intersection* to;
  list<Intersection*>::iterator lit;
  vector< vector<Intersection*> > grid; //intersections stored at (avenue, street)
  Road_Segment* road;
  
  //read in lines of intersections
  while(cin >> street && cin >> avenue && cin >> x && cin >> y && cin >> green_street && cin >> green_avenue){
  
    intersection = new Intersection();
    intersection->street = street;
    intersection->avenue = avenue;
    intersection->x = x;
    intersection->y = y;
    intersection->green[0] = green_street;
    intersection->green[1] = green_avenue;
    
    //insert intersections into list
    all.push_back(intersection);
    
    //create grid of intersections for constructing adjacency list
    if((int) grid.size() < avenue + 1) grid.resize(avenue + 1);
    if((int) grid[avenue].size() < street + 1) grid[avenue].resize(street + 1);
    grid[avenue][street] = intersection;
    
    //store first and last streets based on road numbers
    if(street == 0 && avenue == 0) first = intersection;
    if(last == NULL || (street >= last->street && avenue >= last->avenue)) last = intersection;
    
  }
  
  //create adjacency list for every intersection
  for(size_t a = 0; a < grid.size(); a++){
    for(size_t s = 0; s < grid[a].size(); s++){
    
      from = grid[a][s];
        
      //eastward edge (odd and mod-5 streets)
      if(a > 0 && (s % 2 == 1 || s % 5 == 0)){
        to = grid[a-1][s];
        road = Build_Road(STREET, s, Dist(from, to), from, to);
        from->adj.push_back(road);
      }
        
      //westward edge (even and mod-5 streets)
      if(a < grid.size() - 1 && (s % 2 == 0 || s % 5 == 0)){
        to = grid[a+1][s];
        road = Build_Road(STREET, s, Dist(from, to), from, to);
        from->adj.push_back(road);
      }
      
      //southward edge (odd, mod-5, and highest avenues)
      if(s > 0 && (a % 2 == 1 || a % 5 == 0 || a == grid.size() - 1)){
        to = grid[a][s-1];
        road = Build_Road(AVENUE, a, Dist(from, to), from, to);
        from->adj.push_back(road);
      }
      
      //northward edge (even, mod-5, and highest avenues)
      if(s < grid[a].size() - 1 && (a % 2 == 0 || a % 5 == 0 || a == grid.size() - 1)){
        to = grid[a][s+1];
        road = Build_Road(AVENUE, a, Dist(from, to), from, to);
        from->adj.push_back(road);
      }
      
    }
  }

}

/*
  Implementation of Dijkstra's Algorithm based on scenario ('B' = best, 'W' = worst, 'A' = avg)
*/
double City_Map::Dijkstra(int scenario){
  
  Intersection* intersection;
  Intersection* to;
  Road_Segment* road;
  vector< vector<Intersection*> > grid;
  vector< vector<bool> > visited;
  vector<bool> temp;
  list<Intersection*>::iterator lit;
  list<Road_Segment*>::iterator alit;
  multimap<double, Intersection*>::iterator mit;
  double d;
  
  //initialize all intersections to backedge NULL, distance -1, and store it in the grid
  for(lit = all.begin(); lit != all.end(); lit++){
  
    intersection = *lit;
    intersection->backedge = NULL;
    intersection->best_time = -1;
    intersection->bfsq_ptr = bfsq.end();
    
    if((int) grid.size() < intersection->avenue + 1) grid.resize(intersection->avenue + 1);
    if((int) grid[intersection->avenue].size() < intersection->street + 1) grid[intersection->avenue].resize(intersection->street + 1);
    grid[intersection->avenue][intersection->street] = intersection;
  
  }
  
  //set visited grid to all false
  temp.resize(grid[0].size(), false);
  visited.resize(grid.size(), temp);
  
  //Start with 0,0
  intersection = grid[0][0];
  intersection->best_time = 0;
  intersection->bfsq_ptr = bfsq.insert(make_pair(0, intersection));
  
  //Dijkstra's Algorithm loop
  while(bfsq.size() > 0){
  
    //remove node from multimap and set visited to true
    intersection = bfsq.begin()->second;
    bfsq.erase(bfsq.begin());
    visited[intersection->avenue][intersection->street] = true;
    
    //go through its edges
    for(alit = intersection->adj.begin(); alit != intersection->adj.end(); alit++){
    
      road = *alit;
    
      //to = intersection at end of edge
      to = road->to;
      
      //skip if this intersection is already already visited
      if(visited[to->avenue][to->street]) continue;
      
      //time along this path is time to intersection plus edge time (speed is 1 mi per 120 sec)
      d = intersection->best_time + (road->distance * 120) + Wait_Time(*road, scenario);
    
      //if target intersection has no time set or this path is faster than the current best time
      if(to->best_time == -1 || d < to->best_time){
      
        //remove from multimap if it is in it
        if(to->bfsq_ptr != bfsq.end()) bfsq.erase(to->bfsq_ptr);
        
        //update time and backedge
        to->best_time = d;
        to->backedge = road;
        
        //insert into multimap
        to->bfsq_ptr = bfsq.insert(make_pair(d, to));
      
      }
    
    }
  
  }
  
  //Construct path from backedges
  path.push_front(grid[grid.size()-1][grid[0].size()-1]->backedge);
  while(1){
    if(path.front()->from == grid[0][0]) break;
    path.push_front(path.front()->from->backedge);
  }
  
  return path.back()->to->best_time;
  
}