#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
#include <climits>
#include <cmath>
#include <list>
#include "disjoint_set.hpp"

/*
  sb-play.cpp
  Ethan Maness
  March 3, 2022

  This program uses a multi-step algorithm to determine a good turn to take for a Superball board of the given parameters with the board read in on standard input.
  It can predict and attempt to avoid the game ending on the next turn.
  It can calculate the optimal swap to make to maximize the sum of the sizes of color cell groups that intersect a goal, and score a group if no swap is good
  It can expand the largest cell towards a goal if there is no existing cell touching a goal that can be expanded
*/

using namespace std;
using plank::Disjoint_Set;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

class Superball {
  public:
    Superball();
    Superball(int argc, char **argv);
    Superball(Superball* oldSB, int i1, int i2);
    void combine_colorsets();
    int colorset_pts(int id);
    int adjacent_colored_goal_cell(int i);
    bool contains_all_of_same_color(int id);
    int cell_sets_sum(set<int> ids);
    int scoresets_sum();
    int net_gain(int i1, int i2);
    int closest_cell_to_cell(int id, set<int> ids);
    int closest_cell_to_cells(set<int> from, set<int> to);
    set<int> adjacent_cells(int i);
    string make_move();
    int r;
    int c;
    int mss;
    int empty;
    vector <int> board;
    vector <int> goals;
    vector <int> colors;
    Disjoint_Set colorsets;
};

void usage(const char *s) 
{
  fprintf(stderr, "usage: sb-read rows cols min-score-size colors\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

Superball::Superball(Superball* oldSB, int i1, int i2){

  int tmp;

  this->r = oldSB->r;
  this->c = oldSB->c;
  this->mss = oldSB->mss;
  this->empty = oldSB->empty;
  this->board = oldSB->board;
  this->goals = oldSB->goals;
  this->colors = oldSB->colors;
  
  tmp = board[i1];
  board[i1] = board[i2];
  board[i2] = tmp;
  
  colorsets.Initialize(r*c);
  combine_colorsets();

}

Superball::Superball(int argc, char **argv)
{
  int i, j;
  string s;

  if (argc != 5) usage(NULL);

  if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0) usage("Duplicate color");
    colors[argv[4][i]] = 2+i;
    colors[toupper(argv[4][i])] = 2+i;
  }

  board.resize(r*c);
  goals.resize(r*c, 0);
  colorsets.Initialize(r*c);

  empty = 0;

  for (i = 0; i < r; i++) {
    if (!(cin >> s)) {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if (s.size() != c) {
      fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      exit(1);
    }
    for (j = 0; j < c; j++) {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        exit(1);
      }
      board[i*c+j] = s[j];
      if (board[i*c+j] == '.') empty++;
      if (board[i*c+j] == '*') empty++;
      if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
        goals[i*c+j] = 1;
        board[i*c+j] = tolower(board[i*c+j]);
      }
    }
  }
  
  combine_colorsets();
  
}

//Performs Union operations on the colorsets Disjoint_Set to combine all indices of like colors that are touching into sets
void Superball::combine_colorsets(){
  
  //Scan from left-to-right, top-to-bototm
  for(int i = 0; i < r * c; i++){
    
    //Union if pos to the right has a matching color
    if( (i+1) % c != 0 && colors[board[i]] != 0 && board[i] == board[i+1] && colorsets.Find(i+1) != colorsets.Find(i)) colorsets.Union(colorsets.Find(i+1), colorsets.Find(i));
    //Union if pos below has a matching color
    if(i < (r - 1) * c && colors[board[i]] != 0 && board[i] == board[i+c] && colorsets.Find(i+c) != colorsets.Find(i)) colorsets.Union(colorsets.Find(i+c), colorsets.Find(i));
    
  }
  
}

//Calculates the points of a set
int Superball::colorset_pts(int id){
  int cellpts = colors[board[id]];
  int size = colorsets.Get_Sizes()->at(id);
  return cellpts * size;
}

//Determines if a cell is adjacent to a non-empty goal cell, and returns its index if so
int Superball::adjacent_colored_goal_cell(int i){
  set<int> adjacent = adjacent_cells(i);
  set<int>::iterator sit;
  for(sit = adjacent.begin(); sit != adjacent.end(); sit++){
    if(colors[board[*sit]] != 0 && goals[*sit] == 1) return *sit;
  }
  return -1;
}

//Determines if a color set contains all cells of its color
bool Superball::contains_all_of_same_color(int id){
  int color = board[id];
  for(int i = 0; i < r * c; i++){
    if(board[i] == color && colorsets.Find(i) != id) return false;
  }
  return true;
}

//Calculates the point sum of all of the given sets
int Superball::cell_sets_sum(set<int> ids){
  int sum = 0;
  set<int>::iterator sit;
  for(sit = ids.begin(); sit != ids.end(); sit++){
    sum += colorset_pts(*sit);
  }
  return sum;
}

//sums the point values of all sets that intersect a goal
int Superball::scoresets_sum(){

  set<int> scoresets; //stores scorable sets from colorsets as <set id, scoreable cell index>
  int id;
  
  //for every goal position, add any color sets with a non-empty color that intersect the position to the scoresets map
  for(int i = 0; i < r * c; i++){
    if(goals[i] == 1){
      id = colorsets.Find(i);
      if(id != -1 && colors[board[id]] != 0) scoresets.insert(id);
    }
  }
  
  //return sum of point totals
  return cell_sets_sum(scoresets);
  
}

//Calculates the difference in the sum of the scoresets of a swap's resulting board and that of the current board
int Superball::net_gain(int i1, int i2){
  Superball* rsb = new Superball(this, i1, i2); //creates new board with indices swapped
  int r = rsb->scoresets_sum() - scoresets_sum();
  return r;
}

//calculates the closest cell of a set of cells to a given cell, by taxicab distance
int Superball::closest_cell_to_cell(int id, set<int> ids){
  
  int dist;
  int min_dist;
  int closest = -1;
  set<int>::iterator sit;
  
  min_dist = INT_MAX;
  for(sit = ids.begin(); sit != ids.end(); sit++){
    dist = abs((*sit / c) - (id / c)) + abs((*sit % c) - (id % c));
    if(dist < min_dist){
      min_dist = dist;
      closest = *sit;
    }
  }
  
  return closest;
  
}

//calculates the closest cell from a set of cells to another set of cells, by taxicab distance
int Superball::closest_cell_to_cells(set<int> from, set<int> to){

  int dist;
  int min_dist;
  int closest = -1;
  set<int>::iterator sit;
  for(sit = from.begin(); sit != from.end(); sit++){
    dist = abs((closest_cell_to_cell(*sit, to) / c) - (*sit / c)) + abs((closest_cell_to_cell(*sit, to) % c) - (*sit % c));
    if(dist < min_dist){
      min_dist = dist;
      closest = *sit;
    }
  }
  
  return closest;
  
}

set<int> Superball::adjacent_cells(int i){
  set<int> adjacent;
  if( (i+1) % c != 0 ) adjacent.insert(i+1);
  if( i % c != 0     ) adjacent.insert(i-1);
  if( i < (r-1) * c  ) adjacent.insert(i+c);
  if( i > c - 1      ) adjacent.insert(i-c);
  return adjacent;
}

//Determines a move to make on the next turn for this board and returns the corresponding line for sb-player
string Superball::make_move(){

  map<int, int> scoresets; //stores sets intersecting the goal from colorsets as <set id, scoreable cell index>
  map<int, int> scoreable_scoresets; //stores scoresets of large enough size
  map<int, int>::iterator mit;
  stringstream rs;
  int id;
  int largest;
  int most_valuable;
  bool flag;
  map<int, int> potential_scoresets; //stores ids of colorsets that can be scoresets after this turn as <id, adjacent colored goal cell>
  int adjacent_cell;
  set<int> adjacents;
  set<int> scoreset_peripheries; //stores sets of each colored cell adjacent to a scoreset, indexed on the scoreset's id
  set<int>::iterator sit;
  map<pair<int, int>, int> swap_candidates; //stores sets of sorted pairs of swap indexes and their respective possible net score gains
  map<pair<int, int>, int>::iterator mpit;
  pair<int, int> swap_pair;
  int maxgain;
  int potential_score;
  int i1, i2;
  set<int> periphery;
  set<int> goalcells;
  int smallest;

  //for every goal position, add any color sets with a non-empty color that intersect the position to the scoresets map
  for(int i = 0; i < r * c; i++){
    if(goals[i] == 1){
      id = colorsets.Find(i);
      if(id != -1 && colors[board[id]] != 0) scoresets.insert(make_pair(id, i));
    }
  }
  
/*
  PANIC MODE - When the game will end from overcrowding unless enough cells are cleared
*/
  
  if(empty <= 5){
  
    for(mit = scoresets.begin(); mit != scoresets.end(); mit++){
      if(colorsets.Get_Sizes()->at(mit->first) >= mss) scoreable_scoresets.insert(*mit);
    }
  
    //score if possible
    if(scoreable_scoresets.size() != 0){
    
      //determine if some score would prevent the game from ending
      flag = false;
      largest = -1;
      most_valuable = -1;
      for(mit = scoreable_scoresets.begin(); mit != scoreable_scoresets.end(); mit++){
        if(colorsets.Get_Sizes()->at(mit->first) + empty > 5){
          flag = true;
        }
        if(largest == -1 || colorsets.Get_Sizes()->at(mit->first) > colorsets.Get_Sizes()->at(largest)) largest = mit->first;
        if(most_valuable == -1 || colorset_pts(mit->first) > colorset_pts(most_valuable)) most_valuable = mit->first;
      }
      
      //if a score can clear enough cells to prevent the game ending, score the largest set
      if(flag){
      
        rs << "SCORE " << (scoreable_scoresets[largest] / c) << " " << (scoreable_scoresets[largest] % c) << "\n";
        return rs.str();
      
      //if no score will prevent the game ending, score the most valuable set
      } else {
      
        rs << "SCORE " << (scoreable_scoresets[most_valuable] / c) << " " << (scoreable_scoresets[most_valuable] % c) << "\n";
        return rs.str();
      
      }
    
    //otherwise swap so the game ends
    } else {
    
	  i1 = -1;
	  i2 = -1;
	  for(int i = 0; i < r * c; i++){
        if(colors[board[i]] != 0){
          if(i1 == -1) i1 = i;
		  else{
		    i2 = i;
			break;
		  }
		}
      }
      rs << "SWAP " << (i1 / c) << " " << (i1 % c) << " " << (i2 / c) << " " << (i2 % c) << "\n";
	  return rs.str();
      
    }
    
  }
  
/*
  GROWING MODE - grows sets into scoresets for points
*/

  //Create map of all ids of colorsets that can be scoresets after this turn but are not yet, and the cell that can be changed to make them a scoreset
  for(int i = 0; i < r * c; i++){
  
    //make sure its not already in a scoreset
    if(scoresets.find(colorsets.Find(i)) == scoresets.end()){
  
      //only consider non-empty cells that are adjacent to a colored goal cell
      adjacent_cell = adjacent_colored_goal_cell(i);
	    if(colors[board[i]] != 0 && adjacent_cell != -1){
	    
	      //if it is possible for the goal cell to become the matching color, add the colorset to the map
	      id = colorsets.Find(i);
	      if(!contains_all_of_same_color(id)){
	        potential_scoresets.insert(make_pair(id, adjacent_cell));
		  }
	    
	    }
    
    }
  
  }

  //Create sets of adjacent colored cells to each scoreset and store them in a map indexed on the scoreset
  for(int i = 0; i < r * c; i++){
    
    //if this cell is colored and is adjacent to a cell that is in a different scoreset, insert it into the scoreset_peripheries set
    if(colors[board[i]] != 0){
      
      flag = false;
      adjacents = adjacent_cells(i);
      for(sit = adjacents.begin(); sit != adjacents.end(); sit++){
        if(scoresets.find(*sit) != scoresets.end() && scoresets.find(*sit) != scoresets.find(i)){
		  flag = true;
		}
      }
      
      if(flag) scoreset_peripheries.insert(i);
		  
    }
    
  }
  
  //Calculate greatest net gain of points of each swap for creating new scoresets and insert into map
  for(mit = potential_scoresets.begin(); mit != potential_scoresets.end(); mit++){
    
    i1 = mit->second;
    
    for(i2 = 0; i2 < r * c; i2++){
      
      if(i2 != i1 && colors[board[i2]] != 0 && board[i1] != board[i2] && swap_candidates.find(minmax(i1,i2)) == swap_candidates.end()){
        
        swap_candidates.insert(make_pair(minmax(i1,i2), net_gain(i1, i2)));
	  }
      
    }
    
  }
  
  //Calculate greatest net gain of points of each swap for expanding existing scoresets and insert into map
  for(sit = scoreset_peripheries.begin(); sit != scoreset_peripheries.end(); sit++){
      
	  i1 = *sit;
	  
	  for(i2 = 0; i2 < r * c; i2++){
	  
	    if(i2 != i1 && colors[board[i2]] != 0 && board[i1] != board[i2] && swap_candidates.find(minmax(i1,i2)) == swap_candidates.end()){
	    
	      swap_candidates.insert(make_pair(minmax(i1,i2), net_gain(i1, i2)));
	    
	    }
	  
	  }
    
  }
  
  //Determine the swap pair with the best resulting net potential point gain
  swap_pair = make_pair(-1, -1);
  maxgain = INT_MIN;
  for(mpit = swap_candidates.begin(); mpit != swap_candidates.end(); mpit++){
   	if(swap_pair.first == -1 || mpit->second > maxgain){
      swap_pair = mpit->first;
      maxgain = mpit->second;
    }
  }
  
  //Find the highest-point scoreable scoreset
  most_valuable = -1;
  potential_score = 0;
  for(mit = scoresets.begin(); mit != scoresets.end(); mit++){
    if(colorsets.Get_Sizes()->at(mit->first) >= mss && (most_valuable == -1 || colorset_pts(mit->first) > colorset_pts(most_valuable))){
	  most_valuable = mit->first;
	  potential_score = colorset_pts(most_valuable);
	}
  }


  //If the gain is positive, do the swap
  if(maxgain > 0){
    rs << "SWAP " << (swap_pair.first / c) << " " << (swap_pair.first % c) << " " << (swap_pair.second / c) << " " << (swap_pair.second % c) << "\n";
    return rs.str();
  }
  
  //Otherwise, score the highest-point scoreable scoreset if there is one
  if(most_valuable != -1){
    rs << "SCORE " << (scoresets[most_valuable] / c) << " " << (scoresets[most_valuable] % c) << "\n";
    return rs.str();
  }
   
/*
  GRAVITATING MODE - if this point is reached, no growable scoresets exist, and no scorable scoresets exist or can be made (or it would be detrimental to do the swap necessary), so we will just grow the largest colorset towards a goal by shrinking the smallest of its color
*/

  //find the largest colorset (biased towards sets that are adjacent to a colored goal cell)
  largest = -1;
  for(int i = 0; i < r * c; i++){
    if(colorsets.Find(i) != i) continue;
    if(colors[board[i]] != 0 && (largest == -1 || colorsets.Get_Sizes()->at(i) + ((adjacent_colored_goal_cell(i) != -1) ? 0 : 1) > colorsets.Get_Sizes()->at(largest))) largest = i;
  }

  //find the smallest colorset of the same color as largest
  smallest = -1;
  for(int i = 0; i < r * c; i++){
    if(colorsets.Find(i) != i) continue;
    if(colors[board[i]] == colors[board[largest]] && (smallest = -1 || colorsets.Get_Sizes()->at(i) < colorsets.Get_Sizes()->at(smallest))) smallest = i;
    if(smallest != -1 && colorsets.Get_Sizes()->at(smallest) == 1) break;
  }
    
  //if no other colorset matches largest's color, just pick the smallest other colorset
  if(smallest == largest){
    smallest = -1;
    for(int i = 0; i < r * c; i++){
      if(colorsets.Find(i) != i || i == largest) continue;
      if(colors[board[i]] != 0 && (smallest = -1 || colorsets.Get_Sizes()->at(i) <= colorsets.Get_Sizes()->at(smallest))) smallest = i;
      if(smallest != -1 && colorsets.Get_Sizes()->at(smallest) == 1) break;
    }
  }

  for(int i = 0; i < r * c; i++){

    //construct set of goal cells
    if(goals[i] == 1) goalcells.insert(i);
    //if this cell is colored and not in the largest colorset, but is adjacent to a cell that is, insert it into the periphery set
    if(colors[board[i]] != 0 && colorsets.Find(i) != largest){
      
      flag = false;
      adjacents = adjacent_cells(i);
      for(sit = adjacents.begin(); sit != adjacents.end(); sit++){
        if(colorsets.Find(*sit) == largest) flag = true;
      }
      
      if(flag) periphery.insert(i);
		  
    }
  }
  
  //swap the closest colored cell around largest colorset to the goal with a cell from the smallest colorset
  i1 = closest_cell_to_cells(periphery, goalcells);
  i2 = smallest;
  
  //if no periphery, just swap the two to make to game continue
  if(i1 == -1) i1 = largest;
  
  rs << "SWAP " << (i1 / c) << " " << (i1 % c) << " " << (i2 / c) << " " << (i2 % c) << "\n";
  return rs.str();
  
}

int main(int argc, char **argv)
{
  Superball *s;
 
  s = new Superball(argc, argv);

  cout << s->make_move();

  exit(0);
  
}
