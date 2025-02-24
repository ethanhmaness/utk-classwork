#include <iostream>
#include <string>
#include <vector>
#include "DistinctGridEasy.cpp"
using namespace std;

int main(int argc, char **argv)
{
  int i;
  class DistinctGridEasy TheClass;
  string retval;
  string a;
  int n;
  int k;
  int j;
  vector <int> grid;

  if (argc != 2) { fprintf(stderr, "usage: a.out num, or - to enter the grid on stdin\n"); exit(1); }

  a = argv[1];
  if (a == "-") {
    if (!(cin >> n)) return 0;
    if (!(cin >> k)) return 0;
    for (i = 0; i < n*n; i++) {
      if (!(cin >> j) || j < 0 || j >= k) return 0;
      grid.push_back(j);
    }
  } else {

 if (atoi(argv[1]) == 0) {
    n = 3;
    k = 3;
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(1);
  }

 if (atoi(argv[1]) == 1) {
    n = 3;
    k = 3;
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(0);
  }

 if (atoi(argv[1]) == 2) {
    n = 5;
    k = 2;
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
  }

 if (atoi(argv[1]) == 3) {
    n = 5;
    k = 3;
    grid.push_back(2);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(0);
  }

 if (atoi(argv[1]) == 4) {
    n = 7;
    k = 4;
    grid.push_back(3);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(3);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(3);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(1);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(1);
    grid.push_back(3);
    grid.push_back(0);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(0);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(3);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(1);
    grid.push_back(1);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(1);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(3);
    grid.push_back(1);
    grid.push_back(2);
    grid.push_back(0);
    grid.push_back(3);
    grid.push_back(2);
    grid.push_back(3);
  }
  }

  retval = TheClass.checkGrid(n, k, grid);
  cout << retval << endl;

  exit(0);
}
