#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

/*
  spellpath.cpp
  Ethan Maness
  May 10, 2022
  
  Determines the maximum path size for a given Spellseeker grid using a recursive solving method
*/

typedef vector<bool> Key;

//Staggered grid of cells each with one character
class Grid {
  public:
	  vector<string> rows;
	  vector< pair<int,int> > AdjacentCells(int row, int col);
};

//Implements the solution for solving the grid for the max path length
//(its in a class because i had a cache and then realized its way slower with a cache but im too lazy to remove the class from the code)
class SpellPath {
  public:
    size_t MaxPathLen(int row, int col, Grid G);
};

/*
  Key is bool vector
  0-31 are bits of row
  32-63 are bits of col
  64-(element count of grid + 63) indicate if each element a blank ('-')
*/
Key GenKey(int row, int col, Grid G){

  Key k;

  for(int i = 0; i < 32; i++){
    k.push_back((row << 31-i) >> 31 != 0);
  }
  for(int i = 0; i < 32; i++){
    k.push_back((col << 31-i) >> 31 != 0);
  }
  for(size_t r = 0; r < G.rows.size(); r++){
    for(size_t c = 0; c < G.rows[r].size(); c++){
      k.push_back(G.rows[r][c] == '-');
    }
  }
  
  return k;

}

//determines if two chars are 1 apart
bool CanConnect(char a, char b){ return (a + 1 == b || a - 1 == b); }

//max of two ints
int Max(int a, int b){ return (a < b) ? b : a; }

//Returns a vector of (row,col) positions of adjacent cells
vector< pair<int,int> > Grid::AdjacentCells(int row, int col){

  vector< pair<int,int> > cells;

  //cell directly above
  if(row > 0) cells.push_back(make_pair(row-1, col));
  //cell directly below
  if(row < rows.size()-1) cells.push_back(make_pair(row+1, col));
  
  //cell to left
  if(col > 0) cells.push_back(make_pair(row, col-1));
  //cell to right
  if(col < rows[row].size()-1) cells.push_back(make_pair(row, col+1));
  
  //diagonal cells
  if(col % 2 == 0){
    if(row < rows.size()-1){
      //cell to bottom left
		  if(col > 0) cells.push_back(make_pair(row+1, col-1));
		  //cell to bottom right
		  if(col < rows[row].size()-1) cells.push_back(make_pair(row+1, col+1));
    }
  } else {
	  if(row > 0){
      //cell to top left
		  if(col > 0) cells.push_back(make_pair(row-1, col-1));
		  //cell to top right
		  if(col < rows[row].size()-1) cells.push_back(make_pair(row-1, col+1));
    }
  }
  
  return cells;

}

//Recursive method for solving for the max path length
size_t SpellPath::MaxPathLen(int row, int col, Grid G){
  
  //find adjacent cells
  vector< pair<int,int> > adjacents = G.AdjacentCells(row, col);
  
  pair<int,int> pos;
  int submaxpath = 0;
  Grid Gtemp;
  
  //for each adjacent cell, if a valid path can connect the current and adjacent cell, run this method on the adjacent cell with this cell removed
  //save the max path length
  for(size_t i = 0; i < adjacents.size(); i++){
    pos = adjacents[i];
    if(CanConnect(G.rows[row][col], G.rows[pos.first][pos.second])){
      Gtemp = G;
      Gtemp.rows[row][col] = '-';
      submaxpath = Max(submaxpath, MaxPathLen(pos.first, pos.second, Gtemp));
    }
  }
  
  //max path is max sub path plus this cell
  return submaxpath + 1;

}

int main(){

  SpellPath sp;
  Grid grid;
  string row;
  size_t len = 0;
  
  //populate grid from stdin
  while(cin >> row){
    grid.rows.push_back(row);
  }

  //max of all start positions
  for(size_t r = 0; r < grid.rows.size(); r++){
    for(size_t c = 0; c < grid.rows[r].size(); c++){
      len = Max(len, sp.MaxPathLen(r, c, grid));
    }
  }
  
  cout << len << endl;
  
  return 0;

}