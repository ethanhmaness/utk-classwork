#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iterator>
using namespace std;

class Similars {
  public:
    string Print_Sets;
    string Print_Similarities;
    int maxsim(int L, int R);
};

unsigned short DigitFlags(int n){

	char digit;
	unsigned short digitflags = 0;

	for(int i = 1; i <= n; i*=10){

		digit = (n / i) % 10;
		digitflags |= 1 << digit;

	}

	return digitflags;

}

unsigned char SimilarBits(unsigned short a, unsigned short b){

	unsigned char r = 0;

	for(unsigned char i = 0; i < 16; i++){
		
		if(((a << i) >> 15) | ((b << i) >> 15) == 1) r++;

	}

	return r;

}

int Similars::maxsim(int L, int R)
{
	printf("L = %d.  R = %d.\n", L, R);
	if (Print_Sets == "y") printf("Print_Sets was set.\n");
	if (Print_Similarities == "y") printf("Print_Similarities was set.\n");

	map<unsigned short, unsigned int> occurrences;
	map<unsigned short, unsigned int>::iterator it1, it2;
	unsigned short maxSimilarity;
	unsigned short digitFlags;

	for(unsigned int i = L; i <= R; i++){
	
		digitFlags = DigitFlags(i);
		cout << "digitflags = " << digitFlags << endl;
		occurrences.insert(pair<unsigned short, unsigned int>(digitFlags, 0));
		occurrences.at(digitFlags) += 1;

	}

	for(it1 = occurrences.begin(); it1 != occurrences.end(); it1++){
		it2 = (it1->second < 2) ? next(it1) : it1;
		
		while(it2 != occurrences.end()){

			maxSimilarity = max(maxSimilarity, (unsigned short)SimilarBits(it1->first, it2->first));
			it2++;

		}

	}

	return maxSimilarity;

  return 0;
}
