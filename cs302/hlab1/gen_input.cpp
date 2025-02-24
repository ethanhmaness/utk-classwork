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
  long long v, b;
  int i;

  cin >> seed;

  b = 1000000001LL;

  srand48(seed);

  vs = drand48() * 99 + 2;

  for (i = 0; i < vs; i++) cout << " " << (long long) (drand48()* (2*b) - b);
  cout << endl;

  for (i = 0; i < vs; i++) cout << " " << (long long) (drand48()* (2*b) - b);
  cout << endl;

  for (i = 0; i < vs; i++) cout << " " << (long long) (drand48()* (b/1000) + 1);
  cout << endl;

  cout << " " << (long long) (drand48()*b+1);
  cout << endl;
  return 0;
}
  
  
