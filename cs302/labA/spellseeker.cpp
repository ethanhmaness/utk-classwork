#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <cstdio>
using namespace std;

/*
  spellseeker.cpp
  Ethan Maness
  May 10, 2022
  
  Determines the path of max size for a given Spellseeker grid using a dynamic programming solution
*/

//Contains the answer for a particular call to SpellSeeker::Solve
class Answer {
  public:
    int length;
    int r;
    int c;
    string key;
    Answer *nexta;
};

//Implements the dynamic programming solution for solving the grid for the max path length
class SpellSeeker {
  public:
    vector <string> grid;
    map <string, Answer *> cache;
    int total_r;
    int total_c;

    Answer *Solve(int r, int c);
    string GenKey(int r, int c);
};

//determines if two chars are 1 apart
bool CanConnect(char a, char b){ return (a + 1 == b || a - 1 == b); }

//max of two ints
int Max(int a, int b){ return (a < b) ? b : a; }

//Returns a vector of (row,col) positions of adjacent cells for a rows x cols grid
vector< pair<int,int> > AdjacentCells(pair<int,int> pos, int rows, int cols){

  int row = pos.first;
  int col = pos.second;
  vector< pair<int,int> > cells;

  //cell directly above
  if(row > 0) cells.push_back(make_pair(row-1, col));
  //cell directly below
  if(row < rows-1) cells.push_back(make_pair(row+1, col));
  
  //cell to left
  if(col > 0) cells.push_back(make_pair(row, col-1));
  //cell to right
  if(col < cols-1) cells.push_back(make_pair(row, col+1));
  
  //diagonal cells
  if(col % 2 == 0){
    if(row < rows-1){
      //cell to bottom left
		  if(col > 0) cells.push_back(make_pair(row+1, col-1));
		  //cell to bottom right
		  if(col < cols-1) cells.push_back(make_pair(row+1, col+1));
    }
  } else {
	  if(row > 0){
      //cell to top left
		  if(col > 0) cells.push_back(make_pair(row-1, col-1));
		  //cell to top right
		  if(col < cols-1) cells.push_back(make_pair(row-1, col+1));
    }
  }
  
  return cells;

}

//Constructs a key unique to each starting pos and set of cells reachable from that pos
string SpellSeeker::GenKey(int row, int col){

  int rows = grid.size();
  int cols = grid[0].size();
  
  //start return string as all '-'
  string rv(rows*cols, '-');
  stack< pair<int,int> > dfs_stack;
  vector< pair<int,int> > adjacents;
  pair<int,int> pos = make_pair(row,col);
  pair<int,int> temp_pos;
  
  //starting cell is 'X'
  rv[row*cols + col] = 'X';
  
  //Depth First Search to set all cells accessible from starting cell to 'O'
  dfs_stack.push(pos);
  while(dfs_stack.size() > 0){
    pos = dfs_stack.top();
    dfs_stack.pop();
    if(rv[pos.first*cols + pos.second] == '-') rv[pos.first*cols + pos.second] = 'O';
    adjacents = AdjacentCells(pos, rows, cols);
    for(size_t i = 0; i < adjacents.size(); i++){
      temp_pos = adjacents[i];
      
      //only visit a node if it meets the char requirement and hasn't been visited
      if(CanConnect(grid[pos.first][pos.second], grid[temp_pos.first][temp_pos.second]) && rv[temp_pos.first*cols + temp_pos.second] == '-'){
				dfs_stack.push(temp_pos);
      }
    }
  }
  
  return rv;

}

//Recursive method for solving for the max path length
Answer* SpellSeeker::Solve(int row, int col){

  //get key for this subproblem
  string key = GenKey(row, col);

  //check cache for solution
  map<string, Answer*>::iterator mit = cache.find(key);
  if(mit != cache.end()) return mit->second;

  //initialize solution
  Answer* rv = new Answer();
  rv->r = row;
  rv->c = col;
  rv->key = key;
  rv->length = 1;
  
  pair<int,int> pos = make_pair(row,col);
  vector< pair<int, int> > adjacents = AdjacentCells(pos, grid.size(), grid[0].size());
  Answer* subAnswer;
  char temp;
  
  //recursively call on reachable cells
  for(size_t i = 0; i < adjacents.size(); i++){
    pos = adjacents[i];
    //if reachable, remove this char from grid, call Solve recursively, then add it back
    if(CanConnect(grid[row][col], grid[pos.first][pos.second])){
      temp = grid[row][col];
			grid[row][col] = '-';
      subAnswer = Solve(pos.first, pos.second);
      grid[row][col] = temp;
      
      //if this is the longest sub-path so far, set this as the child answer
      if(subAnswer->length + 1 > rv->length){
        rv->length = subAnswer->length + 1;
        rv->nexta = subAnswer;
      }
    
    }
  }
  
  cache.insert(make_pair(key,rv));
  return rv;

}

int main(){

  SpellSeeker ss;
  string row;
  Answer* ans = new Answer();
  Answer* tmp;
  ans->length = 0;
  
  //populate grid from stdin
  while(cin >> row){
    ss.grid.push_back(row);
  }
  
  //print grid
  for(size_t r = 0; r < ss.grid.size(); r++){
    for(size_t c = 0; c < ss.grid[r].size(); c++){
      cout << ss.grid[r][c];
    }
    cout << endl;
  }
  
  cout << "PATH" << endl;

  //calculate largest path
  for(size_t r = 0; r < ss.grid.size(); r++){
    for(size_t c = 0; c < ss.grid[r].size(); c++){
      tmp = ss.Solve(r,c);
      if(tmp->length > ans->length) ans = tmp;
    }
  }
  
  //print largest path
  while(1){
    cout << ans->r << " " << ans->c << endl;
    if(ans->length == 1) break;
    ans = ans->nexta;
  }
  
  return 0;

}