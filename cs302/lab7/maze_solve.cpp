#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

/*
  maze_solve.cpp
  Ethan Maness
  April 3, 2022
  
  Reads in a formatted maze on standard input, reprints the maze, and then prints a path of cell indices to follow to solve the maze.
  May not be the shortest solution for mazes with multiple valid paths.
*/

//simple function for appending a vector in-line because C++98 sucks and doesn't have lambda expressions >:(
vector<int> v_append(vector<int> v, int e){
  v.push_back(e);
  return v;
}

//Defines a maze, with methods for reading, writing, and solving
class Maze {
  public:
    int r;
    int c;
    set< pair<int,int> > walls;
    void Read();
    void Print();
    vector<int> DFS_Solve(vector<int> path);
};

//Read in formatted maze from stdin
void Maze::Read(){

  string s;
  int a1, a2;

  //maze size parameters (return if invalid)
  if(!(cin >> s && s == "ROWS")) return;
  if(!(cin >> r)) return;
  if(!(cin >> s && s == "COLS")) return;
  if(!(cin >> c)) return;
  
  //read in walls (sort pairs for convenience)
  while(cin >> s && s == "WALL" && cin >> a1 && cin >> a2){
    if(a1 <= a2) walls.insert(make_pair(a1, a2));
    else walls.insert(make_pair(a2, a1));
  }
  
}

//Print formatted maze on stdout
void Maze::Print(){

  set< pair<int,int> >::iterator wit;

  //size
  printf("ROWS %d COLS %d\n", r, c);
  
  //walls ordered by first index
  for(wit = walls.begin(); wit != walls.end(); wit++){
    printf("WALL %d %d\n", wit->first, wit->second);
  }
  
}

/*
  Recursive DFS procedure
  If path already contains the goal cell, it simply returns path
  For each unvisited unblocked cell adjacent to the last cell in path, it calls itself with a path that has that cell appended
  If it solves the maze, return it
  If none solve the maze, return an empty vector
*/
vector<int> Maze::DFS_Solve(vector<int> path){
  
  vector<int> tmp;
  
	//last cell in path is the current
	int current_cell = path.back();
	
	//if solved, change nothing
  if(current_cell == r * c - 1) return path;
  
  /* Checks adjacent squares if there is not a wall blocking and if the square is not previously visited */
  
  //check right
  if((current_cell + 1) % c != 0 && walls.find(make_pair(current_cell, current_cell + 1)) == walls.end() && find(path.begin(), path.end(), current_cell + 1) == path.end()){
		tmp = DFS_Solve(v_append(path, current_cell + 1));
    if(tmp.size() != 0 && tmp.back() == r * c - 1) return tmp;
  }
  
  //check bottom
  if(current_cell < (r-1) * c && walls.find(make_pair(current_cell, current_cell + c)) == walls.end() && find(path.begin(), path.end(), current_cell + c) == path.end()){
		tmp = DFS_Solve(v_append(path, current_cell + c));
    if(tmp.size() != 0 && tmp.back() == r * c - 1) return tmp;
  }
  
  //check left
  if(current_cell % c != 0 && walls.find(make_pair(current_cell - 1, current_cell)) == walls.end() && find(path.begin(), path.end(), current_cell - 1) == path.end()){
		tmp = DFS_Solve(v_append(path, current_cell - 1));
    if(tmp.size() != 0 && tmp.back() == r * c - 1) return tmp;
  }
  
  //check top
  if(current_cell >= c && walls.find(make_pair(current_cell - c, current_cell)) == walls.end() && find(path.begin(), path.end(), current_cell - c) == path.end()){
		tmp = DFS_Solve(v_append(path, current_cell - c));
    if(tmp.size() != 0 && tmp.back() == r * c - 1) return tmp;
  }
  
  return vector<int>();
  
}

//Uses Maze class to build, print, and solve a maze from stdin
int main(){

  vector<int> path;

  Maze maze;

  maze.Read();
  maze.Print(); 

  //solve
  path = maze.DFS_Solve(vector<int>(1, 0));
  for(size_t i = 0; i < path.size(); i++){
    printf("PATH %d\n", path[i]);
  }

  return 0;
}