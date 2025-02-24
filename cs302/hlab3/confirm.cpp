#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int main()
{
  long long area, per, x1, y1, x2, y2, x3, y3;
  long long x, y, p, l, ls, s1, s2, s3, as;

  if (! ( cin >> area >> per >> x1 >> y1 >> x2 >> y2 >> x3 >> y3)) {
    printf("Bad input\n");
    exit(0);
  }
  
  p = 0;

  x = x1 - x2;
  y = y1 - y2;
  ls = x*x + y*y;
  l = sqrt(ls);
  if (l*l != ls) {
    printf("Length not an integer.\n");
    exit(0);
  }
  s1 = l;
  p += l;

  x = x1 - x3;
  y = y1 - y3;
  ls = x*x + y*y;
  l = sqrt(ls);
  if (l*l != ls) {
    printf("Length not an integer.\n");
    exit(0);
  }
  s2 = l;
  p += l;

  x = x2 - x3;
  y = y2 - y3;
  ls = x*x + y*y;
  l = sqrt(ls);
  if (l*l != ls) {
    printf("Length not an integer.\n");
    exit(0);
  }
  s3 = l;
  p += l;

  if (p != per) { printf("Perimeters don't match\n"); exit(0); }
  
  if (p % 2 != 0) { printf("Areas don't match\n"); exit(0); }
  p /= 2;
  as = p * (p-s1) * (p - s2) * (p - s3);
  if (area * area != as) { printf("Areas don't match\n"); exit(0); }
  printf("Good\n");
  exit(0);
}

