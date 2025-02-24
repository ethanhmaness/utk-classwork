#include "fraction.hpp"
#include <iostream>
using namespace std;

/* fraction.cpp
 * Ethan Maness
 * October 17, 2021
 *
 * This program implements a Fraction class from fraction.hpp that handles a numerator and denominator, each of which multiply positive integers.  Values repeated in both numerator and denominator are cancelled, and the decimal value of the fraction may be evaluated.
 */

//Clears numerator and denominator entirely
void Fraction::Clear(){
	numerator.clear();
	denominator.clear();
}

//Multiplies fraction by a number, either by adding value to numerator or removing from denominator
bool Fraction::Multiply_Number(int n){
	//check for valid value
	if(n <= 0) return false;
	//multiply by 1 does nothing
	if(n == 1) return true;
	
	multiset<int>::const_iterator it;
	
	//check denominator for value
	it = denominator.find(n);
	//if present, remove
	if(it != denominator.end()) denominator.erase(it);
	//otherwise, add to numerator
	else numerator.insert(n);

	return true;
}

//Divides fraction by a number, either by adding value to denominator or removing from numerator
bool Fraction::Divide_Number(int n){
	//check for valid value
	if(n <= 0) return false;
	//multiply by 1 does nothing
	if(n == 1) return true;
	
	multiset<int>::const_iterator it;
	
	//check numerator for value
	it = numerator.find(n);
	//if present, remove
	if(it != numerator.end()) numerator.erase(it);
	//otherwise, add to denominator
	else denominator.insert(n);
	
	return true;
}

//Multiplies the fraction by a factorial, by multiplying by every number from 2 to n (does nothing if n == 1)
bool Fraction::Multiply_Factorial(int n){
	//check valid input
	if(n <= 0) return false;

	//loop multiply from 2 to n
	for(int i = 2; i <= n; i++){
		//if multiply fails, return false
		if(!Multiply_Number(i)) return false;
	}
	return true;
}

//Divides the fraction by a factorial, by dividing by every number from 2 to n (does nothing if n == 1)
bool Fraction::Divide_Factorial(int n){
    //check valid input
	if(n <= 0) return false;
	//loop divide from 2 to n
    for(int i = 2; i <= n; i++){
		//if divide fails return false
        if(!Divide_Number(i)) return false;
    }
    return true;
}

//Multiplies the fraction by a binomial coefficient binom(n,k), through multiplying and dividing by three factorials
bool Fraction::Multiply_Binom(int n, int k){
	//check valid input
	if(n <= 0 || k < 0) return false;
	if(n < k) return false;
	//if n == 1 or k == 0, binom(n,k) == 1 so do nothing
	if(n == 1 || k == 0) return true;

	//multiply by n!, divide by (k!)(n-k)!
	//if any fail return false
	if(!Multiply_Factorial(n)) return false;
	if(!Divide_Factorial(k)) return false;
	if(!Divide_Factorial(n-k)) return false;

	return true;
}

//Divides the fraction by a binomial coefficient binom(n,k), through multiplying and dividing by three factorials
bool Fraction::Divide_Binom(int n, int k){
    //check valid input
    if(n <= 0 || k < 0) return false;
    if(n < k) return false;
    //if n == 1 or k == 0, binom(n,k) == 1 so do nothing
    if(n == 1 || k == 0) return true;

    //divide by n!, multiply by (k!)(n-k)!
    //if any fail return false
    if(!Divide_Factorial(n)) return false;
    if(!Multiply_Factorial(k)) return false;
    if(!Multiply_Factorial(n-k)) return false;

    return true;
}

//swaps numerator and denominator
void Fraction::Invert(){
	multiset<int> temp;

	//use temp storage var to swap numerator and denominator's values
	temp = numerator;
	numerator = denominator;
	denominator = temp;
}

//prints the fraction as an equation of * and /
void Fraction::Print() const{
	multiset<int>::const_iterator nit;

	//print vals of numerator multiplied (if no numerator, 1)
	if(numerator.size() == 0) cout << "1";
	else{
		//for each value in numerator
		for(nit = numerator.begin(); nit != numerator.end(); nit++){
			//if not first, put multiplication operator between
			if(nit != numerator.begin()) cout << " * ";
			cout << *nit;
		}
	}
	
	//print vals of denominator
	//for each value in denominator
	for(nit = denominator.begin(); nit != denominator.end(); nit++){
		//print division operator and value
		cout << " / " << *nit;
	}
	

	cout << endl;
}

double Fraction::Calculate_Product() const{
	double r;
	multiset<int>::const_iterator nit;
	
	r = 1;
	for(nit = numerator.begin(); nit != numerator.end(); nit++){
		r *= *nit;
	}
	for(nit = denominator.begin(); nit != denominator.end(); nit++){
		r /= *nit;
	}
	
	return r;
}
