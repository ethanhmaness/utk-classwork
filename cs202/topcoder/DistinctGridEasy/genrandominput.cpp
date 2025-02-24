#include <iostream>
#include <vector>
#include <string>
using namespace std;


int main()
{
  int i, j, n, k, tmp, row1, row2, col1, col2;
  vector < vector <int> > m;
  
  cin >> n >> k;
  srand48(time(0));

  m.resize(n);
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) m[i].push_back((j+i)%k);
  }

  for (i = 0; i < n; i++) {
    row1 = drand48() * n;
    row2 = drand48() * n;
    for (j = 0; j < n; j++) {
      tmp = m[row1][j];
      m[row1][j] = m[row2][j];
      m[row2][j] = tmp;
    }
    col1 = drand48() * n;
    col2 = drand48() * n;
    for (j = 0; j < n; j++) {
      tmp = m[j][col1];
      m[j][col1] = m[j][col2];
      m[j][col2] = tmp;
    }
  }

  if (drand48() < 0.33) {
    row1 = drand48() * n;
    row2 = drand48() * n;
    col1 = drand48() * n;
    col2 = drand48() * n;
    tmp = m[row1][col1];
    m[row1][col1] = m[row2][col2];
    m[row2][col2] = tmp;
  }

  printf("%d %d", n, k);
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) printf(" %d", m[i][j]); 
  }
  printf("\n");

  return 0;
}
