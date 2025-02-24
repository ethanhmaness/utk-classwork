#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <map>
#include "disjoint_set.hpp"

/*
  sb-analyze.cpp
  Ethan Maness
  March 3, 2022

  Reads in parameters defining a Superball board as command-line arguments, and reads in the board on standard input. Prints out info for all scorable sets of squares.
*/

using namespace std;
using plank::Disjoint_Set;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

class Superball {
  public:
    Superball(int argc, char **argv);
    void analyze_superball();
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
}

//Performs Union operations on the colorsets Disjoint_Set to combine all indices of like colors that are touching into sets
void Superball::analyze_superball(){
  
  //Scan from left-to-right, top-to-bototm
  for(int i = 0; i < r * c; i++){
    
    //Union if pos to the right has a matching color
    if( (i+1) % c != 0 && colors[board[i]] != 0 && board[i] == board[i+1] && colorsets.Find(i+1) != colorsets.Find(i)) colorsets.Union(colorsets.Find(i+1), colorsets.Find(i));
    //Union if pos below has a matching color
    if(i < (r - 1) * c && colors[board[i]] != 0 && board[i] == board[i+c] && colorsets.Find(i+c) != colorsets.Find(i)) colorsets.Union(colorsets.Find(i+c), colorsets.Find(i));
    
  }
  
}

int main(int argc, char **argv)
{
  Superball *s;
  map<int, int> scoresets; //stores scorable sets from colorsets as <set id, scoreable cell index>
  map<int, int>::iterator mit;
  int id;
 
  s = new Superball(argc, argv);
  s->analyze_superball();

  //for every goal position, add any color sets with a non-empty color that intersect the position to the scoresets map
  for(int i = 0; i < s->r * s->c; i++){
    if(s->goals[i] == 1){
      id = s->colorsets.Find(i);
      if(id != -1 && s->colors[s->board[id]] != 0) scoresets.insert(make_pair(id, i));
    }
  }
  
  printf("Scoring sets:\n");
  
  //check every scoreset
  for(mit = scoresets.begin(); mit != scoresets.end(); mit++){
  
    //if its bigger than mss print it
    if(s->colorsets.Get_Sizes()->at(mit->first) >= s->mss)
      printf("  Size: %2d  Char: %c  Scoring Cell: %d,%d\n", s->colorsets.Get_Sizes()->at(mit->first), s->board[mit->second], mit->second / s->c, mit->second % s->c);
    
  }

  exit(0);
}