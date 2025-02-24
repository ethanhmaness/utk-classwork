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
  int n;
  set <int> s;
  map <double, int> m;

  cin >> n;

  srand48(time(0));
 
  while (s.size() < n) s.insert(drand48()*1000+1);

  while (s.size() > 0) {
    m[drand48()] = *s.begin();
    s.erase(s.begin());
  }

  while (m.size() > 0) {
    cout << " " << m.begin()->second;
    m.erase(m.begin());
  }

  cout << endl;

  return 0;
}
  

