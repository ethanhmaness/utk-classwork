#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

class ThreeIncreasing {
  public:
    int minEaten(int a, int b, int c);
};

int ThreeIncreasing::minEaten(int a, int b, int c) {
	int eaten = 0;
	if(c < 3 || b < 2) return -1;
	while(!(a < b && b < c)){
		if(b >= c){
			eaten += b - c + 1;
			b = c - 1;
		}
		if(a >= b){
			eaten += a - b + 1;
			a = b - 1;
		}
	}
	if(a < 1 || b < 1 || c < 1) return -1;
	return eaten;
}
