#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

class Cryptography {
  public:
    long long encrypt(vector <int> numbers);
};

long long Cryptography::encrypt(vector <int> numbers)
{
  size_t LNI = 0;
  long long product = 1;

  for(size_t i = 0; i < numbers.size(); i++){
    if(numbers.at(i) < numbers.at(LNI)) LNI = i;
  }

  for(size_t i = 0; i < numbers.size(); i++){
    if(i == LNI) product *= numbers.at(i) + 1;
	else product *= numbers.at(i);
  }

  return product;
}
