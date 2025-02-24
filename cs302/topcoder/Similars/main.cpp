#include "Similars.cpp"
#include <string.h>

void usage(string s)
{ 
  fprintf(stderr, "usage: a.out L R print-sets(y|n) print-similarities(y|n)\n");
  if (s != "") fprintf(stderr, "%s\n", s.c_str());
  exit(1);
}

int main(int argc, char **argv)
{
  int i;
  class Similars TheClass;
  int retval;
  int L;
  int R;

  if (argc != 5) usage("");

  if (sscanf(argv[1], "%d", &L) != 1 || L <= 0) usage("Bad L");
  if (sscanf(argv[2], "%d", &R) != 1 || R <= L) usage("Bad R");

  if (strcmp(argv[3], "y") == 0) TheClass.Print_Sets = "y";
  
  if (strcmp(argv[4], "y") == 0) TheClass.Print_Similarities = "y";

  retval = TheClass.maxsim(L, R);
  cout << retval << endl;

  exit(0);
}
