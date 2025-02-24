/* TCO 2015 Q1A 250 - Similars */
/* James S. Plank */
/* Sat Feb 18 11:31:04 EST 2017 */

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

/* See the writeup for how I use Print_Sets and Print_Similarities */

class Similars {
  public:
    string Print_Sets;
    string Print_Similarities;
    int maxsim(int L, int R);
};

int Similars::maxsim(int L, int R)
{
  map <int, int> S;
  map <int, int>::iterator sit1, sit2, start, stop;
  vector <int> v;
  int i, j, k, s, sim, max, ok;

  /* For each number, calculate its set of digits as a bitmap */

  for (i = L; i <= R; i++) {
    j = i;
    s = 0;
    while (j != 0) {
      k = j % 10;
      s |= (1 << k);
      j /= 10;
    }

    S[s]++;
    if (Print_Sets == "y") {
      printf("i = %6d.  Set = 0x%03x.  Occurrences = %3d\n", i, s, S[s]);
    }
  }
  
  /* Calculate the similarities.  If an element's occurrences is more than 1,
     then just calculate its similarity to itself.  If it is 1, then you need
     to calculate its similarity with every other element from it to end. */

  max = 0;
  for (sit1 = S.begin(); sit1 != S.end(); sit1++) {
    start = sit1;               /* We'll iterate from start to stop. */
    if (sit1->second == 1) {
      start++;
      stop = S.end();
    } else {
      stop = start;
      stop++;
    }
    for (sit2 = start; sit2 != stop; sit2++) {
      if (sit2 != sit1 || sit1->second > 1) {
        s = (sit1->first & sit2->first);   /* This is the union */
        sim = 0;                           /* This is where we calculate the similarity */
        while (s != 0) {
          sim += (s&1);
          s >>= 1;
        }
        if (sim > max) max = sim;          /* Maintain the maximum. */
        if (Print_Similarities == "y") {
          printf("S1: 0x%03x  S2: 0x%03x  S1&S2: 0x%03x  Similarity: %d\n",
                 sit1->first, sit2->first, (sit1->first & sit2->first), sim);
        }
      }
    }
  }

  /* Return the maximum of these. */

  return max;
}
