#include "FindThePerfectTriangle.cpp"

int main(int argc, char **argv)
{
  int i;
  class FindThePerfectTriangle TheClass;
  vector <int> retval;
  int area;
  int perimeter;

  if (argc != 2) { fprintf(stderr, "usage: a.out num (or - to enter values on stdin)\n"); exit(1); }

  if ((string) argv[1] == "-") {
    cin >> area >> perimeter;
  } else {
  if (atoi(argv[1]) == 0) {
    area = 6;
    perimeter = 11;
  }

  if (atoi(argv[1]) == 1) {
    area = 6;
    perimeter = 12;
  }

  if (atoi(argv[1]) == 2) {
    area = 37128;
    perimeter = 882;
  }

  if (atoi(argv[1]) == 3) {
    area = 12;
    perimeter = 18;
  }

  if (atoi(argv[1]) == 4) {
    area = 18096;
    perimeter = 928;
  }

  if (atoi(argv[1]) == 5) {
    area = 1000000;
    perimeter = 1000;
  }
  }

  retval = TheClass.constructTriangle(area, perimeter);
  for(i = 0; i < (int) retval.size(); i++) cout << retval[i] << endl;

  exit(0);
}
