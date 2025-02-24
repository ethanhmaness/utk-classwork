#include <vector>
#include <string>
#include <iostream>
using namespace std;

class SwappingDigits {
  public:
    string minNumber(string num);
};

string SwappingDigits::minNumber(string num) 
{
	string swp;
	size_t lgplacei = string::npos;
	size_t smplacei = string::npos;
	size_t smpinon0 = string::npos;

	for(char d = '0'; d <= '9'; d++){
		smplacei = num.rfind(d);
		if(smplacei != string::npos) d = '9'+1;
	}

	for(char d = '1'; d <= '9'; d++){
        smpinon0 = num.rfind(d);
        if(smpinon0 != string::npos) d = '9'+1;
    }

	if(smplacei == string::npos || smplacei == 0) return num;

	for(size_t i = 0; i < smplacei; i++){
		if(num.at(i) > num.at(smplacei)){
			lgplacei = i;
			i = smplacei;
		}
	}

	if(lgplacei == 0 && num.at(smplacei) == '0') smplacei = smpinon0;

	if(smplacei == string::npos || smplacei == 0) return num;
	
	if(lgplacei == string::npos || lgplacei == num.size() - 1) return num;
	
	if(num.at(smplacei) == num.at(lgplacei)) return num;

	swp = num.substr(0,lgplacei) + num.at(smplacei) + num.substr(lgplacei + 1, smplacei - lgplacei - 1) + num.at(lgplacei);
	if(smplacei != num.size() - 1) swp += num.substr(smplacei + 1, num.size() - smplacei - 1);

	return swp;

}
