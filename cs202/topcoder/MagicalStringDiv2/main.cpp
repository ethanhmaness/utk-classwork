#include "MagicalStringDiv2.cpp"
#include <cstring>
int main(int argc, char **argv)
{
  int i;
  class MagicalStringDiv2 TheClass;
  int retval;
  string S;

  if (argc != 2) { fprintf(stderr, "usage: a.out num or - for stdin\n"); exit(1); }

  if (strcmp(argv[1], "-") == 0) {
    cin >> S;
  } else {

    if (atoi(argv[1]) == 0) {
       S = ">><<><";
    }
   
    if (atoi(argv[1]) == 1) {
       S = ">>>><<<<";
    }
   
    if (atoi(argv[1]) == 2) {
       S = "<<>>";
    }
   
    if (atoi(argv[1]) == 3) {
       S = "<><<<>>>>><<>>>>><>><<<>><><><><<><<<<<><<>>><><><";
    }
  }

  retval = TheClass.minimalMoves(S);
  cout << retval << endl;

  exit(0);
}
