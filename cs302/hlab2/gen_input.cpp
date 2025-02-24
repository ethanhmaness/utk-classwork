#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

int main()
{
  int vs;
  int seed;
  int i, x1, x2, y1, y2, type;
  int maxrec;

  cin >> seed;

  srand48(seed);

  vs = drand48() * 99 + 2;

  maxrec = drand48()*30000+10;
  for (i = 0; i < vs; i++) {
    x1 = drand48() * 40000;
    x2 = x1 + drand48() * maxrec + 1;
    if (x2 > 40000) x2 = 40000;
    y1 = drand48() * 40000;
    y2 = y1 + drand48() * maxrec + 1;
    if (y2 > 40000) y2 = 40000;
    type = drand48()*4+1;
    if (type == 1) type = drand48()*4+1;
    printf("%d %d %d %d %d\n", x1, y1, x2, y2, type);
  }

}
